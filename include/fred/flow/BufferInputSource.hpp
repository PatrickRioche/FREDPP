#pragma once

#include "fred/core/Buffer.hpp"
#include "fred/flow/InputSource.hpp"

#include <cstddef>
#include <optional>
#include <string>

namespace fred {

class BufferInputSource final : public InputSource {
public:
    BufferInputSource(const Buffer& buffer,
                      std::size_t level,
                      bool emit_newlines = true,
                      CharacterInterpretation interpretation =
                          CharacterInterpretation::Normal);

    [[nodiscard]] std::optional<InputCharacter> next() override;
    [[nodiscard]] std::string_view description() const noexcept override;
    [[nodiscard]] std::size_t level() const noexcept override;

private:
    const Buffer* buffer_;
    std::string description_;
    std::size_t level_;
    bool emit_newlines_;
    CharacterInterpretation interpretation_;
    std::size_t line_index_{0};
    std::size_t column_index_{0};
    bool emit_newline_{false};
};

} // namespace fred
