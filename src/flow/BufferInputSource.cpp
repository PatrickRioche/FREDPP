#include "fred/flow/BufferInputSource.hpp"

namespace fred {

BufferInputSource::BufferInputSource(const Buffer& buffer, std::size_t level)
    : buffer_(&buffer),
      description_("buffer(" + buffer.name() + ")"),
      level_(level) {}

std::optional<InputCharacter> BufferInputSource::next() {
    while (line_index_ < buffer_->line_count()) {
        const auto& current = buffer_->lines().at(line_index_);

        if (column_index_ < current.size()) {
            return InputCharacter{current.at(column_index_++), level_};
        }

        if (!emit_newline_) {
            emit_newline_ = true;
            return InputCharacter{'\n', level_};
        }

        ++line_index_;
        column_index_ = 0;
        emit_newline_ = false;
    }

    return std::nullopt;
}

std::string_view BufferInputSource::description() const noexcept {
    return description_;
}

std::size_t BufferInputSource::level() const noexcept {
    return level_;
}

} // namespace fred
