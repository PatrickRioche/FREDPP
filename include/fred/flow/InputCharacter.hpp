#pragma once

#include "fred/core/CharacterInterpretation.hpp"

#include <cstddef>

namespace fred {

/**
 * @brief One character traveling through the FRED flow-expansion layer.
 *
 * InputCharacter carries three independent pieces of information:
 *
 * - the character byte itself;
 * - the flow/input nesting level that produced it;
 * - lexical interpretation metadata preserving literal/forced-special intent.
 *
 * The metadata is intentionally retained until FlowCharacterStream/Lexer so
 * FREDPP can model historical flow semantics without replacing source bytes by
 * machine-specific character codes.
 */
struct InputCharacter {
    /** Character byte emitted by the current input source/expansion. */
    char value{};

    /**
     * Flow/input nesting level.
     *
     * Level 0 denotes the outer source. Injected buffers normally use higher
     * levels so delimiters/directives cannot accidentally cross source levels.
     */
    std::size_t level{};

    /** How later lexical/pattern stages should interpret this byte. */
    CharacterInterpretation interpretation{
        CharacterInterpretation::Normal};
};

} // namespace fred
