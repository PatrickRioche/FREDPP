#pragma once

#include <cstddef>

namespace fredpp {

/**
 * @brief Logical key recognized by the help pager.
 */
enum class PagerKey {
    PageUp,
    PageDown,
    Quit,
    Other,
};

/**
 * @brief Clears an interactive terminal and moves the cursor to the home
 * position.
 *
 * Windows first uses Console API screen-buffer operations. If that cannot be
 * used, and on non-Windows builds, the implementation falls back to ANSI
 * `ESC[2J ESC[H`.
 *
 * @par Side effects
 * Writes/updates the process console.
 *
 * @note Shared by the `:cls` development command and HelpPager.
 */
void clear_terminal();

/**
 * @brief Tests whether standard input is connected to an interactive terminal.
 *
 * @return `_isatty(_fileno(stdin)) != 0` on Windows or
 *         `isatty(STDIN_FILENO) != 0` on POSIX.
 */
[[nodiscard]] bool stdin_is_terminal() noexcept;

/**
 * @brief Returns the currently visible terminal height.
 *
 * Windows queries the console window; POSIX uses `TIOCGWINSZ` on stdout.
 *
 * @return Positive row count when detectable, otherwise the implementation
 *         fallback of 25 rows.
 */
[[nodiscard]] std::size_t terminal_rows() noexcept;

/**
 * @brief Reads one logical key for the interactive help pager without requiring
 * Enter.
 *
 * Windows uses `_getch()`. POSIX temporarily disables canonical mode and echo
 * through an RAII termios guard, restoring terminal state on return.
 *
 * Recognized controls:
 * - PageUp;
 * - PageDown;
 * - `Q`/`q`;
 * - Escape as quit on Windows, or a lone Escape on POSIX.
 *
 * @return PagerKey classification. Input/read/setup failures are treated as
 *         Quit on the relevant paths rather than propagated as exceptions.
 */
[[nodiscard]] PagerKey read_pager_key();

} // namespace fredpp
