#pragma once

#include "fred/lexer/CharacterStream.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace fred {

/**
 * @brief CharacterStream implementation backed by an owned string copy.
 *
 * The constructor copies `source` into internal storage and precomputes a
 * Character for every byte, including SourceLocation information. This makes
 * peek(), consume() and rewind() independent from the lifetime of the original
 * std::string_view.
 *
 * @par Position model
 * - offsets are zero-based;
 * - lines and columns are one-based;
 * - '\n' advances the line and resets the next column to 1;
 * - all characters receive the same constructor-supplied flow level.
 *
 * @par Limits
 * This class is byte-oriented because FREDPP's lexer currently operates on
 * `char`. It does not decode UTF-8 code points or perform newline
 * canonicalization.
 */
class StringCharacterStream final : public CharacterStream {
public:
    /**
     * @brief Copies source text and builds positional metadata.
     *
     * @param source Source bytes to expose through the CharacterStream API.
     *        The caller's storage may be destroyed after construction.
     * @param flow_level Flow level attached to every generated SourceLocation.
     */
    explicit StringCharacterStream(std::string_view source,
                                   std::size_t flow_level = 0);

    /**
     * @copydoc CharacterStream::peek
     */
    [[nodiscard]] std::optional<Character>
    peek(std::size_t lookahead = 0) const noexcept override;

    /**
     * @copydoc CharacterStream::consume
     */
    [[nodiscard]] std::optional<Character> consume() noexcept override;

    /** @copydoc CharacterStream::eof */
    [[nodiscard]] bool eof() const noexcept override;

    /** @copydoc CharacterStream::position */
    [[nodiscard]] std::size_t position() const noexcept override;

    /**
     * @copydoc CharacterStream::rewind
     *
     * @throws std::out_of_range when `position > size()`.
     */
    void rewind(std::size_t position) override;

    /** @copydoc CharacterStream::end_location */
    [[nodiscard]] SourceLocation end_location() const noexcept override;

    /**
     * @brief Returns the number of stored characters.
     *
     * @return Number of bytes/Character entries copied from the input source.
     */
    [[nodiscard]] std::size_t size() const noexcept;

private:
    /** Owned copy preserving the original bytes. */
    std::string storage_;

    /** Precomputed character values and source metadata. */
    std::vector<Character> characters_;

    /** Current zero-based cursor into characters_. */
    std::size_t position_{0};

    /** Source location immediately after the final stored character. */
    SourceLocation end_location_{};
};

} // namespace fred
