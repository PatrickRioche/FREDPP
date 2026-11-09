/**
 * @file CommandParser.cpp
 * @brief Builds FRED command AST from an already expanded TokenStream.
 *
 * CommandParser owns no Buffer/editor state and executes no commands. Normal front ends feed it input after central Flow expansion. Parsing is transactional: failed public parses restore the TokenStream position.
 *
 * @note FREDPP_LOT8_CPP_DOCUMENTATION
 */
#include "fred/parser/CommandParser.hpp"

#include "fred/ast/AbsoluteAddressNode.hpp"
#include "fred/ast/LastAddressNode.hpp"
#include "fred/ast/RangeAddressNode.hpp"
#include "fred/parser/AddressParser.hpp"
#include "fred/parser/ParseError.hpp"
#include "fred/parser/PatternParser.hpp"

#include <charconv>
#include <cctype>
#include <cstdint>
#include <optional>
#include <sstream>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace fred {
namespace {

/** @return true when the token is the FREDPP `*` whole-buffer alias. */
bool is_whole_buffer_alias(const Token& token) noexcept {
    return token.type == TokenType::Symbol && token.lexeme == "*";
}

/**
 * @brief Expands the `*` alias to the syntactic address range `1,$`.
 */
std::unique_ptr<AddressNode> make_whole_buffer_address(
    SourceLocation location) {
    return std::make_unique<RangeAddressNode>(
        std::make_unique<AbsoluteAddressNode>(1, location),
        std::make_unique<LastAddressNode>(location),
        location);
}

/**
 * @brief Tests byte-offset adjacency in the original/reconstructed token source.
 */
bool is_adjacent(const Token& first, const Token& second) noexcept {
    return second.location.offset ==
           first.location.offset + first.lexeme.size();
}

/**
 * @brief Recognizes the one-character compact B syntax allowed by the parser.
 */
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

/**
 * @brief Reconstructs a plain-text operand until newline/EOF.
 *
 * Lexer has already removed Normal horizontal whitespace. Gaps inferred from
 * SourceLocation columns are therefore reinserted as ordinary spaces; original
 * tabs/CR bytes are not preserved byte-for-byte.
 */
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

/**
 * @brief Tests the delimiter immediately following the two-byte JM/JP spelling.
 */
bool is_message_delimiter(
    const Token& command,
    const Token& token) noexcept {
    if (token.type == TokenType::End ||
        token.type == TokenType::NewLine ||
        token.lexeme.size() != 1) {
        return false;
    }

    // JM and JP occupy two bytes in the original input spelling.
    if (token.location.offset != command.location.offset + 2) {
        return false;
    }

    const auto value =
        static_cast<unsigned char>(token.lexeme.front());

    return std::isalnum(value) == 0 &&
           std::isspace(value) == 0;
}

/**
 * @brief Consumes a delimiter-wrapped J message and reconstructs source gaps.
 */
std::string consume_delimited_message(
    TokenStream& tokens,
    const Token& command) {
    const auto opening = tokens.consume();

    if (!is_message_delimiter(command, opening)) {
        throw ParseError(
            "J message requires a non-alphanumeric delimiter",
            opening.location);
    }

    const std::string delimiter = opening.lexeme;
    std::string result;
    std::size_t previous_end_column =
        opening.location.column + opening.lexeme.size();

    while (true) {
        const auto& next = tokens.peek();

        if (next.type == TokenType::End ||
            next.type == TokenType::NewLine) {
            throw ParseError("unterminated J message", next.location);
        }

        const auto token = tokens.consume();

        if (token.location.column > previous_end_column) {
            result.append(
                token.location.column - previous_end_column,
                ' ');
        }

        if (token.lexeme == delimiter) {
            return result;
        }

        result += token.lexeme;
        previous_end_column =
            token.location.column + token.lexeme.size();
    }
}

} // namespace

CommandParser::CommandParser(TokenStream& tokens,
                             const CommandRegistry& registry) noexcept
    : tokens_(&tokens), registry_(&registry) {}

