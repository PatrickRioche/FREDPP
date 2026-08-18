#include <jni.h>

#include "HelpManager.h"
#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/flow/CommandInputExpansion.hpp"
#include "fred/flow/FlowCharacterStream.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"
#include "fred/runtime/ProcedureRunner.hpp"
#include "fredpp/version.hpp"

#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

struct AndroidSession {
    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context;
    fred::CommandRegistry registry;
    fred::CommandExecutor executor;
    fred::ProcedureRunner runner;

    std::unique_ptr<fred::CommandNode> pending_text_command;
    std::vector<std::string> pending_text_lines;
    std::deque<std::unique_ptr<fred::CommandNode>> queued_commands;
    std::mutex mutex;

    AndroidSession()
        : context(buffers, output),
          registry(fred::make_core_command_registry()),
          runner(buffers, context, registry, executor) {}
};

AndroidSession* from_handle(jlong handle) {
    if (handle == 0) {
        throw std::runtime_error("session Android FREDPP invalide");
    }
    return reinterpret_cast<AndroidSession*>(
        static_cast<std::intptr_t>(handle));
}

jlong to_handle(AndroidSession* session) noexcept {
    return static_cast<jlong>(reinterpret_cast<std::intptr_t>(session));
}

std::string from_jstring(JNIEnv* env, jstring value) {
    if (value == nullptr) {
        return {};
    }
    const char* chars = env->GetStringUTFChars(value, nullptr);
    if (chars == nullptr) {
        return {};
    }
    std::string result(chars);
    env->ReleaseStringUTFChars(value, chars);
    return result;
}

jstring to_jstring(JNIEnv* env, std::string_view value) {
    return env->NewStringUTF(std::string(value).c_str());
}

std::string trim_copy(std::string_view value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string_view::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return std::string(value.substr(first, last - first + 1));
}

bool is_text_input_command(const fred::CommandNode& node) noexcept {
    return node.kind() == fred::AstNodeKind::AppendCommand ||
           node.kind() == fred::AstNodeKind::InsertCommand ||
           node.kind() == fred::AstNodeKind::ChangeCommand;
}

void write_current_buffer(AndroidSession& session) {
    const auto& buffer = session.buffers.current();
    session.output.write("[");
    session.output.write(buffer.name());
    session.output.write_line("]");

    std::size_t number = 1;
    for (const auto& line : buffer.lines()) {
        session.output.write(number == buffer.current_line() ? ">" : " ");
        session.output.write(std::to_string(number));
        session.output.write(": ");
        session.output.write_line(line);
        ++number;
    }
}

bool execute_meta_command(AndroidSession& session, std::string_view source) {
    const std::string value = trim_copy(source);

    if (value == "?version") {
        session.output.write("FREDPP v");
        session.output.write_line(fredpp::version());
        session.output.write("Commit Git : ");
        session.output.write_line(fredpp::git_commit());
        session.output.write("État des sources : ");
        session.output.write_line(fredpp::source_state());
        session.output.write("Feuille de route : ROADMAP.md — section v");
        session.output.write_line(fredpp::version());
        return true;
    }

    if (value == ":help") {
        fredpp::HelpManager help;
        session.output.write(help.load_for_terminal(":"));
        return true;
    }

    if (!value.empty() && value.front() == '?') {
        fredpp::HelpManager help;
        std::string topic = value.substr(1);
        const auto topic_start = topic.find_first_not_of(" \t");
        topic = topic_start == std::string::npos
            ? std::string{}
            : topic.substr(topic_start);

        if (topic == "version") {
            session.output.write("FREDPP v");
            session.output.write_line(fredpp::version());
            session.output.write("Commit Git : ");
            session.output.write_line(fredpp::git_commit());
            session.output.write("État des sources : ");
            session.output.write_line(fredpp::source_state());
            return true;
        }

        if (!help.exists(topic)) {
            session.output.write("Aucune rubrique d'aide : ");
            session.output.write_line(topic.empty() ? "index" : topic);
            return true;
        }

        session.output.write(help.load_for_terminal(topic));
        return true;
    }

    if (value == ":print") {
        write_current_buffer(session);
        return true;
    }

    return false;
}

void execute_text_command(AndroidSession& session) {
    if (!session.pending_text_command) {
        return;
    }

    auto node = std::move(session.pending_text_command);
    auto lines = std::move(session.pending_text_lines);
    session.pending_text_lines.clear();

    if (node->kind() == fred::AstNodeKind::AppendCommand) {
        session.executor.execute_append(
            static_cast<const fred::AppendCommandNode&>(*node),
            session.context,
            std::move(lines));
    } else if (node->kind() == fred::AstNodeKind::InsertCommand) {
        session.executor.execute_insert(
            static_cast<const fred::InsertCommandNode&>(*node),
            session.context,
            std::move(lines));
    } else {
        session.executor.execute_change(
            static_cast<const fred::ChangeCommandNode&>(*node),
            session.context,
            std::move(lines));
    }
}

