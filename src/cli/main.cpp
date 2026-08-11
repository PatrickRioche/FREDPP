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

void configure_console_utf8() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

void print_buffer(const fred::Buffer& buffer) {
    std::cout << "[" << buffer.name() << "]\n";
    std::size_t number = 1;
    for (const auto& line : buffer.lines()) {
        const char marker = number == buffer.current_line() ? '>' : ' ';
        std::cout << marker << number << ": " << line << '\n';
        ++number;
    }
}

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

void print_pattern(std::string_view source) {
    fred::PatternParser parser(source);
    const auto node = parser.parse();
    std::cout << fred::describe_pattern(*node) << '\n';
}

bool print_help_topic(const fredpp::HelpManager& help, std::string_view topic) {
    try {
        std::cout << help.load_for_terminal(topic);
        if (std::cout.good()) {
            std::cout << '\n';
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void print_version_info() {
    std::cout << "FREDPP v" << fredpp::version() << '\n'
              << "Commit Git : " << fredpp::git_commit() << '\n'
              << "État des sources : " << fredpp::source_state() << '\n'
              << "Feuille de route : ROADMAP.md — section v"
              << fredpp::version() << '\n';
}


std::filesystem::path with_fredpp_extension(std::filesystem::path path) {
    if (!path.has_extension()) {
        path += ".fredpp";
    }
    return path;
}

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

std::string format_bootstrap_time(const std::tm& local,
                                  const char* format) {
    std::ostringstream stream;
    stream << std::put_time(&local, format);
    if (!stream) {
        throw std::runtime_error("cannot format bootstrap date/time");
    }
    return stream.str();
}

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

void initialize_single_line_bootstrap_buffer(
    fred::BufferManager& manager,
    std::string name,
    std::string value) {
    auto& buffer = manager.get_or_create(std::move(name));
    if (!buffer.empty() ||
        buffer.modified() ||
        buffer.has_associated_file()) {
        throw std::runtime_error(
            "bootstrap special buffer must be initially empty");
    }
    buffer.append(std::move(value));
}

void initialize_historical_bootstrap_environment(
    fred::BufferManager& manager) {
    const auto local = current_local_time();

    // DNB11A : B(d) = mm/jj/aa, B(t) = hh:mn, B(u) = USER-ID.
    initialize_single_line_bootstrap_buffer(
        manager, "d", format_bootstrap_time(local, "%m/%d/%y"));
    initialize_single_line_bootstrap_buffer(
        manager, "t", format_bootstrap_time(local, "%H:%M"));
    initialize_single_line_bootstrap_buffer(
        manager, "u", current_user_id());

    // B(0) doit exister même lorsqu'aucun argument n'a été fourni.
    manager.get_or_create("0");

    // Evite la suppression d'un B(0) courant et vide lors du chargement de B(.).
    manager.select("u");
}

} // namespace

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
        try {
            const auto procedure_path = resolve_procedure_path(argv[1]);
            initialize_parameter_buffer(manager, argc, argv);
            initialize_historical_bootstrap_environment(manager);
            procedure_runner.load_and_execute_file(procedure_path.string());
            return 0;
        } catch (const std::exception& error) {
            std::cerr << "error: " << error.what() << '\n';
            return 1;
        }
    }

    std::cout << "FREDPP v" << fredpp::version()
              << " - executable P, L, D, A, B, I, C, M, T, G, Z, S, Q, R, W, FB, FO, JM and JP commands; \" comments; OI( and OM options; \\B(buffer) procedures; * aliases 1,$\n";
    std::cout << "Type ? for FRED help; type ?: for FREDPP commands; type Q to exit.\n";

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
            if (input == ":help") {
                (void)print_help_topic(help_manager, ":");
                continue;
            }
            if (input == ":print") {
                print_buffer(manager.current());
                continue;
            }

            if (procedure_runner.execute_buffer_directive(input)) {
                if (execution_context.exit_requested()) {
                    break;
                }
                continue;
            }

            std::istringstream stream(input);
            std::string command;
            stream >> command;

            if (command == ":new") {
                std::string name;
                stream >> name;
                manager.create_or_select(name);
            } else if (command == ":use") {
                std::string name;
                stream >> name;
                manager.select(name);
            } else if (command == ":append") {
                std::string text;
                std::getline(stream >> std::ws, text);
                manager.current().append(std::move(text));
            } else if (command == ":insert") {
                std::size_t number{};
                stream >> number;
                std::string text;
                std::getline(stream >> std::ws, text);
                manager.current().insert_before(number, std::move(text));
            } else if (command == ":delete") {
                std::size_t number{};
                stream >> number;
                manager.current().erase(number, number);
            } else if (command == ":flow") {
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
                fred::Lexer lexer(input);
                fred::TokenStream tokens(lexer);
                fred::CommandParser parser(tokens, command_registry);
                const auto node = parser.parse();
                if (node->kind() == fred::AstNodeKind::AppendCommand ||
                    node->kind() == fred::AstNodeKind::InsertCommand ||
                    node->kind() == fred::AstNodeKind::ChangeCommand) {
                    std::cout << "-- text input; finish with \\F --\n";
                    std::vector<std::string> lines;
                    std::string text_line;
                    bool terminated = false;
                    while (std::cout << "text> " && std::getline(std::cin, text_line)) {
                        if (text_line == "\\F") {
                            terminated = true;
                            break;
                        }
                        lines.push_back(std::move(text_line));
                    }
                    if (!terminated) {
                        std::cerr << "error: end of input before \\F; text command cancelled\n";
                        break;
                    }
                    if (node->kind() == fred::AstNodeKind::AppendCommand) {
                        command_executor.execute_append(
                            static_cast<const fred::AppendCommandNode&>(*node),
                            execution_context, std::move(lines));
                    } else if (node->kind() == fred::AstNodeKind::InsertCommand) {
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
                    if (execution_context.exit_requested()) {
                        break;
                    }
                }
            }
        } catch (const std::exception& error) {
            std::cerr << "error: " << error.what() << '\n';
        }
    }
}
