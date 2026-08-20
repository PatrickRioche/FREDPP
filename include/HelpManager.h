#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace fredpp {

/**
 * @brief Accesses the help pages embedded into the FREDPP binary at build time.
 *
 * HelpManager is a read-only façade over generated `embedded_help` data. Source
 * Markdown lives under `docs/fr/`; CMake reads selected pages and generates
 * `EmbeddedHelp.hpp/.cpp` in the build tree without modifying those Markdown
 * files.
 *
 * The class owns no persistent state and may be cheaply constructed wherever a
 * front end needs help lookup/rendering.
 */
class HelpManager {
public:
    /**
     * @brief Tests whether an embedded help topic exists.
     *
     * @param topic User-facing topic spelling. Leading/trailing whitespace and
     *        ASCII case are normalized; empty, `h` and `help` map to `index`.
     * @return true when the normalized topic exists in embedded_help.
     */
    [[nodiscard]] bool exists(std::string_view topic) const;

    /**
     * @brief Loads the raw embedded Markdown for one topic.
     *
     * @param topic Topic accepted by normalize_topic().
     * @return Copy of the embedded Markdown source.
     *
     * @throws std::runtime_error when the normalized topic does not exist.
     *
     * @note This preserves Markdown unchanged. Use load_for_terminal() when
     *       plain terminal rendering is desired.
     */
    [[nodiscard]] std::string load(std::string_view topic) const;

    /**
     * @brief Loads and renders one embedded Markdown page for a text terminal.
     *
     * @return Newly allocated plain-text representation suitable for
     *         HelpPager/console output.
     * @throws std::runtime_error when the topic does not exist.
     */
    [[nodiscard]] std::string load_for_terminal(
        std::string_view topic) const;

    /**
     * @return Lexicographically sorted list of every embedded help topic key.
     *
     * @note This includes internal/special topics that are intentionally not all
     *       shown in the public `?` index.
     */
    [[nodiscard]] std::vector<std::string> topics() const;

    /**
     * @brief Canonicalizes a user-supplied topic key.
     *
     * Rules:
     * - trim spaces/tabs/CR/LF from both ends;
     * - lowercase bytes using std::tolower;
     * - empty, `h` and `help` become `index`.
     *
     * @return Canonical lookup key.
     */
    [[nodiscard]] static std::string normalize_topic(
        std::string_view topic);

    /**
     * @brief Converts the supported Markdown subset to terminal-oriented text.
     *
     * Current rendering handles headings, fenced code, ordinary Markdown
     * tables, blockquotes, bullets, inline links/code/bold markers and blank
     * line compaction.
     *
     * @param markdown Markdown source.
     * @return Rendered plain text ending in exactly one newline for normal
     *         non-empty renderer output.
     *
     * @note This is a presentation renderer, not a complete Markdown parser.
     *       Markdown source files remain authoritative and unchanged.
     */
    [[nodiscard]] static std::string render_markdown_for_terminal(
        std::string_view markdown);
};

} // namespace fredpp
