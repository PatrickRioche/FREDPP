#pragma once

#include "fred/flow/InputCharacter.hpp"
#include "fred/lexer/CharacterStream.hpp"

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace fred {

/**
 * @brief CharacterStream adapter for already-expanded flow characters.
 *
 * FlowCharacterStream owns an expanded vector of InputCharacter values and
 * converts it once to Lexer Character objects. It preserves flow level and
 * CharacterInterpretation while computing ordinary output-relative
 * offset/line/column locations.
 *
 * This class is the bridge from the flow-expansion layer into Lexer.
 */
class FlowCharacterStream final : public CharacterStream {
public:
    /**
     * @brief Takes ownership of expanded input and precomputes locations.
     *
     * @param input Expanded characters to consume.
     *
     * @post Locations are based on the expanded character stream:
     *       offset starts at 0; line/column start at 1.
     * @post `\n` increments line and resets column to 1.
     *
     * @note end_location().flow_level is the last input character's level, or 0
     *       for empty input.
     */
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

    /**
     * @return Character at current position + lookahead, or nullopt past EOF.
     * @note Does not change position().
     */
    [[nodiscard]] std::optional<Character>
    peek(std::size_t lookahead = 0) const noexcept override {
        const auto index = position_ + lookahead;
        if (index >= characters_.size()) {
            return std::nullopt;
        }
        return characters_[index];
    }

    /**
     * @return Current character and advances one position, or nullopt at EOF.
     */
    [[nodiscard]] std::optional<Character> consume() noexcept override {
        if (eof()) {
            return std::nullopt;
        }
        return characters_[position_++];
    }

    /** @return true when position() is at or beyond size(). */
    [[nodiscard]] bool eof() const noexcept override {
        return position_ >= characters_.size();
    }

    /** @return Current zero-based stream cursor. */
    [[nodiscard]] std::size_t position() const noexcept override {
        return position_;
    }

    /**
     * @brief Sets the stream cursor to any valid position, including EOF.
     *
     * @param position New position in 0..size().
     * @throws std::out_of_range when position > size().
     *
     * @note This API permits both backward rewind and forward repositioning.
     */
    void rewind(std::size_t position) override {
        if (position > characters_.size()) {
            throw std::out_of_range(
                "flow character stream position out of range");
        }
        position_ = position;
    }

    /** @return Precomputed location immediately after the final character. */
    [[nodiscard]] SourceLocation end_location() const noexcept override {
        return end_location_;
    }

    /** @return Number of owned Character values. */
    [[nodiscard]] std::size_t size() const noexcept {
        return characters_.size();
    }

private:
    std::vector<Character> characters_;
    std::size_t position_{0};
    SourceLocation end_location_{};
};

} // namespace fred
