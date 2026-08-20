#pragma once

#include "fred/flow/FlowEngine.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace fred {

/**
 * @brief Expanded command text paired with per-character flow metadata.
 *
 * `text[i]` and `characters[i].value` represent the same byte. The character
 * vector additionally preserves flow level and CharacterInterpretation for the
 * FlowCharacterStream -> Lexer pipeline.
 */
struct ExpandedCommandInput {
    std::string text;
    std::vector<InputCharacter> characters;
};

/**
 * @brief Wraps raw command input without performing flow expansion.
 *
 * @param source Raw source.
 * @return Equivalent text plus level-0/Normal metadata for every byte.
 *
 * Used when a caller intentionally bypasses command flow expansion while still
 * needing a FlowCharacterStream-compatible representation.
 */
inline ExpandedCommandInput make_command_input(
    std::string_view source) {
    ExpandedCommandInput result;
    result.text = std::string(source);
    result.characters.reserve(source.size());

    for (const char value : source) {
        result.characters.push_back(InputCharacter{
            value, 0, CharacterInterpretation::Normal});
    }

    return result;
}

/**
 * @brief Central command-input expansion entry point preserving metadata.
 *
 * @param source Raw command source before Lexer/CommandParser.
 * @param buffers Buffer namespace used by `\S`/`\L`.
 * @return Expanded text and matching InputCharacter metadata.
 *
 * @par Architectural requirement
 * This helper is the preferred boundary for FRED command-input substitution.
 * Commands should consume the already-expanded result; they should not each
 * implement their own flow-directive expansion.
 */
inline ExpandedCommandInput expand_command_input_with_metadata(
    std::string_view source,
    const BufferManager& buffers) {
    FlowEngine flow(buffers);

    ExpandedCommandInput result;
    result.characters =
        flow.expand_command_input_characters(source);
    result.text.reserve(result.characters.size());

    for (const auto& character : result.characters) {
        result.text.push_back(character.value);
    }

    return result;
}

/**
 * @brief Metadata-preserving expansion with explicit depth limit.
 *
 * @param maximum_depth Expansion nesting protection passed to FlowEngine.
 */
inline ExpandedCommandInput expand_command_input_with_metadata(
    std::string_view source,
    const BufferManager& buffers,
    std::size_t maximum_depth) {
    FlowEngine flow(buffers, maximum_depth);

    ExpandedCommandInput result;
    result.characters =
        flow.expand_command_input_characters(source);
    result.text.reserve(result.characters.size());

    for (const auto& character : result.characters) {
        result.text.push_back(character.value);
    }

    return result;
}

/**
 * @brief Convenience command-input expansion returning only text.
 *
 * @warning CharacterInterpretation metadata is discarded. Use
 * expand_command_input_with_metadata() when the expanded input will be lexed.
 */
inline std::string expand_command_input(
    std::string_view source,
    const BufferManager& buffers) {
    return expand_command_input_with_metadata(
        source, buffers).text;
}

/**
 * @brief Text-only convenience overload with explicit depth limit.
 */
inline std::string expand_command_input(
    std::string_view source,
    const BufferManager& buffers,
    std::size_t maximum_depth) {
    return expand_command_input_with_metadata(
        source, buffers, maximum_depth).text;
}

} // namespace fred
