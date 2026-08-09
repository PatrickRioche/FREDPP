#pragma once

#include "fred/core/BufferManager.hpp"
#include "fred/runtime/Output.hpp"

#include <cstddef>

namespace fred {

// Services and mutable editor state made available to command execution.
// The context does not own either dependency; their lifetime must exceed its own.
class ExecutionContext {
public:
    ExecutionContext(BufferManager& buffers, Output& output) noexcept;

    [[nodiscard]] BufferManager& buffers() noexcept;
    [[nodiscard]] const BufferManager& buffers() const noexcept;

    [[nodiscard]] Buffer& current_buffer();
    [[nodiscard]] const Buffer& current_buffer() const;

    [[nodiscard]] Output& output() noexcept;

    void set_counter(std::size_t value) noexcept;
    [[nodiscard]] std::size_t counter() const noexcept;

    void set_condition(bool value) noexcept;
    [[nodiscard]] bool condition() const noexcept;

    void set_input_parentheses_required(bool value) noexcept;
    [[nodiscard]] bool input_parentheses_required() const noexcept;
    void set_monitor_commands(bool value) noexcept;
    [[nodiscard]] bool monitor_commands() const noexcept;

    void request_exit(bool immediate) noexcept;
    [[nodiscard]] bool exit_requested() const noexcept;
    [[nodiscard]] bool immediate_exit_requested() const noexcept;

private:
    BufferManager* buffers_;
    Output* output_;
    std::size_t counter_{};
    bool condition_{};
    bool input_parentheses_required_{true};
    bool monitor_commands_{};
    bool exit_requested_{};
    bool immediate_exit_requested_{};
};

} // namespace fred
