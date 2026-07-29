#pragma once

#include "fred/core/BufferManager.hpp"
#include "fred/runtime/Output.hpp"

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

private:
    BufferManager* buffers_;
    Output* output_;
};

} // namespace fred
