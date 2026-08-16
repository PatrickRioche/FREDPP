#pragma once

#include "fred/flow/FlowEngine.hpp"

#include <cstddef>
#include <string>
#include <string_view>

namespace fred {

inline std::string expand_command_input(
    std::string_view source,
    const BufferManager& buffers) {
    FlowEngine flow(buffers);
    return flow.expand_command_input(source);
}

inline std::string expand_command_input(
    std::string_view source,
    const BufferManager& buffers,
    std::size_t maximum_depth) {
    FlowEngine flow(buffers, maximum_depth);
    return flow.expand_command_input(source);
}

} // namespace fred
