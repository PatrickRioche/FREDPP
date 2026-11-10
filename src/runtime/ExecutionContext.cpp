#include "fred/runtime/ExecutionContext.hpp"

#include <cctype>
#include <algorithm>
#include <cstdint>
#include <stdexcept>
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

void ExecutionContext::set_quit_on_error(bool value) noexcept {
    quit_on_error_ = value;
}
bool ExecutionContext::quit_on_error() const noexcept { return quit_on_error_; }
void ExecutionContext::set_substitution_ampersand(bool value) noexcept {
    substitution_ampersand_ = value;
}
bool ExecutionContext::substitution_ampersand() const noexcept {
    return substitution_ampersand_;
}
void ExecutionContext::set_case_insensitive_search(bool value) noexcept {
    case_insensitive_search_ = value;
}
bool ExecutionContext::case_insensitive_search() const noexcept {
    return case_insensitive_search_;
}
void ExecutionContext::set_tab_stops(std::vector<std::size_t> stops) {
    tab_stops_ = std::move(stops);
}
const std::vector<std::size_t>& ExecutionContext::tab_stops() const noexcept {
    return tab_stops_;
}
void ExecutionContext::set_tabs_enabled(bool value) noexcept {
    tabs_enabled_ = value;
}
bool ExecutionContext::tabs_enabled() const noexcept { return tabs_enabled_; }

void ExecutionContext::set_append_prompt(std::string value) {
    append_prompt_ = std::move(value);
}
const std::string& ExecutionContext::append_prompt() const noexcept {
    return append_prompt_;
}
void ExecutionContext::set_command_prompt(std::string value) {
    command_prompt_ = std::move(value);
}
const std::string& ExecutionContext::command_prompt() const noexcept {
    return command_prompt_;
}
void ExecutionContext::set_return_message(std::string value) {
    return_message_ = std::move(value);
}
const std::string& ExecutionContext::return_message() const noexcept {
    return return_message_;
}
void ExecutionContext::set_lower_input_escape(std::optional<char> value) noexcept {
    lower_input_escape_ = value;
}
std::optional<char> ExecutionContext::lower_input_escape() const noexcept {
    return lower_input_escape_;
}
void ExecutionContext::set_upper_input_escape(std::optional<char> value) noexcept {
    upper_input_escape_ = value;
}
std::optional<char> ExecutionContext::upper_input_escape() const noexcept {
    return upper_input_escape_;
}
std::string ExecutionContext::transform_input_line(std::string_view line) const {
    std::string changed(line);
    if (!input_change_from_.empty()) {
        std::string substituted;
        substituted.reserve(changed.size());
        std::size_t position = 0;
        while (position < changed.size()) {
            const auto match = changed.find(input_change_from_, position);
            if (match == std::string::npos) {
                substituted.append(changed, position);
                break;
            }
            substituted.append(changed, position, match - position);
            substituted += input_change_to_;
            if (substituted.size() > 1048576) {
                throw std::runtime_error("OC input expansion exceeds 1 MiB");
            }
            position = match + input_change_from_.size();
        }
        changed = std::move(substituted);
    }
    std::string text;
    text.reserve(changed.size());
    for (std::size_t i = 0; i < changed.size(); ++i) {
        const char value = changed[i];
        const bool to_lower = lower_input_escape_ && value == *lower_input_escape_;
        const bool to_upper = upper_input_escape_ && value == *upper_input_escape_;
        if ((to_lower || to_upper) && i + 1 < changed.size()) {
            const unsigned char next = static_cast<unsigned char>(changed[++i]);
            text.push_back(next < 128 ? static_cast<char>(
                to_lower ? std::tolower(next) : std::toupper(next)) :
                static_cast<char>(next));
        } else {
            text.push_back(value);
        }
    }
    return text;
}
void ExecutionContext::set_input_change(std::string from, std::string to) {
    input_change_from_ = std::move(from);
    input_change_to_ = std::move(to);
}
const std::string& ExecutionContext::input_change_from() const noexcept {
    return input_change_from_;
}
const std::string& ExecutionContext::input_change_to() const noexcept {
    return input_change_to_;
}
void ExecutionContext::set_input_terminator_required(bool value) noexcept {
    input_terminator_required_ = value;
}
bool ExecutionContext::input_terminator_required() const noexcept {
    return input_terminator_required_;
}
void ExecutionContext::set_input_escape(char value) noexcept { input_escape_ = value; }
char ExecutionContext::input_escape() const noexcept { return input_escape_; }
void ExecutionContext::set_input_feed(bool value) noexcept { input_feed_ = value; }
bool ExecutionContext::input_feed() const noexcept { return input_feed_; }
void ExecutionContext::set_output_lower(bool value) noexcept { output_lower_ = value; }
bool ExecutionContext::output_lower() const noexcept { return output_lower_; }
void ExecutionContext::set_output_octal(bool value) noexcept { output_octal_ = value; }
bool ExecutionContext::output_octal() const noexcept { return output_octal_; }
void ExecutionContext::set_output_upper(bool value) noexcept { output_upper_ = value; }
bool ExecutionContext::output_upper() const noexcept { return output_upper_; }
void ExecutionContext::set_page_size(std::size_t value) noexcept { page_size_ = value; }
std::size_t ExecutionContext::page_size() const noexcept { return page_size_; }
void ExecutionContext::set_substitution_separator_required(bool value) noexcept {
    substitution_separator_required_ = value;
}
bool ExecutionContext::substitution_separator_required() const noexcept {
    return substitution_separator_required_;
}
std::string ExecutionContext::format_output_line(std::string_view line) const {
    std::string result;
    result.reserve(line.size());
    for (const unsigned char value : line) {
        if (output_octal_ && (value < 32 || value == 127)) {
            result += '\\';
            result += static_cast<char>('0' + ((value >> 6) & 7));
            result += static_cast<char>('0' + ((value >> 3) & 7));
            result += static_cast<char>('0' + (value & 7));
        } else if (output_lower_ && value >= 'a' && value <= 'z') {
            result += lower_input_escape_.value_or('%');
            result += static_cast<char>(std::toupper(value));
        } else if (output_upper_ && value >= 'A' && value <= 'Z') {
            result += upper_input_escape_.value_or('^');
            result += static_cast<char>(std::tolower(value));
        } else {
            result += static_cast<char>(value);
        }
    }
    return result;
}
void ExecutionContext::set_last_error(std::string message) {
    last_error_ = std::move(message);
}
const std::string& ExecutionContext::last_error() const noexcept {
    return last_error_;
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
