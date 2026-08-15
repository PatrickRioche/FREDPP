#include "HelpPager.h"

#include "Terminal.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace fredpp {
namespace {

constexpr std::size_t kFooterRows = 2;
constexpr std::size_t kMinimumContentRows = 1;

std::vector<std::string> split_lines(std::string_view text) {
    std::vector<std::string> lines;
    std::istringstream input{std::string(text)};
    std::string line;

    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(std::move(line));
    }

    if (lines.empty()) {
        lines.emplace_back();
    }

    return lines;
}

void print_non_interactive(std::string_view text) {
    std::cout << text;
    if (text.empty() || text.back() != '\n') {
        std::cout << '\n';
    }
}

} // namespace

void show_paged_help(std::string_view text) {
    if (!stdin_is_terminal()) {
        print_non_interactive(text);
        return;
    }

    const auto lines = split_lines(text);
    const std::size_t rows = terminal_rows();
    const std::size_t content_rows =
        rows > kFooterRows
            ? std::max(kMinimumContentRows, rows - kFooterRows)
            : kMinimumContentRows;

    const std::size_t page_count =
        std::max<std::size_t>(
            1, (lines.size() + content_rows - 1) / content_rows);

    std::size_t page = 0;

    for (;;) {
        clear_terminal();

        const std::size_t begin = page * content_rows;
        const std::size_t end =
            std::min(lines.size(), begin + content_rows);

        for (std::size_t index = begin; index < end; ++index) {
            std::cout << lines[index] << '\n';
        }

        const std::size_t printed = end - begin;
        for (std::size_t index = printed; index < content_rows; ++index) {
            std::cout << '\n';
        }

        std::cout
            << "------------------------------------------------------------\n"
            << "Page " << (page + 1) << '/' << page_count
            << "  [PgUp] précédent  [PgDn] suivant  [Q] quitter"
            << std::flush;

        switch (read_pager_key()) {
        case PagerKey::PageUp:
            if (page > 0) {
                --page;
            }
            break;

        case PagerKey::PageDown:
            if (page + 1 < page_count) {
                ++page;
            }
            break;

        case PagerKey::Quit:
            clear_terminal();
            return;

        case PagerKey::Other:
            break;
        }
    }
}

} // namespace fredpp
