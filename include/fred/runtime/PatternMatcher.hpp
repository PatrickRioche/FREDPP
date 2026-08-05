#pragma once

#include "fred/ast/PatternNodes.hpp"

#include <string_view>

namespace fred {

class PatternMatcher {
public:
    [[nodiscard]] bool search(const PatternNode& pattern,
                              std::string_view text) const;
};

} // namespace fred
