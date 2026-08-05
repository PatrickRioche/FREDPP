#include "fred/runtime/ExecutionContext.hpp"

namespace fred {

ExecutionContext::ExecutionContext(BufferManager& buffers, Output& output) noexcept
    : buffers_(&buffers), output_(&output) {}

BufferManager& ExecutionContext::buffers() noexcept {
    return *buffers_;
}

const BufferManager& ExecutionContext::buffers() const noexcept {
    return *buffers_;
}

Buffer& ExecutionContext::current_buffer() {
    return buffers_->current();
}

const Buffer& ExecutionContext::current_buffer() const {
    return buffers_->current();
}

Output& ExecutionContext::output() noexcept {
    return *output_;
}

void ExecutionContext::set_counter(std::size_t value) noexcept {
    counter_ = value;
}

std::size_t ExecutionContext::counter() const noexcept {
    return counter_;
}

} // namespace fred
