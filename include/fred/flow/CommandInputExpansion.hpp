#pragma once

#include "fred/flow/FlowEngine.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace fred {

struct ExpandedCommandInput {
    std::string text;
    std::vector<InputCharacter> characters;
};

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

inline std::string expand_command_input(
    std::string_view source,
    const BufferManager& buffers) {
    return expand_command_input_with_metadata(
        source, buffers).text;
}

inline std::string expand_command_input(
    std::string_view source,
    const BufferManager& buffers,
    std::size_t maximum_depth) {
    return expand_command_input_with_metadata(
        source, buffers, maximum_depth).text;
}

} // namespace fred
