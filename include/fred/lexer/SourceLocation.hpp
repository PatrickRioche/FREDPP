#pragma once

#include <cstddef>

namespace fred {

struct SourceLocation {
    std::size_t offset{0};
    std::size_t line{1};
    std::size_t column{1};
    std::size_t flow_level{0};

    friend bool operator==(const SourceLocation&, const SourceLocation&) = default;
};

} // namespace fred
