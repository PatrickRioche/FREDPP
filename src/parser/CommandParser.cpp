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
#include "fred/runtime/ExecutionContext.hpp"

#include <algorithm>
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

/** Parses a short delimiter-wrapped OI prompt, e.g. /Mode CMD>/ or !Next!. */
std::string parse_option_prompt(std::string text, SourceLocation location) {
    if (text.size() < 2 || text.front() != text.back()) {
        throw ParseError("OI prompt requires matching delimiters", location);
    }
    const unsigned char delimiter = static_cast<unsigned char>(text.front());
    if (std::isalnum(delimiter) || std::isspace(delimiter) || delimiter == '\\') {
        throw ParseError("OI prompt delimiter must be symbolic", location);
    }
    std::string value;
    for (std::size_t i = 1; i + 1 < text.size(); ++i) {
        if (text[i] == '\\' && i + 2 < text.size()) {
            value.push_back(text[++i]);
        } else if (text[i] == text.front()) {
            throw ParseError("unescaped delimiter in OI prompt", location);
        } else {
            value.push_back(text[i]);
        }
    }
    if (value.size() > 120) {
        throw ParseError("OI prompt exceeds 120 characters", location);
    }
    return value;
}

/** Parses OC's two delimiter-separated literals, preserving escaped delimiters. */
std::pair<std::string, std::string> parse_option_change(
    std::string_view source, SourceLocation location) {
    if (source.size() < 4 || std::isalnum(static_cast<unsigned char>(source.front())) ||
        source.front() == '\\') {
        throw ParseError("OC requires /from/to/", location);
    }
    const char delimiter = source.front();
    std::string parts[2];
    int field = 0;
    bool closed = false;
    for (std::size_t i = 1; i < source.size(); ++i) {
        const char ch = source[i];
        if (ch == '\\' && i + 1 < source.size()) {
            parts[field].push_back(source[++i]);
        } else if (ch == delimiter) {
            if (++field == 2) {
                closed = (i + 1 == source.size());
                break;
            }
        } else {
            parts[field].push_back(ch);
        }
    }
    if (!closed || parts[0].empty() || parts[0].size() > 120 ||
        parts[1].size() > 120) {
        throw ParseError("OC requires bounded /nonempty-from/to/", location);
    }
    return {std::move(parts[0]), std::move(parts[1])};
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

        if (token.lexeme == delimiter &&
            token.interpretation != CharacterInterpretation::Literal) {
            return result;
        }

        result += token.lexeme;
        previous_end_column =
            token.location.column + token.lexeme.size();
    }
}

} // namespace

