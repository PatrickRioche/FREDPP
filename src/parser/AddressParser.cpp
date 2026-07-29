#include "fred/parser/AddressParser.hpp"

#include "fred/ast/AbsoluteAddressNode.hpp"
#include "fred/ast/CurrentAddressNode.hpp"
#include "fred/ast/LastAddressNode.hpp"
#include "fred/ast/RangeAddressNode.hpp"
#include "fred/ast/RelativeAddressNode.hpp"
#include "fred/parser/ParseError.hpp"

#include <limits>
#include <string>
#include <utility>

namespace fred {

AddressParser::AddressParser(TokenStream& tokens) noexcept
    : tokens_(&tokens) {}

std::unique_ptr<AddressNode> AddressParser::parse() {
    const auto mark = tokens_->position();

    try {
        auto address = parse_prefix();
        require_expression_end();
        return address;
    } catch (...) {
        tokens_->rewind(mark);
        throw;
    }
}

std::unique_ptr<AddressNode> AddressParser::parse_prefix() {
    const auto mark = tokens_->position();

    try {
        auto first = parse_single_address();

        if (tokens_->peek().type == TokenType::Comma) {
            const auto range_location = first->location();
            (void)tokens_->consume();
            auto last = parse_single_address();
            first = std::make_unique<RangeAddressNode>(
                std::move(first), std::move(last), range_location);
        }

        return first;
    } catch (...) {
        tokens_->rewind(mark);
        throw;
    }
}

std::unique_ptr<AddressNode> AddressParser::parse_single_address() {
    const Token token = tokens_->consume();

    if (token.type == TokenType::Number) {
        return std::make_unique<AbsoluteAddressNode>(
            parse_number(token), token.location);
    }

    if (token.type == TokenType::Symbol && token.lexeme == ".") {
        return std::make_unique<CurrentAddressNode>(token.location);
    }

    if (token.type == TokenType::Symbol && token.lexeme == "$") {
        return std::make_unique<LastAddressNode>(token.location);
    }

    if (token.type == TokenType::Symbol &&
        (token.lexeme == "+" || token.lexeme == "-")) {
        const Token distance = tokens_->consume();
        if (distance.type != TokenType::Number) {
            throw ParseError("expected a number after relative address sign",
                             distance.location);
        }

        const auto direction = token.lexeme == "+"
            ? RelativeDirection::Forward
            : RelativeDirection::Backward;

        return std::make_unique<RelativeAddressNode>(
            direction, parse_number(distance), token.location);
    }

    if (token.type == TokenType::End || token.type == TokenType::NewLine) {
        throw ParseError("expected an address", token.location);
    }

    throw ParseError("invalid address token '" + token.lexeme + "'",
                     token.location);
}

std::size_t AddressParser::parse_number(const Token& token) const {
    std::size_t value = 0;

    for (const char digit : token.lexeme) {
        const auto numeric = static_cast<std::size_t>(digit - '0');
        if (value > (std::numeric_limits<std::size_t>::max() - numeric) / 10) {
            throw ParseError("address number is too large", token.location);
        }
        value = value * 10 + numeric;
    }

    return value;
}

void AddressParser::require_expression_end() {
    if (tokens_->peek().type == TokenType::NewLine) {
        (void)tokens_->consume();
    }

    const auto& token = tokens_->peek();
    if (token.type != TokenType::End) {
        throw ParseError("unexpected token after address expression: '" +
                             token.lexeme + "'",
                         token.location);
    }
}

} // namespace fred
