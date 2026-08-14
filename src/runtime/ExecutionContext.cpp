#include "fred/runtime/ExecutionContext.hpp"

#include <utility>

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

Output& ExecutionContext::exchange_output(Output& output) noexcept {
    Output* previous = output_;
    output_ = &output;
    return *previous;
}

void ExecutionContext::set_counter(std::int64_t value) noexcept {
    counter_ = value;
}

std::int64_t ExecutionContext::counter() const noexcept {
    return counter_;
}

void ExecutionContext::set_numeric_register(
    std::string name,
    std::int64_t value) {
    numeric_registers_.insert_or_assign(std::move(name), value);
}

std::int64_t ExecutionContext::numeric_register(
    std::string_view name) const {
    const auto found = numeric_registers_.find(std::string(name));
    return found == numeric_registers_.end() ? 0 : found->second;
}

bool ExecutionContext::has_numeric_register(
    std::string_view name) const {
    return numeric_registers_.find(std::string(name)) !=
           numeric_registers_.end();
}


void ExecutionContext::set_condition(bool value) noexcept {
    condition_ = value;
}

bool ExecutionContext::condition() const noexcept {
    return condition_;
}

void ExecutionContext::set_input_parentheses_required(bool value) noexcept {
    input_parentheses_required_ = value;
}

bool ExecutionContext::input_parentheses_required() const noexcept {
    return input_parentheses_required_;
}

void ExecutionContext::set_monitor_commands(bool value) noexcept {
    monitor_commands_ = value;
}

bool ExecutionContext::monitor_commands() const noexcept {
    return monitor_commands_;
}

void ExecutionContext::request_exit(bool immediate) noexcept {
    exit_requested_ = true;
    immediate_exit_requested_ = immediate;
}

bool ExecutionContext::exit_requested() const noexcept {
    return exit_requested_;
}

bool ExecutionContext::immediate_exit_requested() const noexcept {
    return immediate_exit_requested_;
}

} // namespace fred
