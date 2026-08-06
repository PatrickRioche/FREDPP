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
        if (command.type == TokenType::Identifier && command.lexeme.size() == 2 &&
            std::toupper(static_cast<unsigned char>(command.lexeme[0])) == 'Q' &&
            std::toupper(static_cast<unsigned char>(command.lexeme[1])) == 'Q') {
            command.type = TokenType::Command;
            command.lexeme = "Q";
            immediate_quit = true;
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
            auto buffer_name = parse_parenthesized_buffer_name();
            require_command_end();
            return std::make_unique<BufferCommandNode>(
                std::move(buffer_name), command.location);
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

            std::string filename;
            while (tokens_->peek().type != TokenType::End &&
                   tokens_->peek().type != TokenType::NewLine) {
                filename += tokens_->consume().lexeme;
            }
            require_command_end();
            if (filename.empty()) {
                return std::make_unique<ListCommandNode>(std::nullopt,
                                                         command.location);
            }
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
