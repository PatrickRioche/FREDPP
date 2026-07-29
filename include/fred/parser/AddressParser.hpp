#pragma once

#include "fred/ast/AddressNode.hpp"
#include "fred/lexer/TokenStream.hpp"

#include <memory>

namespace fred {

class AddressParser {
public:
    explicit AddressParser(TokenStream& tokens) noexcept;

    // Parses one complete address expression and requires End (or one
    // trailing NewLine followed by End). The TokenStream is rewound if
    // parsing fails.
    [[nodiscard]] std::unique_ptr<AddressNode> parse();

    // Parses an address expression at the current token position without
    // requiring end-of-input. Intended for higher-level parsers.
    [[nodiscard]] std::unique_ptr<AddressNode> parse_prefix();

private:
    [[nodiscard]] std::unique_ptr<AddressNode> parse_single_address();
    [[nodiscard]] std::size_t parse_number(const Token& token) const;
    void require_expression_end();

    TokenStream* tokens_;
};

} // namespace fred
