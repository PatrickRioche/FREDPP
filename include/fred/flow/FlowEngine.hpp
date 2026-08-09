#pragma once

#include "fred/core/BufferManager.hpp"
#include "fred/flow/InputCharacter.hpp"
#include "fred/flow/InputStack.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

namespace fred {

class FlowEngine {
public:
    explicit FlowEngine(const BufferManager& buffers,
                        std::size_t maximum_depth = 256);

    [[nodiscard]] std::string expand_buffer(std::string_view buffer_name);
    [[nodiscard]] std::string expand_input(std::string_view source);

private:
    [[nodiscard]] std::string expand_current_input();
    [[nodiscard]] std::optional<InputCharacter> read_raw();
    [[nodiscard]] std::string parse_buffer_name(std::size_t directive_level,
                                                char directive);
    void push_buffer(std::string_view buffer_name,
                     std::size_t level,
                     bool emit_newlines = true,
                     bool literal = false);

    const BufferManager* buffers_;
    InputStack input_;
};

} // namespace fred
