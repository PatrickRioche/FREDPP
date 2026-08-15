#include "Terminal.h"

#include <iostream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace fredpp {
namespace {

#ifdef _WIN32
bool clear_windows_console() noexcept {
    const HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
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
#endif

} // namespace

void clear_terminal() {
#ifdef _WIN32
    if (clear_windows_console()) {
        return;
    }
#endif

    // Repli portable pour terminaux ANSI / pseudo-terminaux.
    std::cout << "\x1b[2J\x1b[H" << std::flush;
}

} // namespace fredpp
