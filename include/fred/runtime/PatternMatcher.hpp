#pragma once

#include "fred/ast/PatternNodes.hpp"

#include <cstddef>
#include <optional>
#include <string_view>

namespace fred {

/**
 * @brief Half-open match interval `[start,end)` in a searched string.
 */
struct PatternMatch {
    /** Zero-based byte offset of the first matched character. */
    std::size_t start{};

    /** Zero-based byte offset immediately after the match. */
    std::size_t end{};
};

/**
 * @brief Executes the currently implemented PatternNode AST against text.
 *
 * PatternMatcher operates on std::string_view byte offsets. It does not parse
 * pattern syntax and does not implement PCRE; it executes only the AST forms
 * produced by FREDPP PatternParser.
 *
 * @par Search policy
 * find() scans candidate starts from `start_offset` through text.size().
 * When several end positions are possible at the first matching start, the
 * longest end is selected.
 *
 * @par Zero-length matches
 * Anchors and repetitions may produce start==end. Callers performing repeated
 * substitution must advance explicitly to avoid infinite loops.
 */
class PatternMatcher {
public:
    /**
     * @param pattern Parsed pattern AST.
     * @param text Text to search; not owned.
     * @param start_offset First candidate byte offset.
     * @return First-start/longest-end match, or nullopt if none.
     *
     * @note start_offset > text.size() returns nullopt.
     */
    [[nodiscard]] std::optional<PatternMatch> find(
        const PatternNode& pattern,
        std::string_view text,
        std::size_t start_offset = 0) const;

    /**
     * @return true when find(pattern,text) succeeds.
     */
    [[nodiscard]] bool search(const PatternNode& pattern,
                              std::string_view text) const;
};

} // namespace fred
