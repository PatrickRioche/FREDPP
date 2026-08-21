/**
 * @file Terminal.cpp
 * @brief Cross-platform terminal primitives used by the interactive help pager.
 *
 * Windows uses native console APIs where appropriate; POSIX uses termios, ioctl/select and ANSI sequences. Terminal state restoration follows RAII.
 *
 * @note FREDPP_LOT8_CPP_DOCUMENTATION
 */
#include "Terminal.h"

#include <cstdio>
#include <iostream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <conio.h>
#include <io.h>
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

namespace fredpp {
namespace {

constexpr std::size_t kDefaultTerminalRows = 25;

#ifdef _WIN32

/** @return Current Windows standard-output console handle. */
HANDLE console_output_handle() noexcept {
    return GetStdHandle(STD_OUTPUT_HANDLE);
}

/**
 * @brief Clears a Windows console through native screen-buffer APIs.
 * @return true when all native clear/cursor operations succeed.
 */
bool clear_windows_console() noexcept {
    const HANDLE output = console_output_handle();
    if (output == INVALID_HANDLE_VALUE || output == nullptr) {
        return false;
    }

    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(output, &info) == 0) {
        return false;
    }

    if (info.dwSize.X <= 0 || info.dwSize.Y <= 0) {
        return false;
    }

    const DWORD cell_count =
        static_cast<DWORD>(info.dwSize.X) *
        static_cast<DWORD>(info.dwSize.Y);
    const COORD home{0, 0};

    DWORD written = 0;
    if (FillConsoleOutputCharacterW(
            output, L' ', cell_count, home, &written) == 0) {
        return false;
    }

    if (FillConsoleOutputAttribute(
            output, info.wAttributes, cell_count, home, &written) == 0) {
        return false;
    }

    if (SetConsoleCursorPosition(output, home) == 0) {
        return false;
    }

    return true;
}

#else

/**
 * @brief RAII guard that temporarily places POSIX stdin in noncanonical mode.
 *
 * Construction records the original terminal attributes and disables ICANON
 * and ECHO. The destructor restores the saved state when activation succeeded.
 */
class RawTerminalMode {
public:
    RawTerminalMode() noexcept {
        if (tcgetattr(STDIN_FILENO, &saved_) != 0) {
            return;
        }

        termios raw = saved_;
        raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;

        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0) {
            active_ = true;
        }
    }

    ~RawTerminalMode() {
        if (active_) {
            (void)tcsetattr(STDIN_FILENO, TCSANOW, &saved_);
        }
    }

    [[nodiscard]] bool active() const noexcept {
        return active_;
    }

private:
    termios saved_{};
    bool active_{false};
};

/**
 * @brief Reads one POSIX stdin byte with a select()-based timeout.
 * @return true only when exactly one byte was read before the timeout.
 */
bool read_byte_with_timeout(unsigned char& value, long microseconds) noexcept {
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(STDIN_FILENO, &read_set);

    timeval timeout{};
    timeout.tv_sec = microseconds / 1000000L;
    timeout.tv_usec = microseconds % 1000000L;

    const int ready =
        select(STDIN_FILENO + 1, &read_set, nullptr, nullptr, &timeout);
    if (ready <= 0) {
        return false;
    }

    return read(STDIN_FILENO, &value, 1) == 1;
}

#endif

} // namespace

/**
 * @brief Clears the current terminal using the best platform-specific method.
 *
 * Windows first attempts the native console path and falls back to ANSI.
 */
void clear_terminal() {
#ifdef _WIN32
    if (clear_windows_console()) {
        return;
    }
#endif

    // Portable fallback for ANSI terminals and pseudo-terminals.
    std::cout << "\x1b[2J\x1b[H" << std::flush;
}

bool stdin_is_terminal() noexcept {
#ifdef _WIN32
    return _isatty(_fileno(stdin)) != 0;
#else
    return isatty(STDIN_FILENO) != 0;
#endif
}

std::size_t terminal_rows() noexcept {
#ifdef _WIN32
    const HANDLE output = console_output_handle();
    if (output != INVALID_HANDLE_VALUE && output != nullptr) {
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (GetConsoleScreenBufferInfo(output, &info) != 0) {
            const int rows =
                static_cast<int>(info.srWindow.Bottom) -
                static_cast<int>(info.srWindow.Top) + 1;
            if (rows > 0) {
                return static_cast<std::size_t>(rows);
            }
        }
    }
#else
    winsize size{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0 && size.ws_row > 0) {
        return static_cast<std::size_t>(size.ws_row);
    }
#endif

    return kDefaultTerminalRows;
}

/**
 * @brief Maps platform-specific keyboard sequences to the pager key abstraction.
 *
 * A failed terminal/raw-mode read is treated as Quit so the pager cannot leave
 * the user trapped in an unreadable interactive state.
 */
PagerKey read_pager_key() {
#ifdef _WIN32
    const int first = _getch();

    if (first == 'q' || first == 'Q' || first == 27) {
        return PagerKey::Quit;
    }

    if (first == 0 || first == 224) {
        const int second = _getch();
        if (second == 73) {
            return PagerKey::PageUp;
        }
        if (second == 81) {
            return PagerKey::PageDown;
        }
    }

    return PagerKey::Other;
#else
    RawTerminalMode raw_mode;
    if (!raw_mode.active()) {
        return PagerKey::Quit;
    }

    unsigned char first = 0;
    if (read(STDIN_FILENO, &first, 1) != 1) {
        return PagerKey::Quit;
    }

    if (first == 'q' || first == 'Q') {
        return PagerKey::Quit;
    }

    if (first != 27) {
        return PagerKey::Other;
    }

    // Common PageUp/PageDown escape sequences: ESC [ 5 ~ / ESC [ 6 ~.
    unsigned char second = 0;
    if (!read_byte_with_timeout(second, 100000L)) {
        return PagerKey::Quit;
    }
    if (second != '[') {
        return PagerKey::Other;
    }

    unsigned char third = 0;
    unsigned char fourth = 0;
    if (!read_byte_with_timeout(third, 100000L) ||
        !read_byte_with_timeout(fourth, 100000L)) {
        return PagerKey::Other;
    }

    if (fourth != '~') {
        return PagerKey::Other;
    }
    if (third == '5') {
        return PagerKey::PageUp;
    }
    if (third == '6') {
        return PagerKey::PageDown;
    }

    return PagerKey::Other;
#endif
}

} // namespace fredpp
