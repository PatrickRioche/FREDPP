#pragma once

#include "fred/ast/AddressNode.hpp"

namespace fred {

class LastAddressNode final : public AddressNode {
public:
    explicit LastAddressNode(SourceLocation location) noexcept
        : location_(location) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::LastAddress;
    }

    [[nodiscard]] SourceLocation location() const noexcept override {
        return location_;
    }

private:
    SourceLocation location_;
};

} // namespace fred
