#pragma once

#include "fred/lexer/Character.hpp"

#include <cstddef>
#include <optional>

namespace fred {

/**
 * @brief Abstract, rewindable source of characters for Lexer.
 *
 * CharacterStream is the boundary between character production and lexical
 * analysis. Lexer depends only on this interface and therefore does not need
 * to know whether characters come directly from a string or from the FRED
 * flow-expansion machinery.
 *
 * Implementations own their character storage. Returned Character objects are
 * values, not references into that storage.
 *
 * @par Architectural boundary
 * CharacterStream may attach SourceLocation and CharacterInterpretation
 * metadata, but it does not classify tokens and must not perform command
 * parsing or runtime execution.
 */
class CharacterStream {
public:
    virtual ~CharacterStream() = default;

    /**
     * @brief Reads a character without advancing the current position.
     *
     * @param lookahead Number of characters ahead of the current position.
     *        `0` means the current character.
     * @return The requested Character, or std::nullopt when the requested
     *         position is at or beyond end of stream.
     *
     * @note The current stream position is never modified.
     */
    [[nodiscard]] virtual std::optional<Character>
    peek(std::size_t lookahead = 0) const noexcept = 0;

    /**
     * @brief Returns the current character and advances by one.
     *
     * @return The consumed Character, or std::nullopt when already at EOF.
     *
     * @post On success position() is incremented by one.
     * @post At EOF the position remains unchanged.
     */
    [[nodiscard]] virtual std::optional<Character> consume() noexcept = 0;

    /**
     * @brief Tests whether no character remains at the current position.
     *
     * @return true when position() is at or beyond the stored character count.
     */
    [[nodiscard]] virtual bool eof() const noexcept = 0;

    /**
     * @brief Returns the current zero-based stream cursor.
     *
     * The value is suitable for saving a mark and later passing it to rewind()
     * while the same stream object remains alive.
     */
    [[nodiscard]] virtual std::size_t position() const noexcept = 0;

    /**
     * @brief Moves the cursor to an absolute stream position.
     *
     * @param position Zero-based character index. The position immediately
     *        after the final character is valid and represents EOF.
     *
     * @throws std::out_of_range if the implementation rejects the requested
     *         position as outside its stored character range.
     *
     * @note Existing FREDPP implementations allow rewinding to any valid
     *       absolute character position, including positions ahead of the
     *       current cursor.
     */
    virtual void rewind(std::size_t position) = 0;

    /**
     * @brief Returns the source location immediately after the last character.
     *
     * @return Stable end-of-input location used for EOF tokens and diagnostics.
     */
    [[nodiscard]] virtual SourceLocation end_location() const noexcept = 0;
};

} // namespace fred
