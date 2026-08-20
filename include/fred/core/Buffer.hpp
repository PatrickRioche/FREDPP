#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace fred {

/**
 * @brief Text encoding associated with a Buffer file.
 *
 * Buffer stores the detected/selected file encoding as metadata. It does not
 * itself perform encoding conversion or file I/O.
 */
enum class TextEncoding {
    /** Encoding is not known or no file metadata has established one yet. */
    Unknown,

    /** ASCII file representation. */
    Ascii,

    /** UTF-8 file representation. */
    Utf8
};

/** @brief Line-ending convention associated with a Buffer file. */
enum class LineEnding {
    /** Line-feed (`\n`) convention. */
    Lf,

    /** Carriage-return + line-feed (`\r\n`) convention. */
    CrLf
};

/**
 * @brief In-memory editable FRED line buffer.
 *
 * Buffer owns a sequence of logical text lines together with the current-line
 * cursor, modified flag and optional file metadata.
 *
 * @par Line numbering
 * Existing lines are addressed with one-based numbers. `0` is reserved as the
 * no-current-line position and is valid for an empty buffer. set_current_line()
 * also accepts 0 explicitly.
 *
 * @par Ownership and reference lifetime
 * The Buffer owns all line strings. line() and lines() return non-owning const
 * references that may be invalidated by later mutations of the line vector.
 *
 * @par Architectural boundary
 * Buffer knows nothing about FRED source parsing, flow directives, addresses or
 * command execution. Higher layers decide which Buffer operation to call.
 */
class Buffer {
public:
    /** Public line-number type used throughout the core/runtime boundary. */
    using LineNumber = std::size_t;

    /**
     * @brief Constructs an empty, clean buffer.
     *
     * @param name Non-empty buffer name.
     * @throws std::invalid_argument when `name` is empty.
     *
     * @note The global FREDPP maximum buffer-name length is enforced by
     *       BufferManager, not by this low-level constructor.
     */
    explicit Buffer(std::string name);

    /** @return Stable buffer name owned by this object. */
    [[nodiscard]] const std::string& name() const noexcept;

    /** @return true when the buffer contains no logical lines. */
    [[nodiscard]] bool empty() const noexcept;

    /** @return Number of logical lines. */
    [[nodiscard]] std::size_t line_count() const noexcept;

    /**
     * @return Current one-based line number, or 0 when no line is current.
     */
    [[nodiscard]] LineNumber current_line() const noexcept;

    /**
     * @brief Returns one existing line.
     *
     * @param number One-based existing line number.
     * @return Non-owning const reference to the stored line.
     * @throws std::out_of_range for 0 or a number above line_count().
     */
    [[nodiscard]] const std::string& line(LineNumber number) const;

    /**
     * @return Non-owning const reference to the complete line vector.
     *
     * @warning Later mutations may invalidate references/iterators into the
     *          returned vector.
     */
    [[nodiscard]] const std::vector<std::string>& lines() const noexcept;

    /** @return Whether buffer content is marked as modified/unsaved. */
    [[nodiscard]] bool modified() const noexcept;

    /** @return true when file metadata contains an associated filename. */
    [[nodiscard]] bool has_associated_file() const noexcept;

    /** @return Optional associated filename owned by the Buffer. */
    [[nodiscard]] const std::optional<std::string>&
    associated_file() const noexcept;

    /** @return Stored file-encoding metadata. */
    [[nodiscard]] TextEncoding encoding() const noexcept;

    /** @return Stored line-ending metadata. */
    [[nodiscard]] LineEnding line_ending() const noexcept;

    /** @return Whether the associated file representation ends in newline. */
    [[nodiscard]] bool final_newline() const noexcept;

    /**
     * @brief Appends one line at the end.
     *
     * @param text Line text, without a logical line-ending byte.
     * @post The appended line becomes current.
     * @post The buffer is marked modified.
     */
    void append(std::string text);

    /**
     * @brief Inserts one line immediately before `number`.
     *
     * `line_count() + 1` is accepted as append-at-end.
     *
     * @param number Existing one-based line, or line_count()+1.
     * @param text New line.
     * @throws std::out_of_range for other invalid positions.
     * @post The inserted line becomes current and the buffer is modified.
     */
    void insert_before(LineNumber number, std::string text);

    /**
     * @brief Inserts zero or more lines after a position.
     *
     * @param number Position in the inclusive range 0..line_count(). `0` inserts
     *        before the first existing line.
     * @param text Lines to move into this Buffer.
     * @throws std::out_of_range when number > line_count().
     *
     * @post For non-empty input, the last inserted line becomes current and the
     *       buffer is marked modified.
     * @post For empty input, content/modified state are unchanged and
     *       current_line becomes `number`.
     */
    void insert_after(LineNumber number, std::vector<std::string> text);

    /**
     * @brief Replaces one existing line.
     *
     * @param number One-based existing line.
     * @param text Replacement line.
     * @throws std::out_of_range for an invalid line.
     *
     * @post The selected line becomes current.
     * @post modified() changes to true only when text actually differs.
     */
    void replace(LineNumber number, std::string text);

    /**
     * @brief Erases an inclusive range of existing lines.
     *
     * @param first First existing one-based line.
     * @param last Last existing one-based line.
     * @throws std::out_of_range if either endpoint does not exist.
     * @throws std::invalid_argument if first > last.
     *
     * @post The buffer is marked modified.
     * @post current_line() becomes 0 if empty, otherwise the line now occupying
     *       `first` when it exists, otherwise the new last line.
     */
    void erase(LineNumber first, LineNumber last);

    /**
     * @brief Changes only the current-line cursor.
     *
     * @param number Position in 0..line_count().
     * @throws std::out_of_range above line_count().
     *
     * @note This does not change modified().
     */
    void set_current_line(LineNumber number);

    /**
     * @brief Replaces content and file metadata with a loaded file image.
     *
     * @param lines New logical lines.
     * @param filename Associated filename.
     * @param encoding File encoding metadata.
     * @param line_ending File line-ending convention.
     * @param final_newline Whether the file ended with a line ending.
     *
     * @post The last line is current, or 0 for an empty file.
     * @post modified() is false.
     */
    void load_file(std::vector<std::string> lines,
                   std::string filename,
                   TextEncoding encoding,
                   LineEnding line_ending,
                   bool final_newline);

    /**
     * @brief Updates file association metadata without replacing content.
     *
     * @note This method deliberately does not change modified().
     */
    void associate_file(std::string filename,
                        TextEncoding encoding,
                        LineEnding line_ending,
                        bool final_newline);

    /** @brief Marks current content clean without changing it. */
    void mark_clean() noexcept;

    /** @brief Marks current content modified without changing it. */
    void mark_modified() noexcept;

private:
    /**
     * @brief Enforces a one-based existing-line precondition.
     * @throws std::out_of_range for 0 or number > line_count().
     */
    void require_existing_line(LineNumber number) const;

    std::string name_;
    std::vector<std::string> lines_;
    LineNumber current_line_{0};
    bool modified_{false};
    std::optional<std::string> associated_file_;
    TextEncoding encoding_{TextEncoding::Unknown};
    LineEnding line_ending_{LineEnding::Lf};
    bool final_newline_{true};
};

} // namespace fred
