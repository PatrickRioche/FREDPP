#include "fred/parser/CommandParser.hpp"

#include "fred/ast/AbsoluteAddressNode.hpp"
#include "fred/ast/LastAddressNode.hpp"
#include "fred/ast/RangeAddressNode.hpp"
#include "fred/parser/AddressParser.hpp"
#include "fred/parser/ParseError.hpp"
#include "fred/parser/PatternParser.hpp"

#include <cctype>
#include <optional>
#include <sstream>
#include <string>
#include <utility>

namespace fred {
namespace {

bool is_whole_buffer_alias(const Token& token) noexcept {
    return token.type == TokenType::Symbol && token.lexeme == "*";
}

std::unique_ptr<AddressNode> make_whole_buffer_address(
    SourceLocation location) {
    return std::make_unique<RangeAddressNode>(
        std::make_unique<AbsoluteAddressNode>(1, location),
        std::make_unique<LastAddressNode>(location),
        location);
}

bool is_adjacent(const Token& first, const Token& second) noexcept {
    return second.location.offset ==
           first.location.offset + first.lexeme.size();
}

bool is_short_buffer_name_token(const Token& command,
                                const Token& token) noexcept {
    if (!is_adjacent(command, token) || token.lexeme.size() != 1) {
        return false;
    }

    return token.type != TokenType::End &&
           token.type != TokenType::NewLine &&
           token.type != TokenType::LeftParenthesis &&
           token.type != TokenType::RightParenthesis;
}

std::string consume_plain_text(TokenStream& tokens) {
    std::string result;
    bool first = true;
    std::size_t previous_end_column = 0;

    while (tokens.peek().type != TokenType::End &&
           tokens.peek().type != TokenType::NewLine) {
        const auto token = tokens.consume();

        if (!first && token.location.column > previous_end_column) {
            result.append(token.location.column - previous_end_column, ' ');
        }

        result += token.lexeme;
        previous_end_column = token.location.column + token.lexeme.size();
        first = false;
    }

    return result;
}

std::string consume_slash_delimited_message(TokenStream& tokens) {
    const auto opening = tokens.consume();
    if (opening.type != TokenType::Symbol || opening.lexeme != "/") {
        throw ParseError("J message requires '/' delimiter", opening.location);
    }

    std::string result;
    std::size_t previous_end_column =
        opening.location.column + opening.lexeme.size();

    while (true) {
        const auto& next = tokens.peek();
        if (next.type == TokenType::End || next.type == TokenType::NewLine) {
            throw ParseError("unterminated J message", next.location);
        }

        const auto token = tokens.consume();
        if (token.location.column > previous_end_column) {
            result.append(token.location.column - previous_end_column, ' ');
        }

        if (token.type == TokenType::Symbol && token.lexeme == "/") {
            return result;
        }

        result += token.lexeme;
        previous_end_column = token.location.column + token.lexeme.size();
    }
}

} // namespace

CommandParser::CommandParser(TokenStream& tokens,
                             const CommandRegistry& registry) noexcept
    : tokens_(&tokens), registry_(&registry) {}

std::unique_ptr<CommandNode> CommandParser::parse() {
    const auto mark = tokens_->position();

    try {
        std::unique_ptr<AddressNode> address;
        bool implicit_print = false;

        if (is_whole_buffer_alias(tokens_->peek())) {
            const Token star = tokens_->consume();
            address = make_whole_buffer_address(star.location);
            const auto next_type = tokens_->peek().type;
            implicit_print = next_type == TokenType::End ||
                             next_type == TokenType::NewLine;
        } else if (begins_address(tokens_->peek())) {
            AddressParser address_parser(*tokens_);
            address = address_parser.parse_prefix();
        }

        Token command = implicit_print
            ? Token{TokenType::Command, "P", address->location()}
            : tokens_->consume();
        bool immediate_quit = false;
        bool zap_gather = false;
        FileWriteMode write_mode = FileWriteMode::Preserve;
        std::optional<std::string> compact_buffer_name;
        std::optional<FactsKind> facts_kind;
        std::optional<bool> message_newline;
        if (command.type == TokenType::Identifier && command.lexeme.size() == 2) {
            const char first = static_cast<char>(std::toupper(
                static_cast<unsigned char>(command.lexeme[0])));
            const char second = static_cast<char>(std::toupper(
                static_cast<unsigned char>(command.lexeme[1])));
            if (first == 'Q' && second == 'Q') {
                command.type = TokenType::Command;
                command.lexeme = "Q";
                immediate_quit = true;
            } else if (first == 'W' &&
                       (second == 'A' || second == 'U' || second == 'B')) {
                command.type = TokenType::Command;
                command.lexeme = "W";
                write_mode = second == 'A' ? FileWriteMode::Ascii
                           : second == 'U' ? FileWriteMode::Utf8
                                           : FileWriteMode::BcdUnsupported;
            } else if (first == 'F' && (second == 'B' || second == 'O')) {
                facts_kind = second == 'B' ? FactsKind::Buffers
                                           : FactsKind::Options;
                command.type = TokenType::Command;
                command.lexeme = "F";
            } else if (first == 'J') {
                command.type = TokenType::Command;
                command.lexeme = "J";
                if (second == 'M') {
                    message_newline = true;
                } else if (second == 'P') {
                    message_newline = false;
                }
            } else if (first == 'B') {
                compact_buffer_name = std::string(1, command.lexeme[1]);
                command.type = TokenType::Command;
                command.lexeme = "B";
            } else if (first == 'Z' && second == 'G') {
                zap_gather = true;
                command.type = TokenType::Command;
                command.lexeme = "Z";
            }
        }
        if (command.type == TokenType::Symbol && command.lexeme == "!") {
            if (address) {
                throw ParseError("! does not accept a line address",
                                 address->location());
            }
            auto system_command = consume_plain_text(*tokens_);
            if (system_command.empty()) {
                throw ParseError("! requires a system command",
                                 command.location);
            }
            require_command_end();
            return std::make_unique<SystemCommandNode>(
                std::move(system_command), command.location);
        }
        if (command.type == TokenType::Symbol && command.lexeme == "\"") {
            if (address) {
                throw ParseError("\" does not accept a line address",
                                 address->location());
            }
            auto text = consume_plain_text(*tokens_);
            require_command_end();
            return std::make_unique<CommentCommandNode>(
                std::move(text), command.location);
        }

        if (command.type == TokenType::End || command.type == TokenType::NewLine) {
            throw ParseError("expected a command", command.location);
        }
        if (command.type != TokenType::Command || command.lexeme.size() != 1) {
            throw ParseError("expected a command, got '" + command.lexeme + "'",
                             command.location);
        }

        const char mnemonic =
            static_cast<char>(
                std::toupper(
                    static_cast<unsigned char>(command.lexeme.front())
                )
            );
        const auto* descriptor = registry_->find(mnemonic);
        if (descriptor == nullptr) {
            throw ParseError(std::string("unknown command '") + mnemonic + "'",
                             command.location);
        }

        if (mnemonic == 'R') {
            if (address) {
                throw ParseError("R does not accept a line address",
                                 address->location());
            }
            auto filename = parse_optional_filename();
            if (!filename) {
                throw ParseError("R requires a filename", command.location);
            }
            require_command_end();
            return std::make_unique<ReadCommandNode>(std::move(*filename),
                                                      command.location);
        }

        if (mnemonic == 'W') {
            auto filename = parse_optional_filename();
            require_command_end();
            return std::make_unique<WriteCommandNode>(
                std::move(address), std::move(filename), write_mode,
                command.location);
        }

        if (mnemonic == 'S') {
            auto [pattern, replacement] = parse_substitution_parts();
            bool print_after = false;
            if (tokens_->peek().type == TokenType::Command &&
                tokens_->peek().lexeme.size() == 1 &&
                std::toupper(static_cast<unsigned char>(
                    tokens_->peek().lexeme.front())) == 'P') {
                (void)tokens_->consume();
                print_after = true;
            }
            require_command_end();
            return std::make_unique<SubstituteCommandNode>(
                std::move(address), std::move(pattern), std::move(replacement),
                print_after, command.location);
        }

        if (mnemonic == 'Q') {
            if (address) {
                throw ParseError("Q does not accept a line address",
                                 address->location());
            }
            if (!immediate_quit && tokens_->peek().type == TokenType::Symbol &&
                tokens_->peek().lexeme == "!") {
                throw ParseError(
                    "Q! requires external TSS command support (not implemented yet)",
                    tokens_->peek().location);
            }
            require_command_end();
            return std::make_unique<QuitCommandNode>(immediate_quit,
                                                      command.location);
        }

        if (mnemonic == 'J') {
            if (address) {
                throw ParseError("J does not accept a line address",
                                 address->location());
            }
            if (!message_newline) {
                throw ParseError("only JM and JP are implemented",
                                 command.location);
            }

            std::string message;
            if (tokens_->peek().type == TokenType::Symbol &&
                tokens_->peek().lexeme == "/") {
                message = consume_slash_delimited_message(*tokens_);
            } else {
                message = consume_plain_text(*tokens_);
            }

            require_command_end();
            return std::make_unique<MessageCommandNode>(
                std::move(message), *message_newline, command.location);
        }

        if (mnemonic == 'F') {
            if (address) {
                throw ParseError("F does not accept a line address",
                                 address->location());
            }
            if (!facts_kind) {
                throw ParseError("only FB and FO are implemented",
                                 command.location);
            }
            require_command_end();
            return std::make_unique<FactsCommandNode>(
                *facts_kind, command.location);
        }

        if (mnemonic == 'O') {
            if (address) {
                throw ParseError("O does not accept a line address",
                                 address->location());
            }

            const auto sign = tokens_->consume();
            if (sign.type != TokenType::Symbol ||
                (sign.lexeme != "+" && sign.lexeme != "-")) {
                throw ParseError("O requires '+' or '-'", sign.location);
            }

            const auto option = tokens_->consume();
            if (option.lexeme.size() != 1 ||
                (option.type != TokenType::Command &&
                 option.type != TokenType::Identifier)) {
                throw ParseError("only OI( and OM are implemented",
                                 option.location);
            }

            const char option_name = static_cast<char>(std::toupper(
                static_cast<unsigned char>(option.lexeme.front())));

            if (option_name == 'M') {
                require_command_end();
                return std::make_unique<OptionCommandNode>(
                    OptionKind::Monitor,
                    sign.lexeme == "+",
                    command.location);
            }

            if (option_name != 'I') {
                throw ParseError("only OI( and OM are implemented",
                                 option.location);
            }

            const auto opening = tokens_->consume();
            if (opening.type != TokenType::LeftParenthesis) {
                throw ParseError("OI( requires '('", opening.location);
            }
            require_command_end();
            return std::make_unique<OptionCommandNode>(
                OptionKind::InputParenthesis,
                sign.lexeme == "+",
                command.location);
        }
        if (mnemonic == 'G') {
            bool inverted = false;
            if (tokens_->peek().type == TokenType::Symbol &&
                tokens_->peek().lexeme == "~") {
                (void)tokens_->consume();
                inverted = true;
            }

            auto pattern = parse_delimited_pattern();
            if (tokens_->peek().type == TokenType::End ||
                tokens_->peek().type == TokenType::NewLine) {
                throw ParseError("G requires a command after the pattern",
                                 tokens_->peek().location);
            }

            CommandParser nested_parser(*tokens_, *registry_);
            auto nested_command = nested_parser.parse();
            if (nested_command->has_address()) {
                throw ParseError(
                    "addressed commands inside G are not supported yet",
                    nested_command->location());
            }

            return std::make_unique<GlobalCommandNode>(
                std::move(address), std::move(pattern), inverted,
                std::move(nested_command), command.location);
        }

        if (mnemonic == 'Z') {
            if (zap_gather) {
                if (address) {
                    throw ParseError("ZG does not accept a line address",
                                     address->location());
                }
                if (tokens_->peek().type != TokenType::LeftParenthesis) {
                    throw ParseError("ZG requires a destination buffer",
                                     tokens_->peek().location);
                }

                std::string buffer_name = parse_parenthesized_buffer_name();

                if (tokens_->peek().type == TokenType::End ||
                    tokens_->peek().type == TokenType::NewLine) {
                    throw ParseError("ZG requires a command",
                                     tokens_->peek().location);
                }

                CommandParser nested_parser(*tokens_, *registry_);
                auto nested_command = nested_parser.parse();
                if (nested_command->has_address()) {
                    throw ParseError(
                        "addressed commands inside ZG are not supported yet",
                        nested_command->location());
                }

                return std::make_unique<ZapGatherCommandNode>(
                    std::move(buffer_name),
                    std::move(nested_command),
                    command.location);
            }

            if (address && address->kind() == AstNodeKind::RangeAddress) {
                throw ParseError("Z accepts at most one line address",
                                 address->location());
            }
            require_command_end();
            return std::make_unique<ZapCommandNode>(
                std::move(address), command.location);
        }

        if (mnemonic == 'B') {
            if (address) {
                throw ParseError("B does not accept a line address",
                                 address->location());
            }

            std::string buffer_name;
            bool short_form = false;

            if (compact_buffer_name) {
                buffer_name = std::move(*compact_buffer_name);
                short_form = true;
            } else if (tokens_->peek().type == TokenType::LeftParenthesis) {
                buffer_name = parse_parenthesized_buffer_name();
            } else {
                const auto short_name = tokens_->consume();
                if (!is_short_buffer_name_token(command, short_name)) {
                    throw ParseError(
                        "B requires a buffer name in parentheses",
                        short_name.location);
                }
                buffer_name = short_name.lexeme;
                short_form = true;
            }

            require_command_end();
            return std::make_unique<BufferCommandNode>(
                std::move(buffer_name), short_form, command.location);
        }

        if (mnemonic == 'M' || mnemonic == 'T') {
            const char* command_name = mnemonic == 'M' ? "M" : "T";
            if (!begins_address(tokens_->peek())) {
                throw ParseError(std::string(command_name) +
                                     " requires a destination address",
                                 tokens_->peek().location);
            }

            AddressParser destination_parser(*tokens_);
            auto destination = destination_parser.parse_prefix();
            if (destination->kind() == AstNodeKind::RangeAddress) {
                throw ParseError(std::string(command_name) +
                                     " destination must be a single line address",
                                 destination->location());
            }

            require_command_end();
            if (mnemonic == 'M') {
                return std::make_unique<MoveCommandNode>(
                    std::move(address), std::move(destination), command.location);
            }
            return std::make_unique<TransferCommandNode>(
                std::move(address), std::move(destination), command.location);
        }

        if (mnemonic == 'L') {
            if (address) {
                throw ParseError("L does not accept a line address", address->location());
            }

            auto filename = parse_optional_filename();
            require_command_end();
            return std::make_unique<ListCommandNode>(std::move(filename),
                                                     command.location);
        }

        require_command_end();
        return descriptor->parse(std::move(address), command.location);
    } catch (...) {
        tokens_->rewind(mark);
        throw;
    }
}

bool CommandParser::begins_address(const Token& token) const noexcept {
    if (token.type == TokenType::Number) {
        return true;
    }
    return token.type == TokenType::Symbol &&
           (token.lexeme == "." || token.lexeme == "$" ||
            token.lexeme == "+" || token.lexeme == "-");
}

std::string CommandParser::parse_parenthesized_buffer_name() {
    const auto opening = tokens_->consume();
    if (opening.type != TokenType::LeftParenthesis) {
        throw ParseError("B requires a buffer name in parentheses",
                         opening.location);
    }

    std::string name;
    std::size_t previous_end_column = opening.location.column + opening.lexeme.size();

    while (true) {
        const auto& next = tokens_->peek();
        if (next.type == TokenType::RightParenthesis) {
            (void)tokens_->consume();
            break;
        }
        if (next.type == TokenType::End || next.type == TokenType::NewLine) {
            throw ParseError("unterminated buffer name", next.location);
        }

        const auto token = tokens_->consume();
        if (!name.empty() && token.location.column > previous_end_column) {
            name.append(token.location.column - previous_end_column, ' ');
        }
        name += token.lexeme;
        previous_end_column = token.location.column + token.lexeme.size();
    }

    if (name.empty()) {
        throw ParseError("buffer name must not be empty", opening.location);
    }
    return name;
}


std::optional<std::string> CommandParser::parse_optional_filename() {
    if (tokens_->peek().type == TokenType::End ||
        tokens_->peek().type == TokenType::NewLine) {
        return std::nullopt;
    }

    std::string filename;
    bool quoted = false;
    bool closed_quote = false;
    std::size_t previous_end_column = 0;

    if (tokens_->peek().type == TokenType::Symbol &&
        tokens_->peek().lexeme == "\"") {
        const auto opening = tokens_->consume();
        quoted = true;
        previous_end_column = opening.location.column + opening.lexeme.size();
    }

    while (tokens_->peek().type != TokenType::End &&
           tokens_->peek().type != TokenType::NewLine) {
        const auto token = tokens_->consume();
        if (quoted && token.type == TokenType::Symbol && token.lexeme == "\"") {
            closed_quote = true;
            break;
        }
        if (!filename.empty() && token.location.column > previous_end_column) {
            filename.append(token.location.column - previous_end_column, ' ');
        }
        filename += token.lexeme;
        previous_end_column = token.location.column + token.lexeme.size();
    }

    if (quoted && !closed_quote) {
        throw ParseError("unterminated quoted filename", tokens_->peek().location);
    }
    if (quoted && tokens_->peek().type != TokenType::End &&
        tokens_->peek().type != TokenType::NewLine) {
        throw ParseError("unexpected token after filename: '" +
                             tokens_->peek().lexeme + "'",
                         tokens_->peek().location);
    }
    if (filename.empty()) {
        throw ParseError("filename must not be empty", tokens_->peek().location);
    }
    return filename;
}


std::unique_ptr<PatternNode> CommandParser::parse_delimited_pattern() {
    const auto opening = tokens_->consume();
    if (opening.type != TokenType::Symbol ||
        (opening.lexeme != "/" && opening.lexeme != "?")) {
        throw ParseError("G requires a pattern delimited by '/' or '?'",
                         opening.location);
    }

    const char delimiter = opening.lexeme.front();
    std::string source = opening.lexeme;
    std::size_t previous_end_column =
        opening.location.column + opening.lexeme.size();

    while (true) {
        const auto& next = tokens_->peek();
        if (next.type == TokenType::End || next.type == TokenType::NewLine) {
            throw ParseError("unterminated FRED pattern", next.location);
        }

        const auto token = tokens_->consume();
        if (token.location.column > previous_end_column) {
            source.append(token.location.column - previous_end_column, ' ');
        }

        std::size_t preceding_backslashes = 0;
        for (auto index = source.size(); index > 0 && source[index - 1] == '\\';
             --index) {
            ++preceding_backslashes;
        }

        source += token.lexeme;
        previous_end_column = token.location.column + token.lexeme.size();

        if (token.lexeme.size() == 1 && token.lexeme.front() == delimiter &&
            preceding_backslashes % 2 == 0) {
            break;
        }
    }

    PatternParser parser(source, opening.location.flow_level);
    return parser.parse();
}


std::pair<std::unique_ptr<PatternNode>, std::string>
CommandParser::parse_substitution_parts() {
    const auto opening = tokens_->consume();
    if (opening.type != TokenType::Symbol || opening.lexeme.size() != 1) {
        throw ParseError(
            "S requires a symbolic delimiter before its pattern",
            opening.location);
    }

    const char delimiter = opening.lexeme.front();
    std::string pattern_source = opening.lexeme;
    std::size_t previous_end_column =
        opening.location.column + opening.lexeme.size();

    while (true) {
        const auto& next = tokens_->peek();
        if (next.type == TokenType::End || next.type == TokenType::NewLine) {
            throw ParseError("unterminated S pattern", next.location);
        }

        const auto token = tokens_->consume();
        if (token.location.column > previous_end_column) {
            pattern_source.append(token.location.column - previous_end_column, ' ');
        }

        std::size_t preceding_backslashes = 0;
        for (auto index = pattern_source.size();
             index > 0 && pattern_source[index - 1] == '\\'; --index) {
            ++preceding_backslashes;
        }

        pattern_source += token.lexeme;
        previous_end_column = token.location.column + token.lexeme.size();
        if (token.lexeme.size() == 1 && token.lexeme.front() == delimiter &&
            preceding_backslashes % 2 == 0) {
            break;
        }
    }

    PatternParser parser(pattern_source, opening.location.flow_level);
    auto pattern = parser.parse();

    std::string replacement;
    while (true) {
        const auto& next = tokens_->peek();
        if (next.type == TokenType::End || next.type == TokenType::NewLine) {
            throw ParseError("unterminated S replacement", next.location);
        }

        const auto token = tokens_->consume();
        if (token.location.column > previous_end_column) {
            replacement.append(token.location.column - previous_end_column, ' ');
        }

        std::size_t preceding_backslashes = 0;
        for (auto index = replacement.size();
             index > 0 && replacement[index - 1] == '\\'; --index) {
            ++preceding_backslashes;
        }

        previous_end_column = token.location.column + token.lexeme.size();
        if (token.lexeme.size() == 1 && token.lexeme.front() == delimiter &&
            preceding_backslashes % 2 == 0) {
            break;
        }
        replacement += token.lexeme;
    }

    return {std::move(pattern), std::move(replacement)};
}

void CommandParser::require_command_end() {
    if (tokens_->peek().type == TokenType::NewLine) {
        (void)tokens_->consume();
    }

    const auto& token = tokens_->peek();
    if (token.type != TokenType::End) {
        throw ParseError("unexpected token after command: '" + token.lexeme + "'",
                         token.location);
    }
}

} // namespace fred
