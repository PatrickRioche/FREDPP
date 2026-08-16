#pragma once

#include "fred/core/CharacterInterpretation.hpp"

#include <cstddef>

namespace fred {

struct InputCharacter {
    char value{};
    std::size_t level{};
    CharacterInterpretation interpretation{CharacterInterpretation::Normal};
};

} // namespace fred
