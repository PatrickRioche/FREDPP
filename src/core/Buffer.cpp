#include "fred/core/Buffer.hpp"

#include <iterator>
#include <stdexcept>
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

bool Buffer::modified() const noexcept { return modified_; }
bool Buffer::has_associated_file() const noexcept {
    return associated_file_.has_value();
}
const std::optional<std::string>& Buffer::associated_file() const noexcept {
    return associated_file_;
}
TextEncoding Buffer::encoding() const noexcept { return encoding_; }
LineEnding Buffer::line_ending() const noexcept { return line_ending_; }
bool Buffer::final_newline() const noexcept { return final_newline_; }

void Buffer::append(std::string text) {
    lines_.push_back(std::move(text));
    current_line_ = lines_.size();
    mark_modified();
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
    mark_modified();
}

void Buffer::insert_after(LineNumber number, std::vector<std::string> text) {
    if (number > lines_.size()) {
        throw std::out_of_range("line number out of range");
    }

    if (text.empty()) {
        current_line_ = number;
        return;
    }

    const auto inserted_count = text.size();
    const auto position = lines_.begin() + static_cast<std::ptrdiff_t>(number);
    lines_.insert(position,
                  std::make_move_iterator(text.begin()),
                  std::make_move_iterator(text.end()));
    current_line_ = number + inserted_count;
    mark_modified();
}

void Buffer::replace(LineNumber number, std::string text) {
    require_existing_line(number);
    if (lines_.at(number - 1) != text) {
        lines_.at(number - 1) = std::move(text);
        mark_modified();
    }
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
    mark_modified();
}

void Buffer::set_current_line(LineNumber number) {
    if (number > lines_.size()) {
        throw std::out_of_range("line number out of range");
    }
    current_line_ = number;
}

void Buffer::load_file(std::vector<std::string> lines,
                       std::string filename,
                       TextEncoding encoding,
                       LineEnding line_ending,
                       bool final_newline) {
    lines_ = std::move(lines);
    current_line_ = lines_.size();
    associated_file_ = std::move(filename);
    encoding_ = encoding;
    line_ending_ = line_ending;
    final_newline_ = final_newline;
    modified_ = false;
}

void Buffer::associate_file(std::string filename,
                            TextEncoding encoding,
                            LineEnding line_ending,
                            bool final_newline) {
    associated_file_ = std::move(filename);
    encoding_ = encoding;
    line_ending_ = line_ending;
    final_newline_ = final_newline;
}

void Buffer::mark_clean() noexcept { modified_ = false; }
void Buffer::mark_modified() noexcept { modified_ = true; }

void Buffer::require_existing_line(LineNumber number) const {
    if (number == 0 || number > lines_.size()) {
        throw std::out_of_range("line number out of range");
    }
}

} // namespace fred
