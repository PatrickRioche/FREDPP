#pragma once

#include "fred/flow/InputCharacter.hpp"
#include "fred/lexer/CharacterStream.hpp"

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace fred {

class FlowCharacterStream final : public CharacterStream {
public:
    explicit FlowCharacterStream(std::vector<InputCharacter> input) {
        characters_.reserve(input.size());

        std::size_t line = 1;
        std::size_t column = 1;

        for (std::size_t offset = 0; offset < input.size(); ++offset) {
            const auto& source = input[offset];
            characters_.push_back(Character{
                source.value,
                SourceLocation{offset, line, column, source.level},
                source.interpretation
            });

            if (source.value == '\n') {
                ++line;
                column = 1;
            } else {
                ++column;
            }
        }

        const std::size_t end_level =
            input.empty() ? 0 : input.back().level;
        end_location_ =
            SourceLocation{input.size(), line, column, end_level};
    }

    [[nodiscard]] std::optional<Character>
    peek(std::size_t lookahead = 0) const noexcept override {
        const auto index = position_ + lookahead;
        if (index >= characters_.size()) {
            return std::nullopt;
        }
        return characters_[index];
    }

    [[nodiscard]] std::optional<Character> consume() noexcept override {
        if (eof()) {
            return std::nullopt;
        }
        return characters_[position_++];
    }

    [[nodiscard]] bool eof() const noexcept override {
        return position_ >= characters_.size();
    }

    [[nodiscard]] std::size_t position() const noexcept override {
        return position_;
    }

    void rewind(std::size_t position) override {
        if (position > characters_.size()) {
            throw std::out_of_range(
                "flow character stream position out of range");
        }
        position_ = position;
    }

    [[nodiscard]] SourceLocation end_location() const noexcept override {
        return end_location_;
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return characters_.size();
    }

private:
    std::vector<Character> characters_;
    std::size_t position_{0};
    SourceLocation end_location_{};
};

} // namespace fred
