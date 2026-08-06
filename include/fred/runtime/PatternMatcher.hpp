#pragma once

#include "fred/ast/PatternNodes.hpp"

#include <cstddef>
#include <optional>
#include <string_view>

namespace fred {

struct PatternMatch {
    std::size_t start{};
    std::size_t end{};
};

class PatternMatcher {
public:
    [[nodiscard]] std::optional<PatternMatch> find(
        const PatternNode& pattern,
        std::string_view text,
        std::size_t start_offset = 0) const;

    [[nodiscard]] bool search(const PatternNode& pattern,
                              std::string_view text) const;
};

} // namespace fred
