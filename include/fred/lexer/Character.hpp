#pragma once

#include "fred/lexer/SourceLocation.hpp"

namespace fred {

struct Character {
    char value{};
    SourceLocation location{};

    friend bool operator==(const Character&, const Character&) = default;
};

} // namespace fred