void run_queued_commands(AndroidSession& session) {
    while (!session.queued_commands.empty() &&
           !session.context.exit_requested()) {
        auto node = std::move(session.queued_commands.front());
        session.queued_commands.pop_front();

        if (is_text_input_command(*node)) {
            session.pending_text_command = std::move(node);
            session.pending_text_lines.clear();
            return;
        }

        session.executor.execute(*node, session.context);
    }
}

void parse_and_queue_command_line(AndroidSession& session,
                                  std::string_view source) {
    fred::ExpandedCommandInput expanded = fred::make_command_input(source);

    const auto first_non_space = source.find_first_not_of(" \t\r");
    if (first_non_space != std::string_view::npos &&
        source[first_non_space] != ':' &&
        source[first_non_space] != '"') {
        expanded = fred::expand_command_input_with_metadata(
            source,
            session.buffers);
    }

    const std::string& command_input = expanded.text;
    if (session.runner.execute_buffer_directive(command_input)) {
        return;
    }

    if (first_non_space != std::string_view::npos &&
        source[first_non_space] == ':') {
        session.output.write_line("Unknown development command");
        return;
    }

    fred::FlowCharacterStream character_stream(
        std::move(expanded.characters));
    fred::Lexer lexer(character_stream);
    fred::TokenStream tokens(lexer);
    fred::CommandParser parser(tokens, session.registry);

    while (!tokens.eof() && !session.context.exit_requested()) {
        session.queued_commands.push_back(parser.parse_one());
    }

    run_queued_commands(session);
}

std::string execute_line(AndroidSession& session, std::string_view source) {
    std::lock_guard<std::mutex> guard(session.mutex);
    session.output.clear();

    if (session.context.exit_requested()) {
        session.output.write_line(
            "[session terminée par Q/QQ — utilisez Reset]");
        return session.output.content();
    }

    if (session.pending_text_command) {
        if (source == "\\F") {
            execute_text_command(session);
            run_queued_commands(session);
        } else {
            session.pending_text_lines.emplace_back(source);
        }
        return session.output.content();
    }

    if (source.empty()) {
        return {};
    }

    if (execute_meta_command(session, source)) {
        return session.output.content();
    }

    try {
        parse_and_queue_command_line(session, source);
    } catch (const std::exception& error) {
        session.queued_commands.clear();
        session.pending_text_command.reset();
        session.pending_text_lines.clear();
        session.output.write("error: ");
        session.output.write_line(error.what());
    } catch (...) {
        session.queued_commands.clear();
        session.pending_text_command.reset();
        session.pending_text_lines.clear();
        session.output.write_line("error: erreur native FREDPP inconnue");
    }

    return session.output.content();
}

std::string current_prompt(AndroidSession& session) {
    std::lock_guard<std::mutex> guard(session.mutex);
    if (session.pending_text_command) {
        return "text> ";
    }
    if (session.context.exit_requested()) {
        return "[Q] ";
    }
    return session.buffers.current().name() + "> ";
}

} // namespace

extern "C" JNIEXPORT jlong JNICALL
Java_fr_fredpp_android_NativeBridge_createSession(
    JNIEnv*, jobject) {
    try {
        return to_handle(new AndroidSession());
    } catch (...) {
        return 0;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_fr_fredpp_android_NativeBridge_destroySession(
    JNIEnv*, jobject, jlong handle) {
    delete reinterpret_cast<AndroidSession*>(
        static_cast<std::intptr_t>(handle));
}

extern "C" JNIEXPORT jstring JNICALL
Java_fr_fredpp_android_NativeBridge_executeLine(
    JNIEnv* env, jobject, jlong handle, jstring input) {
    try {
        auto* session = from_handle(handle);
        return to_jstring(env, execute_line(*session, from_jstring(env, input)));
    } catch (const std::exception& error) {
        return to_jstring(env, std::string("error: ") + error.what() + "\n");
    } catch (...) {
        return to_jstring(env, "error: erreur native FREDPP inconnue\n");
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_fr_fredpp_android_NativeBridge_prompt(
    JNIEnv* env, jobject, jlong handle) {
    try {
        auto* session = from_handle(handle);
        return to_jstring(env, current_prompt(*session));
    } catch (...) {
        return to_jstring(env, "?> ");
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_fr_fredpp_android_NativeBridge_version(
    JNIEnv* env, jobject) {
    return to_jstring(env, fredpp::version());
}
