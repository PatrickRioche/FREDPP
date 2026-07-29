#include "fred/core/BufferManager.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace fred {

BufferManager::BufferManager() {
    create_or_select("0");
}

Buffer& BufferManager::create_or_select(std::string name) {
    if (name.empty()) {
        throw std::invalid_argument("buffer name must not be empty");
    }

    auto [it, inserted] = buffers_.try_emplace(name, nullptr);
    if (inserted) {
        it->second = std::make_unique<Buffer>(std::move(name));
    }
    current_ = it->second.get();
    return *current_;
}

Buffer& BufferManager::select(std::string_view name) {
    auto it = buffers_.find(std::string(name));
    if (it == buffers_.end()) {
        throw std::out_of_range("unknown buffer");
    }
    current_ = it->second.get();
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
    buffers_.erase(it);

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

} // namespace fred
