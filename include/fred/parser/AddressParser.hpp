#pragma once

#include "fred/ast/AddressNode.hpp"
#include "fred/lexer/TokenStream.hpp"

#include <memory>

namespace fred {

/**
 * @brief Parses FRED line-address syntax from a TokenStream into AddressNode AST.
 *
 * Recognized single-address forms are absolute numbers, `.`, `$`, `+n` and
 * `-n`. Two single addresses separated by a comma form RangeAddressNode.
 *
 * @par Ownership and lifetime
 * AddressParser borrows TokenStream; it does not own it. The TokenStream must
 * outlive the parser. Returned AST nodes are owned by the caller through
 * std::unique_ptr.
 *
 * @par Rollback
 * Both parse() and parse_prefix() save the starting token position and restore
 * it if any exception escapes parsing. This lets higher-level parsers attempt
 * address parsing transactionally.
 *
 * @par Architectural boundary
 * This parser does not access Buffer state and does not validate whether a line
 * exists. Numeric zero is preserved in the AST.
 */
class AddressParser {
public:
    /**
     * @param tokens Token stream to read. Ownership is not transferred.
     * @pre `tokens` must outlive this parser.
     */
    explicit AddressParser(TokenStream& tokens) noexcept;

    /**
     * @brief Parses one complete address expression.
     *
     * @return Owned AddressNode tree.
     *
     * @throws ParseError for invalid address syntax, trailing input, missing
     *         range/relative operands, or numeric overflow.
     *
     * @post On success, input is consumed through End, optionally accepting one
     *       trailing NewLine before End.
     * @post On failure, TokenStream::position() is restored to its entry value.
     */
    [[nodiscard]] std::unique_ptr<AddressNode> parse();

    /**
     * @brief Parses an address prefix without requiring end-of-input.
     *
     * Intended for CommandParser, where a command mnemonic follows the address.
     *
     * @return Owned AddressNode tree.
     * @throws ParseError on invalid address syntax or numeric overflow.
     * @post On failure, the TokenStream cursor is restored to its entry value.
     */
    [[nodiscard]] std::unique_ptr<AddressNode> parse_prefix();

private:
    /** @brief Parses one non-range address atom at the current token. */
    [[nodiscard]] std::unique_ptr<AddressNode> parse_single_address();

    /**
     * @brief Converts an already-tokenized decimal number without wraparound.
     *
     * @param token TokenType::Number token.
     * @return Value converted to std::size_t.
     * @throws ParseError when the decimal spelling exceeds std::size_t.
     */
    [[nodiscard]] std::size_t parse_number(const Token& token) const;

    /**
     * @brief Enforces `[ NewLine ] End` after a complete address expression.
     * @throws ParseError when additional tokens remain.
     */
    void require_expression_end();

    /** Borrowed token stream; never owned by AddressParser. */
    TokenStream* tokens_;
};

} // namespace fred
