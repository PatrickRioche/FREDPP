#include "fred/parser/CommandParser.hpp"

#include "fred/parser/AddressParser.hpp"
#include "fred/parser/ParseError.hpp"

#include <cctype>
#include <optional>
#include <sstream>
#include <string>
#include <utility>

namespace fred {

CommandParser::CommandParser(TokenStream& tokens,
                             const CommandRegistry& registry) noexcept
    : tokens_(&tokens), registry_(&registry) {}

std::unique_ptr<CommandNode> CommandParser::parse() {
    const auto mark = tokens_->position();

    try {
        std::unique_ptr<AddressNode> address;
        if (begins_address(tokens_->peek())) {
            AddressParser address_parser(*tokens_);
            address = address_parser.parse_prefix();
        }

        const Token command = tokens_->consume();
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
