#pragma once

#include "fred/core/CharacterInterpretation.hpp"
#include "fred/lexer/SourceLocation.hpp"

namespace fred {

/**
 * @brief One logical character consumed by the lexical layer.
 *
 * A Character combines the byte value used by the lexer with its original
 * source location and the interpretation metadata produced by the FRED flow
 * layer.
 *
 * Character is intentionally a value type. CharacterStream::peek() and
 * CharacterStream::consume() return copies of it; no lifetime relationship
 * with the underlying stream is exposed to callers.
 */
struct Character {
    /** Character byte presented to the lexer. */
    char value{};

    /** Original position and flow level of this character. */
    SourceLocation location{};

    /**
     * Lexical interpretation assigned before tokenization.
     *
     * Normal characters participate in ordinary lexical syntax. Literal and
     * ForcedSpecial values allow the flow layer to preserve historical FRED
     * interpretation semantics without coupling Lexer to FlowEngine.
     */
    CharacterInterpretation interpretation{CharacterInterpretation::Normal};

    friend bool operator==(const Character&, const Character&) = default;
};

} // namespace fred
