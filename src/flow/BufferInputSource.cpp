#include "fred/flow/BufferInputSource.hpp"

namespace fred {

BufferInputSource::BufferInputSource(
    const Buffer& buffer,
    std::size_t level,
    bool emit_newlines,
    CharacterInterpretation interpretation)
    : buffer_(&buffer),
      description_("buffer(" + buffer.name() + ")"),
      level_(level),
      emit_newlines_(emit_newlines),
      interpretation_(interpretation) {}

std::optional<InputCharacter> BufferInputSource::next() {
    while (line_index_ < buffer_->line_count()) {
        const auto& current = buffer_->lines().at(line_index_);

        if (column_index_ < current.size()) {
            return InputCharacter{
                current.at(column_index_++), level_, interpretation_};
        }

        // When requested, each logical Buffer line contributes exactly one
        // generated newline, including the final line.
        if (emit_newlines_ && !emit_newline_) {
            emit_newline_ = true;
            return InputCharacter{'\n', level_, interpretation_};
        }

        // Either the newline has already been emitted or newlines are
        // suppressed. Advance to the next logical line.
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
