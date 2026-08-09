#pragma once

#include <cstddef>

namespace fred {

struct InputCharacter {
    char value{};
    std::size_t level{};
    bool literal{};
};

} // namespace fred
