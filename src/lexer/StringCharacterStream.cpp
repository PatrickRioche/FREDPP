#include "fred/lexer/StringCharacterStream.hpp"

#include <stdexcept>
#include <utility>

namespace fred {

StringCharacterStream::StringCharacterStream(std::string_view source,
                                             std::size_t flow_level)
    : storage_(source) {
    characters_.reserve(storage_.size());

    std::size_t line = 1;
    std::size_t column = 1;

    for (std::size_t offset = 0; offset < storage_.size(); ++offset) {
        const char value = storage_[offset];
        characters_.push_back(
            Character{value, SourceLocation{offset, line, column, flow_level}}
        );

        if (value == '\n') {
            ++line;
            column = 1;
        } else {
            ++column;
        }
    }

    end_location_ =
        SourceLocation{storage_.size(), line, column, flow_level};
}

std::optional<Character>
StringCharacterStream::peek(std::size_t lookahead) const noexcept {
    const auto index = position_ + lookahead;
    if (index >= characters_.size()) {
        return std::nullopt;
    }
    return characters_[index];
}

std::optional<Character> StringCharacterStream::consume() noexcept {
    if (eof()) {
        return std::nullopt;
    }
    return characters_[position_++];
}

bool StringCharacterStream::eof() const noexcept {
    return position_ >= characters_.size();
}

std::size_t StringCharacterStream::position() const noexcept {
    return position_;
}

void StringCharacterStream::rewind(std::size_t position) {
    if (position > characters_.size()) {
        throw std::out_of_range("character stream position out of range");
    }
    position_ = position;
}

SourceLocation StringCharacterStream::end_location() const noexcept {
    return end_location_;
}

std::size_t StringCharacterStream::size() const noexcept {
    return characters_.size();
}

} // namespace fred
