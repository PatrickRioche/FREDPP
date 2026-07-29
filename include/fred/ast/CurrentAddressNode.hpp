#pragma once

#include "fred/ast/AddressNode.hpp"

namespace fred {

class CurrentAddressNode final : public AddressNode {
public:
    explicit CurrentAddressNode(SourceLocation location) noexcept
        : location_(location) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::CurrentAddress;
    }

    [[nodiscard]] SourceLocation location() const noexcept override {
        return location_;
    }

private:
    SourceLocation location_;
};

} // namespace fred