/**
 * @brief Parses one complete command under the current trailing-command policy.
 *
 * The method first parses an optional address/`*` alias, normalizes supported
 * compact spellings (QQ, WA/WU/WB, FB/FF/FN/FO/F?, JM/JP, Bx, ZG, ZK), then
 * constructs the appropriate AST, including the separate historical N
 * register operators. Special symbols `=` and `"` are parsed
 * without registry lookup. Any exception rewinds the TokenStream to the entry
 * mark.
 */
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
        bool zap_copy = false;
        std::optional<ZapExtendedKind> zap_extended;
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
            } else if (first == 'F' &&
                       (second == 'B' || second == 'F' ||
                        second == 'N' || second == 'O')) {
                facts_kind = second == 'B' ? FactsKind::Buffers
                           : second == 'F' ? FactsKind::Files
                           : second == 'N' ? FactsKind::Registers
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
            } else if (first == 'Z' && second == 'K') {
                zap_copy = true;
                command.type = TokenType::Command;
                command.lexeme = "Z";
            } else if (first == 'Z') {
                switch (second) {
                case 'A': zap_extended = ZapExtendedKind::AppendBuffer; break;
                case 'B': zap_extended = ZapExtendedKind::SortDescending; break;
                case 'C': zap_extended = ZapExtendedKind::ClearModified; break;
                case 'D': zap_extended = ZapExtendedKind::DeleteBuffer; break;
                case 'L': zap_extended = ZapExtendedKind::Lowercase; break;
                case 'M': zap_extended = ZapExtendedKind::MoveLines; break;
                case 'P': zap_extended = ZapExtendedKind::PrintBuffer; break;
                case 'S': zap_extended = ZapExtendedKind::SortAscending; break;
                case 'T': zap_extended = ZapExtendedKind::Translate; break;
                case 'U': zap_extended = ZapExtendedKind::Uppercase; break;
                default: break;
                }
                if (zap_extended) {
                    command.type = TokenType::Command;
                    command.lexeme = "Z";
                }
            }
        }
        if (command.type == TokenType::Symbol && command.lexeme == "=") {
            if (address && address->kind() == AstNodeKind::RangeAddress) {
                throw ParseError("= accepts a single line address, not a range",
                                 address->location());
            }
            require_command_end();
            return std::make_unique<LineNumberCommandNode>(
                std::move(address), command.location);
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
            if (address && address->kind() == AstNodeKind::RangeAddress) {
                throw ParseError(
                    "R insertion address must be a single line",
                    address->location());
            }
            // Preserve quote characters for the R-specific candidate list.
            // W and L still use the existing single-filename parser.
            std::optional<std::string> filename;
            if (tokens_->peek().type != TokenType::End &&
                tokens_->peek().type != TokenType::NewLine) {
                filename = consume_plain_text(*tokens_);
                bool inside_quotes = false;
                for (const char character : *filename) {
                    if (character == '"') {
                        inside_quotes = !inside_quotes;
                    }
                }
                if (inside_quotes) {
                    throw ParseError("unterminated quoted filename",
                                     command.location);
                }
                if (filename->empty()) {
                    throw ParseError("filename must not be empty",
                                     command.location);
                }
            }
            require_command_end();
            return std::make_unique<ReadCommandNode>(
                std::move(address), std::move(filename), command.location);
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

            if (is_message_delimiter(command, tokens_->peek())) {
                message =
                    consume_delimited_message(*tokens_, command);
            } else {
                message = consume_plain_text(*tokens_);
            }

            if (message.size() > 2000) {
                throw ParseError(
                    "J message exceeds historical 2000-character limit",
                    command.location);
            }

            require_command_end();
            return std::make_unique<MessageCommandNode>(
                std::move(message), *message_newline, command.location);
        }

        if (mnemonic == 'N') {
            const auto opening = tokens_->peek();
            if (opening.type != TokenType::LeftParenthesis) {
                throw ParseError("N: requires N(name):value", opening.location);
            }
            std::string name = parse_parenthesized_buffer_name();
            if (name.size() > 14 || name.find('(') != std::string::npos) {
                throw ParseError("invalid numeric register name (maximum 14 characters)",
                                 opening.location);
            }

            const auto operator_token = tokens_->consume();
            // The lexer intentionally groups ordinary letters in identifiers.
            // Thus NFx may arrive as one "Fx" token, while NF. arrives as
            // separate F and . tokens. Accept exactly one inline fill byte.
            char inline_fill = '\0';
            if (operator_token.lexeme.size() == 2 &&
                std::toupper(static_cast<unsigned char>(
                    operator_token.lexeme.front())) == 'F') {
                inline_fill = operator_token.lexeme[1];
            } else if ((operator_token.type != TokenType::Symbol &&
                        operator_token.type != TokenType::Command) ||
                       operator_token.lexeme.size() != 1) {
                throw ParseError("N requires an operator", operator_token.location);
            }

            const char operator_char = static_cast<char>(std::toupper(
                static_cast<unsigned char>(operator_token.lexeme.front())));
            std::optional<NumericOperationKind> operation;
            switch (operator_char) {
            case '+': operation = NumericOperationKind::Add; break;
            case '-': operation = NumericOperationKind::Subtract; break;
            case '*': operation = NumericOperationKind::Multiply; break;
            case '/': operation = NumericOperationKind::Divide; break;
            case '%': operation = NumericOperationKind::Modulo; break;
            case '=': operation = NumericOperationKind::Equal; break;
            case '<': operation = NumericOperationKind::Less; break;
            case '>': operation = NumericOperationKind::Greater; break;
            case '&': operation = NumericOperationKind::BitAnd; break;
            case '|': operation = NumericOperationKind::BitOr; break;
            case '~': operation = NumericOperationKind::Complement; break;
            case '^': operation = NumericOperationKind::BitXor; break;
            case '{': operation = NumericOperationKind::ShiftLeft; break;
            case '}': operation = NumericOperationKind::ShiftRight; break;
            case 'A': operation = NumericOperationKind::CurrentLine; break;
            case 'B': operation = NumericOperationKind::ChangeBase; break;
            case 'D': operation = NumericOperationKind::MinimumDigits; break;
            case 'F': operation = NumericOperationKind::FillCharacter; break;
            case 'L': operation = NumericOperationKind::CountCharacters; break;
            case 'P': operation = NumericOperationKind::Print; break;
            case ':': break;
            default:
                throw ParseError("unsupported N operator", operator_token.location);
            }

            if (address && operation != NumericOperationKind::CountCharacters) {
                throw ParseError("only NL accepts a line address",
                                 address->location());
            }

            NumericValueKind kind = NumericValueKind::Literal;
            std::int64_t literal = 0;
            std::string referenced_register;
            if (operation == NumericOperationKind::Complement ||
                operation == NumericOperationKind::CurrentLine ||
                operation == NumericOperationKind::CountCharacters ||
                operation == NumericOperationKind::Print) {
                require_command_end();
                return std::make_unique<NumericOperationCommandNode>(
                    std::move(name), *operation, kind, literal,
                    std::string{}, command.location, std::move(address));
            }
            if (operation == NumericOperationKind::FillCharacter) {
                const auto character = inline_fill == '\0'
                    ? tokens_->consume() : operator_token;
                const char fill = inline_fill != '\0'
                    ? inline_fill : character.lexeme.empty()
                        ? '\0' : character.lexeme.front();
                if ((inline_fill == '\0' && character.lexeme.size() != 1) ||
                    static_cast<unsigned char>(fill) < 33 ||
                    static_cast<unsigned char>(fill) > 126) {
                    throw ParseError("NF requires one printable character",
                                     character.location);
                }
                require_command_end();
                return std::make_unique<NumericOperationCommandNode>(
                    std::move(name), *operation, kind, literal,
                    std::string{}, command.location, nullptr,
                    fill);
            }
            if (operator_char == '|' &&
                (tokens_->peek().type == TokenType::End ||
                 tokens_->peek().type == TokenType::NewLine)) {
                operation = NumericOperationKind::Absolute;
                require_command_end();
                return std::make_unique<NumericOperationCommandNode>(
                    std::move(name), *operation, kind, literal,
                    std::string{}, command.location);
            }

            const auto operand = tokens_->consume();
            if (operand.type == TokenType::Symbol &&
                (operand.lexeme == "+" || operand.lexeme == "-")) {
                const bool negative = operand.lexeme == "-";
                const auto number = tokens_->consume();
                if (number.type != TokenType::Number) {
                    throw ParseError("N: requires a signed integer after +/-",
                                     number.location);
                }
                const std::string signed_digits =
                    (negative ? "-" : "") + number.lexeme;
                const auto* first = signed_digits.data();
                const auto* last = first + signed_digits.size();
                const auto result = std::from_chars(first, last, literal);
                if (result.ec != std::errc{} || result.ptr != last) {
                    throw ParseError("numeric value is outside supported integer range",
                                     number.location);
                }
            } else if (operand.type == TokenType::Number) {
                const auto* first = operand.lexeme.data();
                const auto* last = first + operand.lexeme.size();
                const auto result = std::from_chars(first, last, literal);
                if (result.ec != std::errc{} || result.ptr != last) {
                    throw ParseError("numeric value is outside supported integer range",
                                     operand.location);
                }
            } else if (operand.type == TokenType::Symbol &&
                       operand.lexeme == "#") {
                kind = NumericValueKind::Counter;
            } else if (operand.type == TokenType::Symbol &&
                       operand.lexeme == ".") {
                kind = NumericValueKind::CurrentLine;
            } else if (operand.type == TokenType::Symbol &&
                       operand.lexeme == "$") {
                kind = NumericValueKind::LastLine;
            } else if (operation && operand.type == TokenType::Backslash) {
                const auto source = tokens_->consume();
                if (source.type != TokenType::Command ||
                    (source.lexeme != "N" && source.lexeme != "n") ||
                    tokens_->peek().type != TokenType::LeftParenthesis) {
                    throw ParseError("N register operand requires \\N(name)",
                                     source.location);
                }
                referenced_register = parse_parenthesized_buffer_name();
                if (referenced_register.empty() ||
                    referenced_register.size() > 14 ||
                    referenced_register.find('(') != std::string::npos) {
                    throw ParseError("invalid numeric operand register name",
                                     source.location);
                }
                kind = NumericValueKind::Register;
            } else {
                throw ParseError("N: requires an integer or one of $, . and #",
                                 operand.location);
            }

            require_command_end();
            if (!operation) {
                return std::make_unique<NumericAssignmentCommandNode>(
                    std::move(name), kind, literal, command.location);
            }
            return std::make_unique<NumericOperationCommandNode>(
                std::move(name), *operation, kind, literal,
                std::move(referenced_register), command.location);
        }

        if (mnemonic == 'F') {
            if (address) {
                throw ParseError("F does not accept a line address",
                                 address->location());
            }
            // The historical F? form is two adjacent punctuation-aware tokens.
            if (!facts_kind &&
                tokens_->peek().type == TokenType::Symbol &&
                tokens_->peek().lexeme == "?" &&
                is_adjacent(command, tokens_->peek())) {
                (void)tokens_->consume();
                facts_kind = FactsKind::Modified;
            }
            if (!facts_kind) {
                // Standalone F is a query; F with a filename changes metadata.
                if (tokens_->peek().type == TokenType::End ||
                    tokens_->peek().type == TokenType::NewLine) {
                    facts_kind = FactsKind::Current;
                } else {
                    if (tokens_->peek().type == TokenType::Symbol &&
                        tokens_->peek().lexeme == "?") {
                        throw ParseError("F? must not contain whitespace",
                                         tokens_->peek().location);
                    }
                    auto filename = parse_optional_filename();
                    require_command_end();
                    return std::make_unique<FileAssociationCommandNode>(
                        std::move(*filename), command.location);
                }
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

            // Both implemented O options use an explicit +/- state. OI( also
            // requires a literal '(' as part of the option name, not a value.
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

            auto pattern = parse_delimited_pattern('G');
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
            if (zap_extended) {
                const auto operation = *zap_extended;
                const bool takes_buffer = operation == ZapExtendedKind::AppendBuffer ||
                    operation == ZapExtendedKind::DeleteBuffer ||
                    operation == ZapExtendedKind::MoveLines ||
                    operation == ZapExtendedKind::PrintBuffer;
                const bool no_address = operation == ZapExtendedKind::ClearModified ||
                    operation == ZapExtendedKind::DeleteBuffer ||
                    operation == ZapExtendedKind::PrintBuffer;
                if (no_address && address) {
                    throw ParseError("this Z command does not accept an address",
                                     address->location());
                }
                if (operation == ZapExtendedKind::AppendBuffer && address &&
                    address->kind() == AstNodeKind::RangeAddress) {
                    throw ParseError("ZA accepts at most one line address", address->location());
                }
                std::string buffer_name;
                std::unique_ptr<PatternNode> pattern;
                std::string from, to;
                if (takes_buffer) {
                    if (tokens_->peek().type != TokenType::LeftParenthesis) {
                        throw ParseError("Z command requires a buffer in parentheses",
                                         tokens_->peek().location);
                    }
                    buffer_name = parse_parenthesized_buffer_name();
                } else if (operation == ZapExtendedKind::Lowercase ||
                           operation == ZapExtendedKind::Uppercase) {
                    if (tokens_->peek().type == TokenType::Symbol &&
                        (tokens_->peek().lexeme == "/" || tokens_->peek().lexeme == "?")) {
                        pattern = parse_delimited_pattern('Z');
                    }
                } else if (operation == ZapExtendedKind::Translate) {
                    const auto location = tokens_->peek().location;
                    const std::string parts = consume_plain_text(*tokens_);
                    if (parts.size() < 3 || parts.front() != '/' || parts.back() != '/') {
                        throw ParseError("ZT requires /source/target/", location);
                    }
                    const auto separator = parts.find('/', 1);
                    if (separator == std::string::npos ||
                        parts.find('/', separator + 1) != parts.size() - 1) {
                        throw ParseError("ZT requires exactly two character sets", location);
                    }
                    from = parts.substr(1, separator - 1);
                    to = parts.substr(separator + 1, parts.size() - separator - 2);
                    if (from.empty() || from.size() != to.size()) {
                        throw ParseError("ZT sets must be nonempty and equal length", location);
                    }
                    for (std::size_t i = 0; i < from.size(); ++i) {
                        if (static_cast<unsigned char>(from[i]) >= 128 ||
                            static_cast<unsigned char>(to[i]) >= 128 ||
                            from.find(from[i], i + 1) != std::string::npos) {
                            throw ParseError("ZT requires unique ASCII source characters and ASCII target characters", location);
                        }
                    }
                }
                require_command_end();
                return std::make_unique<ZapExtendedCommandNode>(
                    operation, std::move(address), std::move(buffer_name),
                    std::move(pattern), std::move(from), std::move(to), command.location);
            }
            if (zap_copy) {
                // ZK is an insert-after-target-copy, distinct from K's
                // replace-destination behavior and ZG's output capture.
                if (tokens_->peek().type != TokenType::LeftParenthesis) {
                    throw ParseError("ZK requires a destination buffer",
                                     tokens_->peek().location);
                }
                std::string buffer_name = parse_parenthesized_buffer_name();
                require_command_end();
                return std::make_unique<ZapCopyCommandNode>(
                    std::move(address), std::move(buffer_name), command.location);
            }

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

        if (mnemonic == 'M') {
            if (tokens_->peek().type != TokenType::LeftParenthesis) {
                throw ParseError(
                    "M requires a destination buffer",
                    tokens_->peek().location);
            }

            auto buffer_name = parse_parenthesized_buffer_name();
            require_command_end();
            return std::make_unique<MoveCommandNode>(
                std::move(address), std::move(buffer_name), command.location);
        }

        if (mnemonic == 'K') {
            if (tokens_->peek().type != TokenType::LeftParenthesis) {
                throw ParseError(
                    "K requires a destination buffer",
                    tokens_->peek().location);
            }

            auto buffer_name = parse_parenthesized_buffer_name();
            require_command_end();
            return std::make_unique<CopyCommandNode>(
                std::move(address), std::move(buffer_name), command.location);
        }

        if (mnemonic == 'T') {
            // Historical T is a read-only pattern predicate. Its former
            // FREDPP transfer/destination-address interpretation was incorrect.
            bool inverted = false;
            if (tokens_->peek().type == TokenType::Symbol &&
                tokens_->peek().lexeme == "~") {
                (void)tokens_->consume();
                inverted = true;
            }

            auto pattern = parse_delimited_pattern('T');
            require_command_end();
            return std::make_unique<TestCommandNode>(
                std::move(address), std::move(pattern), inverted,
                command.location);
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


/**
 * @brief Parses one command from a chained input line.
 *
 * Temporarily allows trailing tokens so the caller can repeatedly parse_one()
 * until EOF. The prior parser mode is restored on success and failure.
 */
std::unique_ptr<CommandNode> CommandParser::parse_one() {
    const bool previous_mode = allow_trailing_commands_;
    allow_trailing_commands_ = true;

    try {
        auto node = parse();
        allow_trailing_commands_ = previous_mode;
        return node;
    } catch (...) {
        allow_trailing_commands_ = previous_mode;
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

/**
 * @brief Reconstructs a non-empty parenthesized buffer-name operand.
 */
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


/**
 * @brief Parses an optional quoted or unquoted filename operand.
 *
 * SourceLocation gaps are reconstructed as spaces for the same reason as
 * consume_plain_text().
 */
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


/**
 * @brief Reconstructs a G/T pattern plus per-byte interpretation metadata.
 *
 * Only `/` and `?` are currently accepted here. Literal metadata and
 * escaped-delimiter parity determine whether a delimiter closes the pattern.
 */
std::unique_ptr<PatternNode> CommandParser::parse_delimited_pattern(
    char mnemonic) {
    const auto opening = tokens_->consume();
    if (opening.type != TokenType::Symbol ||
        (opening.lexeme != "/" && opening.lexeme != "?")) {
        throw ParseError(std::string(1, mnemonic) +
                             " requires a pattern delimited by '/' or '?'",
                         opening.location);
    }

    const char delimiter = opening.lexeme.front();
    std::string source = opening.lexeme;
    std::vector<CharacterInterpretation> source_interpretations(opening.lexeme.size(), opening.interpretation);
    std::size_t previous_end_column =
        opening.location.column + opening.lexeme.size();

    while (true) {
        const auto& next = tokens_->peek();
        if (next.type == TokenType::End || next.type == TokenType::NewLine) {
            throw ParseError("unterminated FRED pattern", next.location);
        }

        const auto token = tokens_->consume();
        if (token.location.column > previous_end_column) {
            const auto gap = token.location.column - previous_end_column;
            source.append(gap, ' ');
            source_interpretations.insert(source_interpretations.end(), gap, CharacterInterpretation::Normal);
        }

        std::size_t preceding_backslashes = 0;
        for (auto index = source.size(); index > 0 && source[index - 1] == '\\';
             --index) {
            ++preceding_backslashes;
        }

        source += token.lexeme;
        source_interpretations.insert(source_interpretations.end(), token.lexeme.size(), token.interpretation);
        previous_end_column = token.location.column + token.lexeme.size();

        if (token.lexeme.size() == 1 && token.lexeme.front() == delimiter &&
            token.interpretation != CharacterInterpretation::Literal &&
            preceding_backslashes % 2 == 0) {
            break;
        }
    }

    PatternParser parser(source, std::move(source_interpretations), opening.location.flow_level);
    return parser.parse();
}


std::pair<std::unique_ptr<PatternNode>, std::string>
/**
 * @brief Parses S pattern/replacement segments around one symbolic delimiter.
 *
 * Pattern interpretation metadata is preserved into PatternParser; replacement
 * text remains an ordinary string consumed later by Runtime substitution.
 */
CommandParser::parse_substitution_parts() {
    const auto opening = tokens_->consume();
    if (opening.type != TokenType::Symbol || opening.lexeme.size() != 1) {
        throw ParseError(
            "S requires a symbolic delimiter before its pattern",
            opening.location);
    }

    const char delimiter = opening.lexeme.front();
    std::string pattern_source = opening.lexeme;
    std::vector<CharacterInterpretation> pattern_interpretations(opening.lexeme.size(), opening.interpretation);
    std::size_t previous_end_column =
        opening.location.column + opening.lexeme.size();

    while (true) {
        const auto& next = tokens_->peek();
        if (next.type == TokenType::End || next.type == TokenType::NewLine) {
            throw ParseError("unterminated S pattern", next.location);
        }

        const auto token = tokens_->consume();
        if (token.location.column > previous_end_column) {
            const auto gap = token.location.column - previous_end_column;
            pattern_source.append(gap, ' ');
            pattern_interpretations.insert(pattern_interpretations.end(), gap, CharacterInterpretation::Normal);
        }

        std::size_t preceding_backslashes = 0;
        for (auto index = pattern_source.size();
             index > 0 && pattern_source[index - 1] == '\\'; --index) {
            ++preceding_backslashes;
        }

        pattern_source += token.lexeme;
        pattern_interpretations.insert(pattern_interpretations.end(), token.lexeme.size(), token.interpretation);
        previous_end_column = token.location.column + token.lexeme.size();
        if (token.lexeme.size() == 1 && token.lexeme.front() == delimiter &&
            token.interpretation != CharacterInterpretation::Literal &&
            preceding_backslashes % 2 == 0) {
            break;
        }
    }

    PatternParser parser(pattern_source, std::move(pattern_interpretations), opening.location.flow_level);
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
            token.interpretation != CharacterInterpretation::Literal &&
            preceding_backslashes % 2 == 0) {
            break;
        }
        replacement += token.lexeme;
    }

    return {std::move(pattern), std::move(replacement)};
}

/**
 * @brief Enforces command termination unless parse_one() enabled chaining.
 */
void CommandParser::require_command_end() {
    if (tokens_->peek().type == TokenType::NewLine) {
        (void)tokens_->consume();
    }

    if (allow_trailing_commands_) {
        return;
    }

    const auto& token = tokens_->peek();
    if (token.type != TokenType::End) {
        throw ParseError("unexpected token after command: '" + token.lexeme + "'",
                         token.location);
    }
}

} // namespace fred
