#pragma once

#include "fred/flow/FlowEngine.hpp"

#include <cctype>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>

namespace fred {

inline bool is_global_command_line(std::string_view source) noexcept {
    std::size_t position = 0;

    while (position < source.size() &&
           std::isspace(
               static_cast<unsigned char>(source[position])) != 0) {
        ++position;
    }

    // Adresses simples/ranges reconnues devant G.
    while (position < source.size()) {
        const char ch = source[position];
        if (std::isdigit(static_cast<unsigned char>(ch)) != 0 ||
            ch == '.' || ch == '$' || ch == ',' ||
            std::isspace(static_cast<unsigned char>(ch)) != 0) {
            ++position;
            continue;
        }
        break;
    }

    return position < source.size() &&
           (source[position] == 'G' || source[position] == 'g');
}

inline std::string expand_global_s_directives_with_flow(
    std::string_view source,
    FlowEngine& flow) {
    if (!is_global_command_line(source) ||
        (source.find("\\S(") == std::string_view::npos &&
         source.find("\\s(") == std::string_view::npos)) {
        return std::string(source);
    }

    std::string expanded;
    expanded.reserve(source.size());

    std::size_t position = 0;
    while (position < source.size()) {
        const bool is_s =
            source[position] == '\\' &&
            position + 2 < source.size() &&
            (source[position + 1] == 'S' ||
             source[position + 1] == 's') &&
            source[position + 2] == '(';

        if (!is_s) {
            expanded.push_back(source[position++]);
            continue;
        }

        const auto closing = source.find(')', position + 3);
        if (closing == std::string_view::npos) {
            throw std::runtime_error(
                "unterminated \\S(buffer) in G command");
        }

        const auto directive =
            source.substr(position, closing - position + 1);

        expanded += flow.expand_input(directive);
        position = closing + 1;
    }

    return expanded;
}

inline std::string expand_global_s_directives(
    std::string_view source,
    const BufferManager& buffers) {
    FlowEngine flow(buffers);
    return expand_global_s_directives_with_flow(source, flow);
}

inline std::string expand_global_s_directives(
    std::string_view source,
    const BufferManager& buffers,
    std::size_t maximum_depth) {
    FlowEngine flow(buffers, maximum_depth);
    return expand_global_s_directives_with_flow(source, flow);
}

} // namespace fred
