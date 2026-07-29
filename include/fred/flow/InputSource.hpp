#pragma once

#include "fred/flow/InputCharacter.hpp"

#include <optional>
#include <string_view>

namespace fred {

class InputSource {
public:
    virtual ~InputSource() = default;

    [[nodiscard]] virtual std::optional<InputCharacter> next() = 0;
    [[nodiscard]] virtual std::string_view description() const noexcept = 0;
    [[nodiscard]] virtual std::size_t level() const noexcept = 0;
};

} // namespace fred
