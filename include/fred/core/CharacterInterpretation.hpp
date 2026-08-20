#pragma once

namespace fred {

/**
 * @brief Controls how a character produced by the FRED flow layer is lexed.
 *
 * This metadata is deliberately separate from the character byte itself. It
 * lets FREDPP preserve historical flow/directive semantics without replacing
 * characters with platform-specific encoded values.
 */
enum class CharacterInterpretation {
    /**
     * Ordinary source character.
     *
     * Lexer applies normal lexical rules, including structural punctuation and
     * horizontal-whitespace skipping.
     */
    Normal,

    /**
     * Character protected from ordinary lexical structure where applicable.
     *
     * Lexer currently turns Literal punctuation/whitespace into Symbol, while
     * letters, digits and '_' remain eligible to form identifiers/numbers.
     */
    Literal,

    /**
     * Character explicitly required to retain special/structural meaning.
     *
     * Lexer applies normal structural classification but preserves this value
     * in Token::interpretation.
     */
    ForcedSpecial
};

} // namespace fred
