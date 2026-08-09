#include "fred/core/BufferManager.hpp"
#include "fred/core/Limits.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace fred {

BufferManager::BufferManager() {
    create_or_select("0");
}

void BufferManager::touch(std::string_view name) {
    const std::string value(name);
    usage_order_.erase(
        std::remove(usage_order_.begin(), usage_order_.end(), value),
        usage_order_.end());
    usage_order_.insert(usage_order_.begin(), value);
}

Buffer& BufferManager::create_or_select(std::string name) {
    if (name.empty()) {
        throw std::invalid_argument("buffer name must not be empty");
    }
    if (name.size() > limits::max_buffer_name_length) {
        throw std::invalid_argument(
            "buffer name exceeds historical limit of " +
            std::to_string(limits::max_buffer_name_length) + " characters");
    }

    std::string previous_name;
    bool remove_previous = false;
    if (current_ != nullptr && current_->name() != name &&
        current_->name() != "0" && current_->empty() &&
        !current_->modified() && !current_->has_associated_file()) {
        previous_name = current_->name();
        remove_previous = true;
    }

    auto [it, inserted] = buffers_.try_emplace(name, nullptr);
    if (inserted) {
        it->second = std::make_unique<Buffer>(name);
    }
    current_ = it->second.get();
    touch(name);

    // FR-0006: remove only a transient empty buffer with no file association
    // and no unsaved state when it is left.
    if (remove_previous) {
        buffers_.erase(previous_name);
        usage_order_.erase(
            std::remove(usage_order_.begin(), usage_order_.end(), previous_name),
            usage_order_.end());
    }

    return *current_;
}

Buffer& BufferManager::get_or_create(std::string name) {
    if (name.empty()) {
        throw std::invalid_argument("buffer name must not be empty");
    }
    if (name.size() > limits::max_buffer_name_length) {
        throw std::invalid_argument(
            "buffer name exceeds historical limit of " +
            std::to_string(limits::max_buffer_name_length) + " characters");
    }

    auto [it, inserted] = buffers_.try_emplace(name, nullptr);
    if (inserted) {
        it->second = std::make_unique<Buffer>(name);
    }
    return *it->second;
}

Buffer& BufferManager::select(std::string_view name) {
    auto it = buffers_.find(std::string(name));
    if (it == buffers_.end()) {
        throw std::out_of_range("unknown buffer");
    }
    current_ = it->second.get();
    touch(name);
    return *current_;
}

const Buffer& BufferManager::get(std::string_view name) const {
    auto it = buffers_.find(std::string(name));
    if (it == buffers_.end()) {
        throw std::out_of_range("unknown buffer: " + std::string(name));
    }
    return *it->second;
}

bool BufferManager::contains(std::string_view name) const noexcept {
    return buffers_.contains(std::string(name));
}

void BufferManager::erase(std::string_view name) {
    auto it = buffers_.find(std::string(name));
    if (it == buffers_.end()) {
        throw std::out_of_range("unknown buffer");
    }

    const bool deleting_current = current_ == it->second.get();
    const std::string erased_name = it->first;
    buffers_.erase(it);
    usage_order_.erase(
        std::remove(usage_order_.begin(), usage_order_.end(), erased_name),
        usage_order_.end());

    if (buffers_.empty()) {
        current_ = nullptr;
        create_or_select("0");
    } else if (deleting_current) {
        current_ = buffers_.begin()->second.get();
    }
}

Buffer& BufferManager::current() {
    if (current_ == nullptr) {
        throw std::logic_error("no current buffer");
    }
    return *current_;
}

const Buffer& BufferManager::current() const {
    if (current_ == nullptr) {
        throw std::logic_error("no current buffer");
    }
    return *current_;
}

std::vector<std::string> BufferManager::names() const {
    std::vector<std::string> result;
    result.reserve(buffers_.size());
    for (const auto& [name, value] : buffers_) {
        (void)value;
        result.push_back(name);
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<std::string> BufferManager::recent_names() const {
    return usage_order_;
}

std::vector<std::string> BufferManager::modified_names() const {
    std::vector<std::string> result;
    for (const auto& [name, buffer] : buffers_) {
        if (buffer->modified()) {
            result.push_back(name);
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

} // namespace fred
