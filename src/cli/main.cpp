/**
 * @file main.cpp
 * @brief Desktop CLI bootstrap, developer tooling and interactive REPL orchestration.
 *
 * This front end coordinates shared FREDPP services but does not own normal FRED command semantics. Ordinary commands use central Flow expansion -> FlowCharacterStream -> Lexer -> TokenStream -> CommandParser -> CommandExecutor.
 *
 * @note FREDPP_LOT8_CPP_DOCUMENTATION
 */
#include "fredpp/version.hpp"
#include "HelpManager.h"
#include "fred/ast/AbsoluteAddressNode.hpp"
#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/ast/RangeAddressNode.hpp"
#include "fred/ast/RelativeAddressNode.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/flow/FlowEngine.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenType.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/AddressParser.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/parser/PatternParser.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ConsoleOutput.hpp"
#include "Terminal.h"
#include "HelpPager.h"
#include "fred/flow/CommandInputExpansion.hpp"
#include "fred/flow/FlowCharacterStream.hpp"
#include "fred/runtime/ProcedureRunner.hpp"
#include "fred/runtime/ExecutionContext.hpp"

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

/**
 * @brief Requests UTF-8 console input/output code pages on Windows.
 *
 * The calls are best-effort front-end configuration; non-Windows builds are a no-op.
 */
void configure_console_utf8() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

/**
 * @brief Implements the desktop `:print` developer view of one Buffer.
 *
 * This is front-end diagnostic output and is not historical P execution.
 */
void print_buffer(const fred::Buffer& buffer) {
    std::cout << "[" << buffer.name() << "]\n";
    std::size_t number = 1;
    for (const auto& line : buffer.lines()) {
        const char marker = number == buffer.current_line() ? '>' : ' ';
        std::cout << marker << number << ": " << line << '\n';
        ++number;
    }
}

/** @brief Implements the eager `:lex` developer token dump. */
void print_tokens(std::string_view source) {
    fred::Lexer lexer(source);
    for (const auto& token : lexer.tokenize()) {
        std::cout << fred::token_type_name(token.type)
                  << " \"" << token.lexeme << "\""
                  << " @ " << token.location.line
                  << ':' << token.location.column
                  << " level=" << token.location.flow_level
                  << '\n';
    }
}

/** @brief Implements the lazy TokenStream `:tokens` developer dump. */
void print_token_stream(std::string_view source) {
    fred::Lexer lexer(source);
    fred::TokenStream stream(lexer);

    while (true) {
        const auto token = stream.consume();
        std::cout << fred::token_type_name(token.type)
                  << " \"" << token.lexeme << "\""
                  << " @ " << token.location.line
                  << ':' << token.location.column
                  << " level=" << token.location.flow_level
                  << '\n';

        if (token.type == fred::TokenType::End) {
            break;
        }
    }
}

/** @brief Parses and prints an address AST for the `:address` developer tool. */
void print_address(std::string_view source) {
    fred::Lexer lexer(source);
    fred::TokenStream stream(lexer);
    fred::AddressParser parser(stream);
    const auto node = parser.parse();

    switch (node->kind()) {
    case fred::AstNodeKind::AbsoluteAddress: {
        const auto& value = static_cast<const fred::AbsoluteAddressNode&>(*node);
        std::cout << "Absolute(" << value.line() << ")\n";
        break;
    }
    case fred::AstNodeKind::CurrentAddress:
        std::cout << "Current(.)\n";
        break;
    case fred::AstNodeKind::LastAddress:
        std::cout << "Last($)\n";
        break;
    case fred::AstNodeKind::RelativeAddress: {
        const auto& value = static_cast<const fred::RelativeAddressNode&>(*node);
        const char sign = value.direction() == fred::RelativeDirection::Forward
            ? '+' : '-';
        std::cout << "Relative(" << sign << value.distance() << ")\n";
        break;
    }
    case fred::AstNodeKind::RangeAddress: {
        const auto& value = static_cast<const fred::RangeAddressNode&>(*node);
        std::cout << "Range(" << static_cast<int>(value.first().kind())
                  << "," << static_cast<int>(value.last().kind()) << ")\n";
        break;
    }
    default:
        throw std::logic_error("non-address AST node returned by AddressParser");
    }
}


/**
 * @brief Parses and prints a compact command-AST summary for `:command`.
 *
 * This debug presentation is intentionally not the authoritative list of every
 * current AstNodeKind label.
 */
