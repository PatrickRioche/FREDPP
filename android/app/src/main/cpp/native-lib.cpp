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

/**
 * @brief Complete persistent native state for one Android terminal session.
 *
 * The session owns the same Core/Runtime services used by the desktop front end
 * and adds Android-specific command queuing/text-input state. `context` borrows
 * `buffers` and `output`; `runner` borrows buffers/context/registry/executor.
 *
 * A per-session mutex serializes JNI execute/prompt calls against this mutable
 * state. The Java/Kotlin side owns the session through an opaque jlong handle.
 */
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

/**
 * @brief Converts an opaque JNI handle back to its native session pointer.
 * @throws std::runtime_error when handle is zero.
 *
 * @warning Non-zero stale/foreign handles cannot be validated here; lifecycle
 * correctness is the responsibility of NativeSession/create/destroy pairing.
 */
AndroidSession* from_handle(jlong handle) {
    if (handle == 0) {
        throw std::runtime_error("session Android FREDPP invalide");
    }
    return reinterpret_cast<AndroidSession*>(
        static_cast<std::intptr_t>(handle));
}

/** @brief Encodes a native session pointer as the jlong handle used by Kotlin. */
jlong to_handle(AndroidSession* session) noexcept {
    return static_cast<jlong>(reinterpret_cast<std::intptr_t>(session));
}

/**
 * @brief Copies a Java string through JNI modified-UTF-8 access.
 * @return Empty string for a null jstring or failed GetStringUTFChars().
 */
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

/**
 * @brief Creates a Java string from native text using NewStringUTF.
 *
 * JNI NewStringUTF uses modified UTF-8 semantics; this bridge therefore does
 * not constitute a general arbitrary-byte transport API.
 */
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

/** @return true for A/I/C nodes that require subsequent text lines. */
bool is_text_input_command(const fred::CommandNode& node) noexcept {
    return node.kind() == fred::AstNodeKind::AppendCommand ||
           node.kind() == fred::AstNodeKind::InsertCommand ||
           node.kind() == fred::AstNodeKind::ChangeCommand;
}

/**
 * @brief Appends the Android `:print` debug view to session StringOutput.
 *
 * This is a front-end/debug representation, not execution of historical P.
 */
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

/**
 * @brief Executes Android-side help/version/debug meta commands.
 *
 * @return true when the input was fully handled as a meta command.
 *
 * Current Android-specific compatibility includes `:help` as an alias for the
 * FREDPP special help page, in addition to ordinary `?` help and `:print`.
 */
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

/**
 * @brief Completes the pending A/I/C command using accumulated text lines.
 *
 * Ownership of the pending AST and line vector is consumed. The function is a
 * no-op if no text command is pending.
 */
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

/**
 * @brief Executes queued parsed commands until completion, exit, or A/I/C.
 *
 * Encountering A/I/C moves that AST into pending_text_command and pauses the
 * remaining chain. After `\F`, execution resumes with queued commands.
 */
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

/**
 * @brief Applies the Android command pipeline and queues one parsed command line.
 *
 * Normal FRED input uses the same central metadata-preserving Flow expansion as
 * the desktop CLI, then FlowCharacterStream -> Lexer -> TokenStream ->
 * CommandParser. Lines beginning (after horizontal whitespace) with `:` or `"`
 * intentionally bypass command-input expansion.
 */
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

/**
 * @brief Executes one UI-submitted line against a persistent Android session.
 *
 * The complete operation is serialized by the session mutex and output is
 * cleared per submitted line. Once Q/QQ requests exit, only Reset (new session)
 * re-enables execution.
 *
 * Text-input mode accepts exactly `\F` as terminator; other lines are stored
 * literally until the pending A/I/C operation is completed.
 *
 * Parser/runtime exceptions are converted to textual `error:` output and any
 * queued/pending command state is cleared.
 */
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

/**
 * @brief Returns the prompt appropriate to the session's current state.
 *
 * `text> ` denotes A/I/C input, `[Q] ` denotes a terminated session, otherwise
 * the prompt is `<current-buffer-name>> `.
 */
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

/**
 * @brief JNI NativeBridge.createSession().
 * @return Opaque handle to a new AndroidSession, or 0 if construction fails.
 *
 * The Java/Kotlin caller becomes responsible for destroySession().
 */
extern "C" JNIEXPORT jlong JNICALL
Java_fr_fredpp_android_NativeBridge_createSession(
    JNIEnv*, jobject) {
    try {
        return to_handle(new AndroidSession());
    } catch (...) {
        return 0;
    }
}

/**
 * @brief JNI NativeBridge.destroySession().
 *
 * Deletes the pointer represented by the handle. Kotlin NativeSession prevents
 * normal double-destroy by setting its handle to zero after close().
 */
extern "C" JNIEXPORT void JNICALL
Java_fr_fredpp_android_NativeBridge_destroySession(
    JNIEnv*, jobject, jlong handle) {
    delete reinterpret_cast<AndroidSession*>(
        static_cast<std::intptr_t>(handle));
}

/**
 * @brief JNI NativeBridge.executeLine().
 *
 * Converts Java input, executes one native session line and returns textual
 * output. Escaping native exceptions are converted into an `error:` string so
 * C++ exceptions never cross the JNI boundary.
 */
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

/**
 * @brief JNI NativeBridge.prompt().
 * @return Current native prompt, or `?> ` if native prompt generation fails.
 */
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

/**
 * @brief JNI NativeBridge.version().
 * @return Build-generated FREDPP version string.
 */
extern "C" JNIEXPORT jstring JNICALL
Java_fr_fredpp_android_NativeBridge_version(
    JNIEnv* env, jobject) {
    return to_jstring(env, fredpp::version());
}
