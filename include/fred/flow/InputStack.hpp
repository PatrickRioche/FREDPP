#pragma once

#include "fred/flow/InputSource.hpp"

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

namespace fred {

class InputStack {
public:
    explicit InputStack(std::size_t maximum_depth = 256);

    void push(std::unique_ptr<InputSource> source);
    [[nodiscard]] std::optional<InputCharacter> next();

    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] std::size_t depth() const noexcept;
    [[nodiscard]] std::size_t maximum_depth() const noexcept;

private:
    std::vector<std::unique_ptr<InputSource>> sources_;
    std::size_t maximum_depth_;
};

} // namespace fred