void print_command(std::string_view source) {
    fred::Lexer lexer(source);
    fred::TokenStream stream(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(stream, registry);
    const auto node = parser.parse();

    std::string_view name;
    switch (node->kind()) {
    case fred::AstNodeKind::PrintCommand: name = "Print"; break;
    case fred::AstNodeKind::ListCommand: name = "List"; break;
    case fred::AstNodeKind::DeleteCommand: name = "Delete"; break;
    case fred::AstNodeKind::AppendCommand: name = "Append"; break;
    case fred::AstNodeKind::InsertCommand: name = "Insert"; break;
    case fred::AstNodeKind::ChangeCommand: name = "Change"; break;
    case fred::AstNodeKind::MoveCommand: name = "Move"; break;
    case fred::AstNodeKind::TransferCommand: name = "Transfer"; break;
    case fred::AstNodeKind::BufferCommand: name = "Buffer"; break;
    case fred::AstNodeKind::GlobalCommand: name = "Global"; break;
    case fred::AstNodeKind::ZapCommand: name = "Zap"; break;
    case fred::AstNodeKind::SubstituteCommand: name = "Substitute"; break;
    case fred::AstNodeKind::ReadCommand: name = "Read"; break;
    case fred::AstNodeKind::WriteCommand: name = "Write"; break;
    case fred::AstNodeKind::CommentCommand: name = "Comment"; break;
    case fred::AstNodeKind::MessageCommand: name = "Message"; break;
    case fred::AstNodeKind::FactsCommand: name = "Facts"; break;
    case fred::AstNodeKind::OptionCommand: name = "Option"; break;
    case fred::AstNodeKind::QuitCommand: name = "Quit"; break;
    default: name = "Command"; break;
    }

    std::cout << name << "(";
    if (node->has_address()) {
        std::cout << "address-kind=" << static_cast<int>(node->address()->kind());
    } else {
        std::cout << "no-address";
    }
    std::cout << ")\n";
}

/** @brief Parses and describes a pattern for the `:pattern` developer tool. */
void print_pattern(std::string_view source) {
    fred::PatternParser parser(source);
    const auto node = parser.parse();
    std::cout << fred::describe_pattern(*node) << '\n';
}

/**
 * @brief Loads, renders and pages one help topic.
 * @return false when HelpManager/rendering raises a standard exception.
 */
bool print_help_topic(const fredpp::HelpManager& help, std::string_view topic) {
    try {
        const auto rendered = help.load_for_terminal(topic);
        fredpp::show_paged_help(rendered);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

/**
 * @brief Prints build-generated software/Git/source-state metadata.
 */
void print_version_info() {
    std::cout << "FREDPP v" << fredpp::version() << '\n'
              << "Commit Git : " << fredpp::git_commit() << '\n'
              << "État des sources : " << fredpp::source_state() << '\n'
              << "Feuille de route : ROADMAP.md — section v"
              << fredpp::version() << '\n';
}


/** @brief Adds `.fredpp` only when a procedure path has no extension. */
std::filesystem::path with_fredpp_extension(std::filesystem::path path) {
    if (!path.has_extension()) {
        path += ".fredpp";
    }
    return path;
}

/**
 * @brief Resolves a requested procedure according to current desktop search policy.
 *
 * Explicit paths are tested directly (and with `.fredpp` when extensionless).
 * Simple names are searched in the current directory and, on Windows only, the
 * fixed `C:\fredpp\library` location.
 */
std::filesystem::path resolve_procedure_path(std::string_view name) {
    if (name.empty()) {
        throw std::runtime_error("procedure name must not be empty");
    }

    const std::filesystem::path requested{std::string(name)};
    std::vector<std::filesystem::path> candidates;
    const bool explicit_request =
        requested.is_absolute() || requested.has_parent_path() ||
        requested.has_extension();

    if (explicit_request) {
        candidates.push_back(requested);
        if (!requested.has_extension()) {
            candidates.push_back(with_fredpp_extension(requested));
        }
    } else {
        const auto physical = with_fredpp_extension(requested);
        candidates.push_back(std::filesystem::current_path() / physical);
#ifdef _WIN32
        candidates.push_back(
            std::filesystem::path{R"(C:\fredpp\library)"} / physical);
#endif
    }

    for (const auto& candidate : candidates) {
        std::error_code error;
        if (std::filesystem::is_regular_file(candidate, error) && !error) {
            return candidate;
        }
    }

    std::string message = "procedure not found: " + std::string(name) + "; tried:";
    for (const auto& candidate : candidates) {
        message += " " + candidate.string();
    }
    throw std::runtime_error(message);
}

/**
 * @brief Populates historical parameter Buffer `0` from argv[2..].
 *
 * The buffer must still be empty, clean and unassociated when bootstrap reaches
 * this step.
 */
void initialize_parameter_buffer(fred::BufferManager& manager,
                                 int argc, char** argv) {
    auto& parameters = manager.get_or_create("0");
    if (!parameters.empty() || parameters.modified() ||
        parameters.has_associated_file()) {
        throw std::runtime_error(
            "bootstrap parameter buffer B(0) must be initially empty");
    }
    for (int index = 2; index < argc; ++index) {
        parameters.append(argv[index]);
    }
}


/** @brief Obtains local civil time through the platform-safe C runtime API. */
std::tm current_local_time() {
    const std::time_t now = std::time(nullptr);
    if (now == static_cast<std::time_t>(-1)) {
        throw std::runtime_error("cannot obtain current time");
    }

    std::tm local{};
#ifdef _WIN32
    if (localtime_s(&local, &now) != 0) {
        throw std::runtime_error("cannot convert current time");
    }
#else
    if (localtime_r(&now, &local) == nullptr) {
        throw std::runtime_error("cannot convert current time");
    }
#endif
    return local;
}

/** @brief Formats the historical bootstrap date/time Buffer values. */
std::string format_bootstrap_time(const std::tm& local,
                                  const char* format) {
    std::ostringstream stream;
    stream << std::put_time(&local, format);
    if (!stream) {
        throw std::runtime_error("cannot format bootstrap date/time");
    }
    return stream.str();
}

/**
 * @brief Resolves the current user identifier from platform environment variables.
 */
std::string current_user_id() {
#ifdef _WIN32
    if (const char* value = std::getenv("USERNAME");
        value != nullptr && *value != '\0') {
        return value;
    }
#else
    if (const char* value = std::getenv("USER");
        value != nullptr && *value != '\0') {
        return value;
    }
    if (const char* value = std::getenv("LOGNAME");
        value != nullptr && *value != '\0') {
        return value;
    }
#endif
    return "unknown";
}

/**
 * @brief Creates/populates one historical single-line bootstrap Buffer.
 *
 * Explicit selection records the buffer in BufferManager's MRU view used by FB.
 */
void initialize_single_line_bootstrap_buffer(
    fred::BufferManager& manager,
    std::string name,
    std::string value) {
    auto& buffer = manager.get_or_create(name);
    if (!buffer.empty() ||
        buffer.modified() ||
        buffer.has_associated_file()) {
        throw std::runtime_error(
            "bootstrap special buffer must be initially empty");
    }
    buffer.append(std::move(value));

    // FB must know every opened Buffer. get_or_create() intentionally does
    // not touch MRU ordering, so explicit selection records d, t and u in the
    // historical bootstrap usage order.
    manager.select(name);
}

/**
 * @brief Creates the desktop procedure-startup `d`, `t` and `u` Buffers.
 *
 * This process bootstrap policy belongs to the desktop front end and is not a
 * generic Core/Runtime constructor responsibility.
 */
void initialize_historical_bootstrap_environment(
    fred::BufferManager& manager) {
    const auto local = current_local_time();

    // DNB11A bootstrap values: B(d)=mm/dd/yy, B(t)=hh:mm, B(u)=USER-ID.
    initialize_single_line_bootstrap_buffer(
        manager, "d", format_bootstrap_time(local, "%m/%d/%y"));
    initialize_single_line_bootstrap_buffer(
        manager, "t", format_bootstrap_time(local, "%H:%M"));
    initialize_single_line_bootstrap_buffer(
        manager, "u", current_user_id());

    // Historical bootstrap populates B(0) after user init execution.
    // initialize_parameter_buffer() performs that later materialization step.
    manager.select("u");
}


/**
 * @brief Resolved `.init.fredpp` location and policy flags.
 */
struct UserInitLocation {
    std::filesystem::path path;
    bool explicit_override{};
    bool disabled{};
};

/**
 * @brief Resolves FREDPP_INIT override/default/disabled user-init policy.
 *
 * An explicitly empty FREDPP_INIT disables init. Missing default home also
 * disables it, while a non-empty explicit override must later exist.
 */
UserInitLocation resolve_user_init_location() {
    if (const char* override_path = std::getenv("FREDPP_INIT");
        override_path != nullptr) {
        if (*override_path == '\0') {
            return UserInitLocation{{}, true, true};
        }
        return UserInitLocation{
            std::filesystem::path{override_path}, true, false};
    }

#ifdef _WIN32
    const char* home = std::getenv("USERPROFILE");
#else
    const char* home = std::getenv("HOME");
#endif

    if (home == nullptr || *home == '\0') {
        return UserInitLocation{{}, false, true};
    }

    return UserInitLocation{
        std::filesystem::path{home} / "fredpp" / ".init.fredpp",
        false,
        false};
}

/** @brief Removes the reserved temporary `__init` Buffer when present. */
void erase_bootstrap_init_buffer(fred::BufferManager& manager) {
    if (manager.contains("__init")) {
        manager.erase("__init");
    }
}

/**
 * @brief Executes the optional user init procedure in reserved Buffer `__init`.
 *
 * Missing default init is not an error. Missing explicit FREDPP_INIT is an
 * error. The temporary Buffer is erased on both success and exception paths.
 */
void execute_user_init_if_present(
    fred::ProcedureRunner& procedure_runner,
    fred::BufferManager& manager) {
    const auto location = resolve_user_init_location();
    if (location.disabled) {
        return;
    }

    std::error_code error;
    const bool exists = std::filesystem::exists(location.path, error);
    if (error) {
        throw std::runtime_error(
            "cannot inspect user init file: " + location.path.string() +
            ": " + error.message());
    }

    if (!exists) {
        if (location.explicit_override) {
            throw std::runtime_error(
                "FREDPP_INIT file not found: " + location.path.string());
        }
        // As in FRED bootstrap behavior, a missing default init is not an error.
        return;
    }

    if (!std::filesystem::is_regular_file(location.path, error) || error) {
        const std::string detail =
            error ? ": " + error.message() : std::string{};
        throw std::runtime_error(
            "user init is not a regular file: " +
            location.path.string() + detail);
    }

    if (manager.contains("__init")) {
        throw std::runtime_error(
            "reserved bootstrap buffer __init already exists");
    }

    try {
        procedure_runner.load_and_execute_file(
            location.path.string(), "__init");
        erase_bootstrap_init_buffer(manager);
    } catch (const std::exception& init_error) {
        erase_bootstrap_init_buffer(manager);
        throw std::runtime_error(
            "user init failed: " + location.path.string() +
            ": " + init_error.what());
    } catch (...) {
        erase_bootstrap_init_buffer(manager);
        throw std::runtime_error(
            "user init failed: " + location.path.string());
    }
}

} // namespace

/**
 * @brief Desktop FREDPP process entry point.
 *
 * `--version` exits before service construction. Procedure mode performs
 * historical d/t/u bootstrap, user init, B(0) parameters and path resolution,
 * then executes the requested procedure. Procedure failure preserves state and
 * falls through to the interactive debug REPL.
 *
 * Interactive normal commands use central Flow expansion before parsing.
 * A/I/C text collection is a front-end concern; Q/QQ exit exclusively through
 * ExecutionContext rather than a duplicate CLI quit flag.
 *
 * @return 0 for normal/version/procedure completion, 1 for startup/bootstrap
 * failures before procedure execution.
 */
int main(int argc, char** argv) {
    configure_console_utf8();

    if (argc > 1 && std::string_view(argv[1]) == "--version") {
        print_version_info();
        return 0;
    }

    fredpp::HelpManager help_manager;
    fred::BufferManager manager;
    fred::ConsoleOutput output(std::cout);
    fred::ExecutionContext execution_context(manager, output);
    fred::CommandExecutor command_executor;
    const auto command_registry = fred::make_core_command_registry();
    fred::ProcedureRunner procedure_runner(
        manager, execution_context, command_registry, command_executor);
    if (argc >= 2) {
        std::filesystem::path procedure_path;
        try {
            initialize_historical_bootstrap_environment(manager);
            execute_user_init_if_present(procedure_runner, manager);
            if (execution_context.exit_requested()) {
                return 0;
            }

            initialize_parameter_buffer(manager, argc, argv);
            procedure_path = resolve_procedure_path(argv[1]);
        } catch (const std::exception& error) {
            std::cerr << "error: " << error.what() << '\n';
            return 1;
        }

        try {
            manager.select("0");
            procedure_runner.load_and_execute_file(procedure_path.string());
            return 0;
        } catch (const std::exception& error) {
            std::cerr << "error: " << error.what() << '\n';
            std::cerr
                << "procedure stopped; entering interactive debug mode\n";
            // Do not return: preserve current Buffer/runtime state and fall
            // through to the interactive FREDPP debug loop below.
        }
    }

    std::cout << "FREDPP v" << fredpp::version()
              << " - Type ? for FRED help; type ?: for FREDPP commands; type Q or QQ to exit.\n";

    std::string input;
    while (std::cout << manager.current().name() << "> " &&
           std::getline(std::cin, input)) {
        try {
            const auto first_non_space = input.find_first_not_of(" \t\r");
            if (first_non_space != std::string::npos &&
                input[first_non_space] == '?') {
                std::string topic = input.substr(first_non_space + 1);
                const auto topic_start = topic.find_first_not_of(" \t");
                topic = topic_start == std::string::npos ? std::string{}
                                                        : topic.substr(topic_start);
                if (topic == "version") {
                    print_version_info();
                } else if (!print_help_topic(help_manager, topic)) {
                    std::cout << "Aucune rubrique d'aide : "
                              << (topic.empty() ? "index" : topic) << '\n';
                }
                continue;
            }
            if (input == ":cls") {
                fredpp::clear_terminal();
                continue;
            }
            if (input == ":print") {
                print_buffer(manager.current());
                continue;
            }

            fred::ExpandedCommandInput expanded_command =
                fred::make_command_input(input);
            if (first_non_space != std::string::npos &&
                input[first_non_space] != ':' &&
                input[first_non_space] != '"') {
                expanded_command =
                    fred::expand_command_input_with_metadata(
                        input, manager);
            }
            const std::string& command_input =
                expanded_command.text;

            if (procedure_runner.execute_buffer_directive(command_input)) {
                if (execution_context.exit_requested()) {
                    break;
                }
                continue;
            }

            std::istringstream stream(input);
            std::string command;
            stream >> command;

            if (command == ":flow") {
                std::string name;
                stream >> name;
                fred::FlowEngine flow(manager);
                std::cout << flow.expand_buffer(name);
            } else if (command == ":lex") {
                std::string text;
                std::getline(stream >> std::ws, text);
                print_tokens(text);
            } else if (command == ":tokens") {
                std::string text;
                std::getline(stream >> std::ws, text);
                print_token_stream(text);
            } else if (command == ":address") {
                std::string text;
                std::getline(stream >> std::ws, text);
                print_address(text);
            } else if (command == ":command") {
                std::string text;
                std::getline(stream >> std::ws, text);
                print_command(text);
            } else if (command == ":pattern") {
                std::string text;
                std::getline(stream >> std::ws, text);
                print_pattern(text);
            } else if (!input.empty() && input.front() == ':') {
                std::cout << "Unknown development command\n";
            } else if (!input.empty()) {
                fred::FlowCharacterStream character_stream(
                    std::move(expanded_command.characters));
                fred::Lexer lexer(character_stream);
                fred::TokenStream tokens(lexer);
                fred::CommandParser parser(tokens, command_registry);

                while (!tokens.eof() &&
                       !execution_context.exit_requested()) {
                    const auto node = parser.parse_one();

                    if (node->kind() == fred::AstNodeKind::AppendCommand ||
                        node->kind() == fred::AstNodeKind::InsertCommand ||
                        node->kind() == fred::AstNodeKind::ChangeCommand) {
                        std::cout << "-- text input; finish with \\F --\n";
                        std::vector<std::string> lines;
                        std::string text_line;
                        bool terminated = false;

                        while (std::cout << "text> " &&
                               std::getline(std::cin, text_line)) {
                            if (text_line == "\\F") {
                                terminated = true;
                                break;
                            }
                            lines.push_back(std::move(text_line));
                        }

                        if (!terminated) {
                            throw std::runtime_error(
                                "end of input before \\F; text command cancelled");
                        }

                        if (node->kind() == fred::AstNodeKind::AppendCommand) {
                            command_executor.execute_append(
                                static_cast<const fred::AppendCommandNode&>(*node),
                                execution_context, std::move(lines));
                        } else if (node->kind() ==
                                   fred::AstNodeKind::InsertCommand) {
                            command_executor.execute_insert(
                                static_cast<const fred::InsertCommandNode&>(*node),
                                execution_context, std::move(lines));
                        } else {
                            command_executor.execute_change(
                                static_cast<const fred::ChangeCommandNode&>(*node),
                                execution_context, std::move(lines));
                        }
                    } else {
                        command_executor.execute(*node, execution_context);
                    }
                }

                if (execution_context.exit_requested()) {
                    break;
                }
            }
        } catch (const std::exception& error) {
            std::cerr << "error: " << error.what() << '\n';
        }
    }
}
