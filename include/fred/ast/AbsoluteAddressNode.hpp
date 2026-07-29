#pragma once

#include "fred/ast/AddressNode.hpp"

#include <cstddef>

namespace fred {

class AbsoluteAddressNode final : public AddressNode {
public:
    AbsoluteAddressNode(std::size_t line, SourceLocation location) noexcept
        : line_(line), location_(location) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::AbsoluteAddress;
    }

    [[nodiscard]] SourceLocation location() const noexcept override {
        return location_;
    }

    [[nodiscard]] std::size_t line() const noexcept { return line_; }

private:
    std::size_t line_;
    SourceLocation location_;
};

} // namespace fred
