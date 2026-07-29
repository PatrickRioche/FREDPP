#pragma once

#include "fred/lexer/Character.hpp"

#include <cstddef>
#include <optional>

namespace fred {

class CharacterStream {
public:
    virtual ~CharacterStream() = default;

    [[nodiscard]] virtual std::optional<Character>
    peek(std::size_t lookahead = 0) const noexcept = 0;

    [[nodiscard]] virtual std::optional<Character> consume() noexcept = 0;

    [[nodiscard]] virtual bool eof() const noexcept = 0;
    [[nodiscard]] virtual std::size_t position() const noexcept = 0;

    virtual void rewind(std::size_t position) = 0;

    [[nodiscard]] virtual SourceLocation end_location() const noexcept = 0;
};

} // namespace fred
