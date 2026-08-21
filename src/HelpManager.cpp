/**
 * @file HelpManager.cpp
 * @brief Embedded-help lookup and controlled Markdown-to-terminal rendering.
 *
 * This implementation deliberately supports the subset of Markdown used by FREDPP help pages. It is presentation code, not a general CommonMark renderer. Public HelpManager contracts are documented in HelpManager.h.
 *
 * @note FREDPP_LOT8_CPP_DOCUMENTATION
 */
#include "HelpManager.h"
#include "EmbeddedHelp.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fredpp {
namespace {

constexpr std::string_view kMajorRule =
    "============================================================";
constexpr std::string_view kMinorRule =
    "------------------------------------------------------------";

/**
 * @brief Returns a non-owning view with surrounding ASCII whitespace removed.
 *
 * The returned view always refers to the caller-provided storage.
 */
std::string_view trim_view(std::string_view text) {
    const auto first = text.find_first_not_of(" \t\r\n");
    if (first == std::string_view::npos) {
        return {};
    }
    const auto last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

/** @brief Returns an owning trimmed copy used by renderer helpers. */
std::string trim_copy(std::string_view text) {
    return std::string(trim_view(text));
}

/**
 * @brief Computes the renderer's approximate terminal width.
 *
 * UTF-8 continuation bytes are ignored, so the result approximates Unicode
 * code-point count. It is intentionally not wcwidth/grapheme aware.
 */
std::size_t display_width(std::string_view text) {
    std::size_t width = 0;
    for (const unsigned char ch : text) {
        if ((ch & 0xC0U) != 0x80U) {
            ++width;
        }
    }
    return width;
}


/**
 * @brief Uppercases the French-oriented heading subset used by embedded help.
 *
 * Common accented lowercase sequences are replaced explicitly before ASCII
 * uppercasing. This is not a general Unicode case-mapping implementation.
 */
std::string uppercase_heading(std::string text) {
    static constexpr std::pair<std::string_view, std::string_view> replacements[] = {
        {"à", "À"}, {"â", "Â"}, {"ä", "Ä"}, {"ç", "Ç"},
        {"é", "É"}, {"è", "È"}, {"ê", "Ê"}, {"ë", "Ë"},
        {"î", "Î"}, {"ï", "Ï"}, {"ô", "Ô"}, {"ö", "Ö"},
        {"ù", "Ù"}, {"û", "Û"}, {"ü", "Ü"}
    };

    for (const auto& [lower, upper] : replacements) {
        std::size_t position = 0;
        while ((position = text.find(lower, position)) != std::string::npos) {
            text.replace(position, lower.size(), upper);
            position += upper.size();
        }
    }

    for (char& ch : text) {
        const auto byte = static_cast<unsigned char>(ch);
        if (byte < 0x80U) {
            ch = static_cast<char>(std::toupper(byte));
        }
    }
    return text;
}

/**
 * @brief Collapses repeated blank output lines and guarantees one final newline.
 */
std::string collapse_blank_lines(std::string text) {
    std::istringstream input(std::move(text));
    std::ostringstream output;
    std::string line;
    bool previous_blank = false;

    while (std::getline(input, line)) {
        const bool blank = trim_view(line).empty();
        if (blank && previous_blank) {
            continue;
        }
        output << line << '\n';
        previous_blank = blank;
    }

    auto result = output.str();
    while (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    result.push_back('\n');
    return result;
}

/**
 * @brief Removes the limited inline Markdown syntax supported by terminal help.
 *
 * Link targets are discarded while labels are preserved; backticks and simple
 * bold markers are removed. Nested/general Markdown parsing is out of scope.
 */
std::string strip_inline_markdown(std::string_view source) {
    std::string result;
    result.reserve(source.size());

    for (std::size_t index = 0; index < source.size();) {
        if (source[index] == '[') {
            const auto close_label = source.find(']', index + 1);
            if (close_label != std::string_view::npos &&
                close_label + 1 < source.size() &&
                source[close_label + 1] == '(') {
                const auto close_target = source.find(')', close_label + 2);
                if (close_target != std::string_view::npos) {
                    result.append(source.substr(index + 1, close_label - index - 1));
                    index = close_target + 1;
                    continue;
                }
            }
        }

        if (source[index] == '`') {
            ++index;
            continue;
        }
        if (index + 1 < source.size() &&
            ((source[index] == '*' && source[index + 1] == '*') ||
             (source[index] == '_' && source[index + 1] == '_'))) {
            index += 2;
            continue;
        }

        result.push_back(source[index]);
        ++index;
    }

    return result;
}

/** @return true for the pipe-delimited table-row shape used by help pages. */
bool is_table_row(std::string_view line) {
    const auto trimmed = trim_view(line);
    return trimmed.size() >= 2 && trimmed.front() == '|' && trimmed.back() == '|';
}

/**
 * @brief Splits one supported Markdown table row and strips inline formatting.
 */
std::vector<std::string> parse_table_row(std::string_view line) {
    const auto trimmed = trim_view(line);
    std::vector<std::string> cells;
    std::size_t begin = 1;

    while (begin < trimmed.size()) {
        const auto separator = trimmed.find('|', begin);
        if (separator == std::string_view::npos) {
            break;
        }
        cells.push_back(strip_inline_markdown(
            trim_view(trimmed.substr(begin, separator - begin))));
        begin = separator + 1;
    }

    return cells;
}

/**
 * @brief Recognizes the Markdown separator row made of dashes/colons.
 */
bool is_table_separator(const std::vector<std::string>& cells) {
    if (cells.empty()) {
        return false;
    }
    for (const auto& cell : cells) {
        bool has_dash = false;
        for (const unsigned char ch : cell) {
            if (ch == '-') {
                has_dash = true;
            } else if (ch != ':' && !std::isspace(ch)) {
                return false;
            }
        }
        if (!has_dash) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Renders parsed table rows as a fixed-width plain-text table.
 *
 * Width calculations inherit display_width()'s approximate Unicode behavior.
 */
void append_table(std::ostringstream& output,
                  const std::vector<std::vector<std::string>>& rows) {
    if (rows.empty()) {
        return;
    }

    std::size_t column_count = 0;
    for (const auto& row : rows) {
        column_count = std::max(column_count, row.size());
    }

    std::vector<std::size_t> widths(column_count, 0);
    for (const auto& row : rows) {
        for (std::size_t column = 0; column < row.size(); ++column) {
            widths[column] = std::max(widths[column], display_width(row[column]));
        }
    }

    const auto append_row = [&](const std::vector<std::string>& row) {
        output << "    ";
        for (std::size_t column = 0; column < column_count; ++column) {
            const std::string_view cell = column < row.size()
                                              ? std::string_view(row[column])
                                              : std::string_view{};
            output << cell;
            if (column + 1 < column_count) {
                const auto padding = widths[column] - display_width(cell) + 3;
                output << std::string(padding, ' ');
            }
        }
        output << '\n';
    };

    append_row(rows.front());
    output << "    ";
    for (std::size_t column = 0; column < column_count; ++column) {
        output << std::string(widths[column], '-');
        if (column + 1 < column_count) {
            output << "   ";
        }
    }
    output << '\n';

    for (std::size_t row = 1; row < rows.size(); ++row) {
        append_row(rows[row]);
    }
    output << '\n';
}

/**
 * @brief Renders a normalized heading with the FREDPP major/minor rule style.
 */
void append_heading(std::ostringstream& output,
                    std::string_view heading,
                    int level) {
    const auto clean = uppercase_heading(strip_inline_markdown(trim_view(heading)));
    if (level == 1) {
        output << kMajorRule << '\n' << clean << '\n' << kMajorRule << "\n\n";
    } else {
        output << '\n' << clean << '\n' << kMinorRule << "\n\n";
    }
}

} // namespace

std::string HelpManager::normalize_topic(std::string_view topic) {
    const auto first = topic.find_first_not_of(" \t\r\n");
    const auto last = topic.find_last_not_of(" \t\r\n");

    if (first == std::string_view::npos) {
        return "index";
    }

    std::string normalized(topic.substr(first, last - first + 1));
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
        [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

    if (normalized == "h" || normalized == "help") {
        return "index";
    }
    return normalized;
}

bool HelpManager::exists(std::string_view topic) const {
    const auto normalized = normalize_topic(topic);
    return embedded_help::find(normalized) != nullptr;
}

std::string HelpManager::load(std::string_view topic) const {
    const auto normalized = normalize_topic(topic);
    if (const auto* text = embedded_help::find(normalized)) {
        return std::string(*text);
    }
    throw std::runtime_error("Aucune rubrique d'aide : " + normalized);
}

std::string HelpManager::load_for_terminal(std::string_view topic) const {
    return render_markdown_for_terminal(load(topic));
}

/**
 * @brief Converts the supported FREDPP-help Markdown subset to terminal text.
 *
 * Supported structures include headings, fenced code, simple pipe tables,
 * quotes, bullets and a restricted set of inline markers. Unknown/general
 * Markdown constructs are intentionally not interpreted as CommonMark.
 */
std::string HelpManager::render_markdown_for_terminal(std::string_view markdown) {
    std::vector<std::string> lines;
    std::istringstream input{std::string(markdown)};
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(std::move(line));
    }

    std::ostringstream output;
    bool in_code_block = false;
    std::string code_language;

    for (std::size_t index = 0; index < lines.size();) {
        const auto trimmed = trim_view(lines[index]);

        if (trimmed.starts_with("```")) {
            if (!in_code_block) {
                code_language = trim_copy(trimmed.substr(3));
                std::transform(code_language.begin(), code_language.end(),
                               code_language.begin(),
                               [](unsigned char ch) {
                                   return static_cast<char>(std::toupper(ch));
                               });
                output << "    [" << (code_language.empty() ? "EXEMPLE" : code_language)
                       << "]\n    " << kMinorRule << '\n';
                in_code_block = true;
            } else {
                output << "    " << kMinorRule << "\n\n";
                in_code_block = false;
            }
            ++index;
            continue;
        }

        if (in_code_block) {
            output << "    " << lines[index] << '\n';
            ++index;
            continue;
        }

        if (is_table_row(lines[index]) && index + 1 < lines.size() &&
            is_table_row(lines[index + 1])) {
            const auto separator = parse_table_row(lines[index + 1]);
            if (is_table_separator(separator)) {
                std::vector<std::vector<std::string>> rows;
                rows.push_back(parse_table_row(lines[index]));
                index += 2;
                while (index < lines.size() && is_table_row(lines[index])) {
                    rows.push_back(parse_table_row(lines[index]));
                    ++index;
                }
                append_table(output, rows);
                continue;
            }
        }

        if (trimmed.starts_with("### ")) {
            append_heading(output, trimmed.substr(4), 3);
        } else if (trimmed.starts_with("## ")) {
            append_heading(output, trimmed.substr(3), 2);
        } else if (trimmed.starts_with("# ")) {
            append_heading(output, trimmed.substr(2), 1);
        } else if (trimmed.starts_with(">")) {
            output << "  " << strip_inline_markdown(trim_view(trimmed.substr(1)))
                   << "\n\n";
        } else if (trimmed.starts_with("- ")) {
            output << "  - " << strip_inline_markdown(trimmed.substr(2)) << '\n';
        } else if (trimmed.empty()) {
            output << '\n';
        } else {
            output << strip_inline_markdown(lines[index]) << '\n';
        }
        ++index;
    }

    return collapse_blank_lines(output.str());
}

std::vector<std::string> HelpManager::topics() const {
    auto result = embedded_help::topics();
    std::sort(result.begin(), result.end());
    return result;
}

} // namespace fredpp
