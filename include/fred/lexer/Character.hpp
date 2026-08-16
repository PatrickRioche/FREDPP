#pragma once

#include "fred/core/CharacterInterpretation.hpp"
#include "fred/lexer/SourceLocation.hpp"

namespace fred {

struct Character {
    char value{};
    SourceLocation location{};
    CharacterInterpretation interpretation{CharacterInterpretation::Normal};

    friend bool operator==(const Character&, const Character&) = default;
};

} // namespace fred
