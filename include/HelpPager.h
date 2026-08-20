#pragma once

#include <string_view>

namespace fredpp {

/**
 * @brief Displays rendered help with interactive paging when possible.
 *
 * If stdin is not attached to an interactive terminal, the complete text is
 * printed once and the function returns. In an interactive terminal it splits
 * text into pages based on terminal_rows(), reserves footer rows for controls,
 * and responds to PageUp/PageDown/Q through Terminal primitives.
 *
 * @param text Already-rendered help text. The view is borrowed only for the
 *        duration of the call.
 *
 * @par Side effects
 * Writes to std::cout, may clear the terminal repeatedly, and may block waiting
 * for pager key input.
 *
 * @note Paging belongs to the CLI/presentation layer, not Runtime Output.
 */
void show_paged_help(std::string_view text);

} // namespace fredpp