CommandParser::CommandParser(TokenStream& tokens,
                             const CommandRegistry& registry,
                             const ExecutionContext* context) noexcept
    : tokens_(&tokens), registry_(&registry), context_(context) {}

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
        bool zap_read = false;
        bool zap_append_write = false;
        bool user_error_message = false;
        bool redo_undo = false;
        bool repeat_control = false;
        bool repeat_breaks = false;
        char repeat_condition = 'A';
        std::size_t repeat_limit = 0;
        char repeat_until = 'N';
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
                        second == 'N' || second == 'O' || second == 'V' ||
                        second == 'E')) {
                facts_kind = second == 'B' ? FactsKind::Buffers
                           : second == 'F' ? FactsKind::Files
                           : second == 'N' ? FactsKind::Registers
                           : second == 'V' ? FactsKind::Version
                           : second == 'E' ? FactsKind::Patterns
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
                } else if (second == 'E') {
                    message_newline = true;
                    user_error_message = true;
                } else if (second == 'B' || second == 'O') {
                    repeat_control = true;
                    repeat_breaks = second == 'B';
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
            } else if (first == 'Z' && second == 'V') {
                redo_undo = true;
                command.type = TokenType::Command;
                command.lexeme = "Z";
            } else if (first == 'Z' && second == 'R') {
                zap_read = true;
                command.type = TokenType::Command;
                command.lexeme = "R";
            } else if (first == 'Z' && second == 'W') {
                zap_append_write = true;
                command.type = TokenType::Command;
                command.lexeme = "W";
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
                case 'I': zap_extended = ZapExtendedKind::CompressTabs; break;
                case 'O': zap_extended = ZapExtendedKind::ExpandTabs; break;
                default: break;
                }
                if (zap_extended) {
                    command.type = TokenType::Command;
                    command.lexeme = "Z";
                }
            }
        }
        if (command.type == TokenType::Identifier &&
            command.lexeme.size() == 3 &&
            std::toupper(static_cast<unsigned char>(command.lexeme[0])) == 'J' &&
            (std::toupper(static_cast<unsigned char>(command.lexeme[1])) == 'B' ||
             std::toupper(static_cast<unsigned char>(command.lexeme[1])) == 'O')) {
            const char suffix = static_cast<char>(std::toupper(
                static_cast<unsigned char>(command.lexeme[2])));
            if (suffix == 'T' || suffix == 'F') {
                repeat_control = true;
                repeat_breaks = std::toupper(static_cast<unsigned char>(command.lexeme[1])) == 'B';
                repeat_condition = suffix;
                command.type = TokenType::Command;
                command.lexeme = "J";
            }
        }
        if (command.type == TokenType::Identifier &&
            command.lexeme.size() >= 2 &&
            std::toupper(static_cast<unsigned char>(command.lexeme[0])) == 'U') {
            const auto suffix = std::string_view(command.lexeme).substr(1);
            std::size_t digits = 0;
            while (digits < suffix.size() &&
                   std::isdigit(static_cast<unsigned char>(suffix[digits]))) ++digits;
            if (digits || suffix.size() == 1) {
                if (digits) {
                    const auto parsed = std::from_chars(suffix.data(), suffix.data() + digits,
                                                        repeat_limit);
                    if (parsed.ec != std::errc{} || repeat_limit < 1 ||
                        repeat_limit > 262143) {
                        throw ParseError("U repetition limit must be 1..262143", command.location);
                    }
                }
                if (digits < suffix.size()) {
                    if (digits + 1 != suffix.size()) {
                        throw ParseError("invalid U repeat suffix", command.location);
                    }
                    repeat_until = static_cast<char>(std::toupper(
                        static_cast<unsigned char>(suffix[digits])));
                    if (repeat_until != 'T' && repeat_until != 'F' && repeat_until != 'E') {
                        throw ParseError("U suffix must be T, F or E", command.location);
                    }
                }
                command.type = TokenType::Command;
                command.lexeme = "U";
            }
        }
        if (command.type == TokenType::Identifier &&
            command.lexeme.size() == 3 &&
            std::toupper(static_cast<unsigned char>(command.lexeme[0])) == 'Z' &&
            std::toupper(static_cast<unsigned char>(command.lexeme[1])) == 'W') {
            const char mode = static_cast<char>(std::toupper(
                static_cast<unsigned char>(command.lexeme[2])));
            if (mode == 'A' || mode == 'U' || mode == 'B') {
                zap_append_write = true;
                command.type = TokenType::Command;
                command.lexeme = "W";
                write_mode = mode == 'A' ? FileWriteMode::Ascii
                           : mode == 'U' ? FileWriteMode::Utf8
                                         : FileWriteMode::BcdUnsupported;
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
        if (command.type == TokenType::Symbol && command.lexeme == "#") {
            if (address) {
                throw ParseError("# does not accept a line address", address->location());
            }
            require_command_end();
            return std::make_unique<CounterCommandNode>(nullptr, command.location);
        }
        if (command.type == TokenType::Symbol && command.lexeme == ":") {
            if (address) {
                throw ParseError(": does not accept a line address", address->location());
            }
            std::size_t radius = 5;
            if (tokens_->peek().type == TokenType::Number) {
                const auto number = tokens_->consume();
                const auto result = std::from_chars(
                    number.lexeme.data(), number.lexeme.data() + number.lexeme.size(), radius);
                if (result.ec != std::errc{} || radius > 100000) {
                    throw ParseError("window radius must be 0..100000", number.location);
                }
            }
            require_command_end();
            return std::make_unique<WindowCommandNode>(radius, command.location);
        }
        if (command.type == TokenType::Symbol && command.lexeme == "@") {
            if (address) {
                throw ParseError("@ does not accept a line address", address->location());
            }
            const auto name = parse_parenthesized_buffer_name();
            if (name.size() > 15 || name.find('(') != std::string::npos ||
                name.find(')') != std::string::npos) {
                throw ParseError("label must contain 1..15 characters", command.location);
            }
            require_command_end();
            return std::make_unique<LabelCommandNode>(name, command.location);
        }
        if (command.type == TokenType::Backslash && command.lexeme == "\\") {
            if (address) {
                throw ParseError("wait does not accept a line address", address->location());
            }
            const auto second = tokens_->consume();
            if (second.type != TokenType::Backslash || !is_adjacent(command, second)) {
                throw ParseError("wait requires two adjacent backslashes", second.location);
            }
            const auto number = tokens_->consume();
            if (number.type != TokenType::Number || !is_adjacent(second, number)) {
                throw ParseError("wait requires a number of seconds", number.location);
            }
            std::size_t seconds = 0;
            const auto result = std::from_chars(
                number.lexeme.data(), number.lexeme.data() + number.lexeme.size(), seconds);
            if (result.ec != std::errc{} || seconds > 86400) {
                throw ParseError("wait must not exceed 86400 seconds", number.location);
            }
            require_command_end();
            return std::make_unique<WaitCommandNode>(seconds, command.location);
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

        if (mnemonic == 'E') {
            if (address) throw ParseError("E does not take an address", command.location);
            const auto name = parse_parenthesized_buffer_name();
            if (name.size() > 15 || name.find('(') != std::string::npos ||
                name.find(')') != std::string::npos) {
                throw ParseError("E pattern name must be 1..15 characters", command.location);
            }
            const auto raw = consume_plain_text(*tokens_);
            if (raw.size() < 2 || std::isalnum(static_cast<unsigned char>(raw.front())) ||
                raw.front() != raw.back()) {
                throw ParseError("E requires a delimited pattern", command.location);
            }
            return std::make_unique<NamedPatternCommandNode>(name, raw, command.location);
        }
        if (mnemonic == 'U') {
            if (address) throw ParseError("U addressed ranges are not yet supported", command.location);
            // Lexer intentionally separates U3 (command + number), as for M5.
            // A T/F/E modifier is part of U only when adjacent to the count.
            if (tokens_->peek().type == TokenType::Number) {
                const auto count = tokens_->consume();
                auto parsed = std::from_chars(count.lexeme.data(),
                    count.lexeme.data() + count.lexeme.size(), repeat_limit);
                if (parsed.ec != std::errc{} || repeat_limit < 1 ||
                    repeat_limit > 262143) {
                    throw ParseError("U count must be 1..262143", count.location);
                }
                const auto& modifier = tokens_->peek();
                if ((modifier.type == TokenType::Command ||
                     modifier.type == TokenType::Identifier) &&
                    modifier.lexeme.size() == 1 &&
                    modifier.location.column == count.location.column + count.lexeme.size()) {
                    const auto candidate = static_cast<char>(std::toupper(
                        static_cast<unsigned char>(modifier.lexeme[0])));
                    if (candidate == 'T' || candidate == 'F' || candidate == 'E') {
                        repeat_until = candidate;
                        (void)tokens_->consume();
                    }
                }
            }
            if (repeat_limit == 0 && repeat_until == 'N') {
                throw ParseError("U requires a repetition count or stop condition", command.location);
            }
            auto body = consume_plain_text(*tokens_);
            if (body.empty()) throw ParseError("U requires a command body", command.location);
            return std::make_unique<RepeatCommandNode>(repeat_limit, repeat_until,
                                                        std::move(body), command.location);
        }
        if (mnemonic == 'V') {
            if (address) throw ParseError("V does not take an address", command.location);
            require_command_end();
            return std::make_unique<UndoCommandNode>(nullptr, command.location);
        }

        if (mnemonic == 'R') {
            if (zap_read && address) {
                throw ParseError("ZR does not accept line addresses", address->location());
            }
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
            if (zap_read && !filename) {
                throw ParseError("ZR requires a filename", command.location);
            }
            return std::make_unique<ReadCommandNode>(
                std::move(address), std::move(filename), command.location,
                zap_read);
        }

        if (mnemonic == 'W') {
            auto filename = parse_optional_filename();
            require_command_end();
            return std::make_unique<WriteCommandNode>(
                std::move(address), std::move(filename), write_mode,
                command.location, zap_append_write);
        }

        if (mnemonic == 'S') {
            auto [pattern, replacement, closed] = parse_substitution_parts();
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
                print_after, command.location, closed);
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
            if (repeat_control) {
                require_command_end();
                return std::make_unique<RepeatControlCommandNode>(
                    repeat_breaks, repeat_condition, command.location);
            }
            if (!message_newline) {
                throw ParseError("only JM, JP and JE are implemented",
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
                std::move(message), *message_newline, command.location,
                user_error_message);
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

        if (mnemonic == 'Y') {
            if (address) {
                throw ParseError("Y does not accept a line address", address->location());
            }
            require_command_end();
            return std::make_unique<WhyCommandNode>(nullptr, command.location);
        }

        if (mnemonic == 'O') {
            if (address) {
                throw ParseError("O does not accept a line address",
                                 address->location());
            }

            // Option names may be one or two letters, with suffix punctuation.
            const auto sign = tokens_->consume();
            if (sign.type != TokenType::Symbol ||
                (sign.lexeme != "+" && sign.lexeme != "-")) {
                throw ParseError("O requires '+' or '-'", sign.location);
            }

            const auto option = tokens_->consume();
            if (option.lexeme.empty() ||
                (option.type != TokenType::Command &&
                 option.type != TokenType::Identifier)) {
                throw ParseError("invalid O option",
                                 option.location);
            }
            std::string option_name = option.lexeme;
            std::transform(option_name.begin(), option_name.end(),
                           option_name.begin(), [](unsigned char ch) {
                               return static_cast<char>(std::toupper(ch));
                           });
            const bool enabled = sign.lexeme == "+";

            if (option_name == "M" || option_name == "Q" ||
                option_name == "SD" || option_name == "IF" ||
                option_name == "OL" || option_name == "OO" ||
                option_name == "OU" || option_name == "R") {
                const OptionKind kind = option_name == "M" ? OptionKind::Monitor
                    : option_name == "Q" ? OptionKind::QuitOnError
                    : option_name == "SD" ? OptionKind::CaseInsensitiveSearch
                    : option_name == "IF" ? OptionKind::InputFeed
                    : option_name == "OL" ? OptionKind::OutputLower
                    : option_name == "OO" ? OptionKind::OutputOctal
                    : option_name == "OU" ? OptionKind::OutputUpper
                    : OptionKind::PublicRead;
                require_command_end();
                return std::make_unique<OptionCommandNode>(
                    kind, enabled, command.location);
            }
            if (option_name == "C") {
                const auto location = tokens_->peek().location;
                const std::string raw = consume_plain_text(*tokens_);
                if (!enabled && !raw.empty()) {
                    throw ParseError("O-C does not accept a replacement", location);
                }
                const auto [from, to] = enabled ? parse_option_change(raw, location) :
                    std::pair<std::string, std::string>{};
                return std::make_unique<OptionCommandNode>(
                    OptionKind::InputChange, enabled, command.location,
                    std::vector<std::size_t>{}, from, to);
            }
            if (option_name == "B") {
                const auto raw = consume_plain_text(*tokens_);
                std::size_t pages = 0;
                const auto parsed = std::from_chars(raw.data(), raw.data() + raw.size(), pages);
                if (parsed.ec != std::errc{} || parsed.ptr != raw.data() + raw.size() ||
                    pages < 1 || pages > 65535) {
                    throw ParseError("OB expects a page count 1..65535", command.location);
                }
                return std::make_unique<OptionCommandNode>(
                    OptionKind::BufferPages, enabled, command.location,
                    std::vector<std::size_t>{}, std::to_string(pages));
            }
            if (option_name == "O") {
                const auto raw = consume_plain_text(*tokens_);
                if (raw.empty()) throw ParseError("OO requires output escape codes", command.location);
                return std::make_unique<OptionCommandNode>(
                    OptionKind::OutputCharacters, enabled, command.location,
                    std::vector<std::size_t>{}, raw);
            }
            if (option_name == "IE") {
                const auto location = tokens_->peek().location;
                const std::string raw = consume_plain_text(*tokens_);
                if ((enabled && raw.size() != 1) || (!enabled && !raw.empty())) {
                    throw ParseError("OIE requires one escape character after +", location);
                }
                if (enabled && (std::isalnum(static_cast<unsigned char>(raw[0])) ||
                                std::isspace(static_cast<unsigned char>(raw[0])))) {
                    throw ParseError("OIE requires a symbolic escape", location);
                }
                return std::make_unique<OptionCommandNode>(
                    OptionKind::InputEscape, enabled, command.location,
                    std::vector<std::size_t>{}, raw);
            }
            if (option_name == "P") {
                const auto location = tokens_->peek().location;
                const std::string raw = consume_plain_text(*tokens_);
                std::size_t value = 0;
                if (enabled) {
                    const auto parsed = std::from_chars(
                        raw.data(), raw.data() + raw.size(), value);
                    if (parsed.ec != std::errc{} ||
                        parsed.ptr != raw.data() + raw.size() ||
                        value < 1 || value > 4096) {
                        throw ParseError("O+P requires a page size 1..4096", location);
                    }
                } else if (!raw.empty()) {
                    throw ParseError("O-P takes no page size", location);
                }
                return std::make_unique<OptionCommandNode>(
                    OptionKind::PageSize, enabled, command.location,
                    std::vector<std::size_t>{}, enabled ? std::to_string(value) : "");
            }
            if (option_name == "IA" || option_name == "IC" || option_name == "IR") {
                const auto value_location = tokens_->peek().location;
                const std::string raw = consume_plain_text(*tokens_);
                if ((enabled && raw.empty()) || (!enabled && !raw.empty())) {
                    throw ParseError("OI prompt requires +delimited text or - without text",
                                     value_location);
                }
                auto prompt = enabled ? parse_option_prompt(raw, value_location) : std::string{};
                const auto kind = option_name == "IA" ? OptionKind::InputAppendPrompt
                                : option_name == "IC" ? OptionKind::InputCommandPrompt
                                : OptionKind::InputReturnMessage;
                return std::make_unique<OptionCommandNode>(
                    kind, enabled, command.location, std::vector<std::size_t>{},
                    std::move(prompt));
            }
            if (option_name == "IL" || option_name == "IU") {
                const auto value_location = tokens_->peek().location;
                const std::string raw = consume_plain_text(*tokens_);
                if ((enabled && raw.size() != 1) || (!enabled && !raw.empty())) {
                    throw ParseError("OI case escape requires one character after +, none after -",
                                     value_location);
                }
                if (enabled && (std::isspace(static_cast<unsigned char>(raw[0])) ||
                                std::isalnum(static_cast<unsigned char>(raw[0])))) {
                    throw ParseError("OI case escape must be a symbol", value_location);
                }
                return std::make_unique<OptionCommandNode>(
                    option_name == "IL" ? OptionKind::InputLowerEscape :
                    OptionKind::InputUpperEscape, enabled, command.location,
                    std::vector<std::size_t>{}, raw);
            }
            if (option_name == "S") {
                const auto raw = consume_plain_text(*tokens_);
                if (raw.empty()) throw ParseError("OS requires special characters", command.location);
                return std::make_unique<OptionCommandNode>(
                    raw == "&" ? OptionKind::SubstituteAmpersand :
                    raw == "/" ? OptionKind::SubstituteSeparator :
                    OptionKind::SpecialCharacters, enabled, command.location,
                    std::vector<std::size_t>{}, raw);
            }
            if (option_name == "T") {
                std::vector<std::size_t> stops;
                const auto location = tokens_->peek().location;
                const auto list = consume_plain_text(*tokens_);
                if (enabled && list.empty()) {
                    throw ParseError("O+T requires tab stops", location);
                }
                if (!enabled && !list.empty()) {
                    throw ParseError("O-T takes no tab stops", location);
                }
                std::size_t begin = 0;
                while (begin < list.size()) {
                    const auto comma = list.find(',', begin);
                    const auto end = comma == std::string::npos ? list.size() : comma;
                    std::size_t value = 0;
                    const auto* start = list.data() + begin;
                    const auto* finish = list.data() + end;
                    const auto result = std::from_chars(start, finish, value);
                    if (result.ec != std::errc{} || result.ptr != finish ||
                        value < 2 || value > 4096 ||
                        (!stops.empty() && value <= stops.back()) || stops.size() >= 64) {
                        throw ParseError("O+T requires increasing stops (2..4096)", location);
                    }
                    stops.push_back(value);
                    if (comma == std::string::npos) break;
                    begin = comma + 1;
                    if (begin == list.size()) {
                        throw ParseError("O+T has a trailing comma", location);
                    }
                }
                require_command_end();
                return std::make_unique<OptionCommandNode>(
                    OptionKind::TabStops, enabled, command.location,
                    std::move(stops));
            }

            if (option_name != "I") {
                throw ParseError("unsupported O option",
                                 option.location);
            }

            const auto opening = tokens_->consume();
            if (opening.type == TokenType::Backslash) {
                const auto final = tokens_->consume();
                if (final.lexeme != "F" && final.lexeme != "f") {
                    throw ParseError("OI\\F requires F", final.location);
                }
                require_command_end();
                return std::make_unique<OptionCommandNode>(
                    OptionKind::InputTerminatorRequired, enabled, command.location);
            }
            if (opening.type != TokenType::LeftParenthesis) {
                throw ParseError("OI( requires '('", opening.location);
            }
            require_command_end();
            return std::make_unique<OptionCommandNode>(
                OptionKind::InputParenthesis,
                enabled,
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

            CommandParser nested_parser(*tokens_, *registry_, context_);
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
            if (redo_undo) {
                if (address) throw ParseError("ZV does not take an address", command.location);
                require_command_end();
                return std::make_unique<RedoCommandNode>(nullptr, command.location);
            }
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

    apply_pattern_option_interpretation(source, source_interpretations);
    PatternParser parser(source, std::move(source_interpretations), opening.location.flow_level);
    return parser.parse();
}


std::tuple<std::unique_ptr<PatternNode>, std::string, bool>
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

    apply_pattern_option_interpretation(pattern_source, pattern_interpretations);
    PatternParser parser(pattern_source, std::move(pattern_interpretations), opening.location.flow_level);
    auto pattern = parser.parse();

    std::string replacement;
    bool closed = false;
    while (true) {
        const auto& next = tokens_->peek();
        if (next.type == TokenType::End || next.type == TokenType::NewLine) {
            break;
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
            closed = true;
            break;
        }
        replacement += token.lexeme;
    }

    return {std::move(pattern), std::move(replacement), closed};
}

/**
 * @brief Enforces command termination unless parse_one() enabled chaining.
 */
void CommandParser::apply_pattern_option_interpretation(
    std::string_view pattern,
    std::vector<CharacterInterpretation>& interpretations) const {
    if (!context_ || pattern.size() < 3) return;
    for (std::size_t i = 1; i + 1 < pattern.size(); ++i) {
        if (interpretations[i] == CharacterInterpretation::Normal &&
            !context_->pattern_character_special(pattern[i])) {
            interpretations[i] = CharacterInterpretation::Literal;
        }
    }
}

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
