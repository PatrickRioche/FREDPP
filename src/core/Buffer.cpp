#include "fred/core/Buffer.hpp"

#include <stdexcept>
#include <iterator>
#include <utility>

namespace fred {

Buffer::Buffer(std::string name) : name_(std::move(name)) {
    if (name_.empty()) {
        throw std::invalid_argument("buffer name must not be empty");
    }
}

const std::string& Buffer::name() const noexcept { return name_; }
bool Buffer::empty() const noexcept { return lines_.empty(); }
std::size_t Buffer::line_count() const noexcept { return lines_.size(); }
Buffer::LineNumber Buffer::current_line() const noexcept { return current_line_; }

const std::string& Buffer::line(LineNumber number) const {
    require_existing_line(number);
    return lines_.at(number - 1);
}

const std::vector<std::string>& Buffer::lines() const noexcept {
    return lines_;
}

void Buffer::append(std::string text) {
    lines_.push_back(std::move(text));
    current_line_ = lines_.size();
}

void Buffer::insert_before(LineNumber number, std::string text) {
    if (number == lines_.size() + 1) {
        append(std::move(text));
        return;
    }
    require_existing_line(number);
    lines_.insert(lines_.begin() + static_cast<std::ptrdiff_t>(number - 1),
                  std::move(text));
    current_line_ = number;
}

void Buffer::insert_after(LineNumber number, std::vector<std::string> text) {
    if (number > lines_.size()) {
        throw std::out_of_range("line number out of range");
    }

    if (text.empty()) {
        current_line_ = number;
        return;
    }

    const auto position = lines_.begin() + static_cast<std::ptrdiff_t>(number);
    lines_.insert(position,
                  std::make_move_iterator(text.begin()),
                  std::make_move_iterator(text.end()));
    current_line_ = number + text.size();
}

void Buffer::replace(LineNumber number, std::string text) {
    require_existing_line(number);
    lines_.at(number - 1) = std::move(text);
    current_line_ = number;
}

void Buffer::erase(LineNumber first, LineNumber last) {
    require_existing_line(first);
    require_existing_line(last);
    if (first > last) {
        throw std::invalid_argument("first line must not exceed last line");
    }

    lines_.erase(
        lines_.begin() + static_cast<std::ptrdiff_t>(first - 1),
        lines_.begin() + static_cast<std::ptrdiff_t>(last)
    );

    if (lines_.empty()) {
        current_line_ = 0;
    } else if (first <= lines_.size()) {
        current_line_ = first;
    } else {
        current_line_ = lines_.size();
    }
}

void Buffer::set_current_line(LineNumber number) {
    if (number > lines_.size()) {
        throw std::out_of_range("line number out of range");
    }
    current_line_ = number;
}

void Buffer::require_existing_line(LineNumber number) const {
    if (number == 0 || number > lines_.size()) {
        throw std::out_of_range("line number out of range");
    }
}

} // namespace fred
