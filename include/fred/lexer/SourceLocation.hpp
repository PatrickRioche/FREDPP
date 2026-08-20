#pragma once

#include <cstddef>

namespace fred {

/**
 * @brief Identifies one position in the FRED source stream.
 *
 * SourceLocation is attached to characters, tokens and diagnostics so that
 * later parser/runtime layers can report the original position of an input
 * element without re-reading the source text.
 *
 * Line and column numbers are one-based. The byte/character offset is
 * zero-based. `flow_level` records the FRED flow-expansion level associated
 * with the source element.
 *
 * @note This type contains location metadata only. It does not own or refer to
 *       source text.
 */
struct SourceLocation {
    /** Zero-based offset in the character stream. */
    std::size_t offset{0};

    /** One-based source line. */
    std::size_t line{1};

    /** One-based source column. */
    std::size_t column{1};

    /**
     * FRED flow-expansion level from which the character/token originates.
     *
     * Level 0 denotes ordinary top-level input. Higher values are propagated
     * by flow-aware character streams.
     */
    std::size_t flow_level{0};

    friend bool operator==(const SourceLocation&, const SourceLocation&) = default;
};

} // namespace fred
