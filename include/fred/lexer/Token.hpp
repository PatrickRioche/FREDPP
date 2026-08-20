#pragma once

#include "fred/core/CharacterInterpretation.hpp"
#include "fred/lexer/SourceLocation.hpp"
#include "fred/lexer/TokenType.hpp"

#include <string>

namespace fred {

/**
 * @brief Value object produced by Lexer and consumed by parser layers.
 *
 * A Token owns its lexeme string. It also retains the location and
 * interpretation of the first character from which the token was formed.
 *
 * @par Ownership
 * Token is self-contained: it does not hold references to Lexer,
 * CharacterStream or the original source string.
 */
struct Token {
    /** Lexical category. */
    TokenType type{TokenType::Unknown};

    /** Exact bytes grouped into this token; empty for TokenType::End. */
    std::string lexeme;

    /** Location of the token's first character, or end location for End. */
    SourceLocation location;

    /**
     * Interpretation metadata propagated from the token's first character.
     *
     * This allows parser/runtime layers to distinguish ordinary syntax from
     * syntax originating from FRED flow processing when required.
     */
    CharacterInterpretation interpretation{CharacterInterpretation::Normal};

    friend bool operator==(const Token&, const Token&) = default;
};

} // namespace fred
