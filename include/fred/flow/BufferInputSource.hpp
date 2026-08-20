#pragma once

#include "fred/core/Buffer.hpp"
#include "fred/flow/InputSource.hpp"

#include <cstddef>
#include <optional>
#include <string>

namespace fred {

/**
 * @brief InputSource that traverses the lines of an existing Buffer.
 *
 * BufferInputSource adapts Buffer content to the character-oriented flow
 * engine. Depending on configuration it either emits a `\n` after every Buffer
 * line or concatenates logical lines with no separator.
 *
 * @par Ownership and lifetime
 * The Buffer is borrowed through a raw const pointer and must outlive this
 * source. The source owns only its description/cursor state.
 *
 * @par Interpretation propagation
 * The configured CharacterInterpretation is applied to every emitted
 * character, including generated newline characters.
 *
 * @warning Buffer content is expected to remain stable while this source is
 * being consumed. Mutating/erasing the borrowed Buffer during traversal is not
 * part of the source contract.
 */
class BufferInputSource final : public InputSource {
public:
    /**
     * @param buffer Buffer to traverse; ownership is not transferred.
     * @param level Flow/input nesting level attached to emitted characters.
     * @param emit_newlines Whether to emit `\n` after every logical line.
     * @param interpretation Metadata applied to all emitted characters.
     *
     * @pre `buffer` must outlive this source.
     */
    BufferInputSource(
        const Buffer& buffer,
        std::size_t level,
        bool emit_newlines = true,
        CharacterInterpretation interpretation =
            CharacterInterpretation::Normal);

    /**
     * @return Next Buffer byte/generated newline, or nullopt after the final
     *         line has been fully consumed.
     *
     * @note When emit_newlines is true, a newline is emitted after every stored
     *       line, including the final line.
     */
    [[nodiscard]] std::optional<InputCharacter> next() override;

    /**
     * @return View of the owned description `buffer(<name>)`.
     */
    [[nodiscard]] std::string_view
    description() const noexcept override;

    /** @return Configured flow/input level. */
    [[nodiscard]] std::size_t level() const noexcept override;

private:
    /** Borrowed Buffer; never owned/deleted here. */
    const Buffer* buffer_;

    /** Owned diagnostic description, making description() view stable. */
    std::string description_;

    std::size_t level_;
    bool emit_newlines_;
    CharacterInterpretation interpretation_;

    /** Zero-based logical line currently being traversed. */
    std::size_t line_index_{0};

    /** Zero-based byte position in the current line. */
    std::size_t column_index_{0};

    /** Whether the current line's generated newline was already emitted. */
    bool emit_newline_{false};
};

} // namespace fred
