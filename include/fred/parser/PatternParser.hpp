#pragma once

#include "fred/ast/PatternNodes.hpp"
#include "fred/core/CharacterInterpretation.hpp"

#include <memory>
#include <string_view>
#include <vector>

namespace fred {

/**
 * @brief Recursive-descent parser for the implemented FRED pattern language.
 *
 * PatternParser consumes one complete *delimited* pattern string and builds an
 * owned PatternNode tree. It parses syntax only; matching is performed later
 * by PatternMatcher.
 *
 * Implemented core grammar:
 *
 * @code
 * pattern      := alternation
 * alternation  := sequence ("|" sequence)*
 * sequence     := repetition*
 * repetition   := atom ("*" | "+")?
 * atom         := literal | "." | "^" | "$" | group | class
 * group        := "(" alternation ")"
 * class        := "[" "^"? class-item+ "]"
 * @endcode
 *
 * @par Source ownership
 * `source_` is a borrowed std::string_view. The original character storage must
 * remain valid while parse() is running. Interpretation metadata, when
 * supplied, is copied/moved into the parser.
 *
 * @par Flow interpretation
 * A CharacterInterpretation::Literal position is never treated as pattern
 * metasyntax. ForcedSpecial remains syntactically special. This preserves the
 * result of FRED flow processing when patterns are reconstructed by
 * CommandParser.
 *
 * @par Current location limit
 * SourceLocation produced by this parser is relative to the pattern fragment:
 * line is always 1, offset starts at 0 and column starts at 1. `flow_level` is
 * propagated, but original command-line columns are not retained.
 *
 * @par Current language limits
 * This parser intentionally does not silently implement PCRE. Historical FRED
 * forms not yet implemented include column assertions, tags, word boundaries,
 * defined patterns, fence and several class/case modifiers documented in
 * SPEC-007.
 */
class PatternParser {
public:
    /**
     * @brief Constructs a parser with ordinary interpretation for every byte.
     *
     * @param source Complete delimited pattern source.
     * @param flow_level Flow level attached to generated SourceLocation values.
     *
     * @pre `source` storage must remain valid until parsing is complete.
     */
    explicit PatternParser(std::string_view source,
                           std::size_t flow_level = 0) noexcept;

    /**
     * @brief Constructs a parser with per-byte flow interpretation metadata.
     *
     * @param source Complete delimited pattern source.
     * @param interpretations Either empty, or exactly one entry per source byte.
     * @param flow_level Flow level attached to generated SourceLocation values.
     *
     * @throws std::invalid_argument when a non-empty interpretation vector has
     *         a different size from `source`.
     */
    PatternParser(
        std::string_view source,
        std::vector<CharacterInterpretation> interpretations,
        std::size_t flow_level = 0);

    /**
     * @brief Parses the complete delimited pattern.
     *
     * @return Owned root PatternNode.
     * @throws ParseError on malformed delimiter, unterminated constructs,
     *         invalid repetition/alternation, dangling escapes or trailing text.
     *
     * @note Direct PatternParser accepts any non-alphanumeric, non-whitespace
     *       symbolic delimiter except `_` and `\`. CommandParser may impose
     *       narrower command-specific delimiter rules (for example G uses
     *       `/` or `?`).
     */
    [[nodiscard]] std::unique_ptr<PatternNode> parse();

private:
    [[nodiscard]] std::unique_ptr<PatternNode> parse_alternation();
    [[nodiscard]] std::unique_ptr<PatternNode> parse_sequence();
    [[nodiscard]] std::unique_ptr<PatternNode> parse_repetition();
    [[nodiscard]] std::unique_ptr<PatternNode> parse_atom();
    [[nodiscard]] std::unique_ptr<PatternNode> parse_group();
    [[nodiscard]] std::unique_ptr<PatternNode> parse_character_class();

    [[nodiscard]] bool at_end() const noexcept;

    /**
     * @brief Tests whether source[position] may act as pattern metasyntax.
     *
     * @return false for out-of-range or Literal positions; true for Normal and
     *         ForcedSpecial positions.
     */
    [[nodiscard]] bool is_special(std::size_t position) const noexcept;

    [[nodiscard]] bool peek_is_special(
        char value,
        std::size_t lookahead = 0) const noexcept;

    [[nodiscard]] char peek(std::size_t lookahead = 0) const noexcept;

    /**
     * @brief Consumes one source byte.
     * @throws ParseError when called at end of source.
     */
    char consume();

    /** @return Fragment-relative current SourceLocation. */
    [[nodiscard]] SourceLocation location() const noexcept;

    /** @throws ParseError unconditionally with the current location. */
    [[noreturn]] void fail(std::string_view message) const;

    /** Borrowed complete pattern fragment. */
    std::string_view source_;

    /** Optional owned interpretation metadata, one entry per source byte. */
    std::vector<CharacterInterpretation> interpretations_;

    /** Current zero-based position within source_. */
    std::size_t position_{};

    /** Flow level propagated to generated locations. */
    std::size_t flow_level_{};

    /** Opening delimiter selected by parse(). */
    char delimiter_{};
};

/**
 * @brief Produces a deterministic debug description of a pattern AST.
 *
 * @param node Pattern root to describe.
 * @return Newly allocated human-readable structural description.
 *
 * @note This helper is for inspection/tests; it does not reproduce source text
 *       and it does not perform pattern matching.
 */
[[nodiscard]] std::string describe_pattern(const PatternNode& node);

} // namespace fred
