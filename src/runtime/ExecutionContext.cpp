#include "fred/runtime/ExecutionContext.hpp"

#include <algorithm>
#include <cstdint>
#include <utility>

namespace fred {

ExecutionContext::ExecutionContext(
    BufferManager& buffers,
    Output& output) noexcept
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
    // Used by ZG to capture nested command output temporarily. Ownership of
    // neither sink changes; callers are responsible for restoring/keeping the
    // referenced Output objects alive.
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

    // Historical/procedure numeric code currently treats an absent register as
    // value zero. has_numeric_register() exists when absence matters.
    return found == numeric_registers_.end() ? 0 : found->second;
}

bool ExecutionContext::has_numeric_register(
    std::string_view name) const {
    return numeric_registers_.find(std::string(name)) !=
           numeric_registers_.end();
}

std::vector<std::string> ExecutionContext::numeric_register_names() const {
    std::vector<std::string> names;
    names.reserve(numeric_registers_.size());
    for (const auto& entry : numeric_registers_) {
        names.push_back(entry.first);
    }
    std::sort(names.begin(), names.end());
    return names;
}

void ExecutionContext::set_numeric_base(
    std::string_view name, unsigned base) {
    const std::string key(name);
    numeric_registers_.try_emplace(key, 0);
    numeric_formats_[key].base = base;
}

void ExecutionContext::set_numeric_minimum_digits(
    std::string_view name, std::size_t digits) {
    const std::string key(name);
    numeric_registers_.try_emplace(key, 0);
    numeric_formats_[key].minimum_digits = digits;
}

void ExecutionContext::set_numeric_fill_character(
    std::string_view name, char character) {
    const std::string key(name);
    numeric_registers_.try_emplace(key, 0);
    numeric_formats_[key].fill_character = character;
}

std::string ExecutionContext::format_numeric_register(
    std::string_view name) const {
    const auto value = numeric_register(name);
    const auto options = numeric_formats_.find(std::string(name));
    const NumericDisplayOptions defaults;
    const auto& display = options == numeric_formats_.end()
        ? defaults : options->second;

    // Convert through unsigned magnitude so INT64_MIN remains representable.
    std::uint64_t magnitude = value < 0
        ? static_cast<std::uint64_t>(-(value + 1)) + 1
        : static_cast<std::uint64_t>(value);
    std::string digits;
    constexpr std::string_view alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    do {
        digits.push_back(alphabet[magnitude % display.base]);
        magnitude /= display.base;
    } while (magnitude != 0);
    std::reverse(digits.begin(), digits.end());

    if (digits.size() < display.minimum_digits) {
        digits.insert(digits.begin(),
                      display.minimum_digits - digits.size(),
                      display.fill_character);
    }
    if (value < 0) {
        digits.insert(digits.begin(), '-');
    }
    return digits;
}

void ExecutionContext::set_condition(bool value) noexcept {
    condition_ = value;
}

bool ExecutionContext::condition() const noexcept {
    return condition_;
}

void ExecutionContext::set_input_parentheses_required(
    bool value) noexcept {
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
    // A later request overwrites the immediate flag while preserving the
    // monotonic "exit requested" state.
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
