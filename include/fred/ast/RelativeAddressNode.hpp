#pragma once

#include "fred/ast/AddressNode.hpp"

#include <cstddef>

namespace fred {

enum class RelativeDirection {
    Forward,
    Backward
};

class RelativeAddressNode final : public AddressNode {
public:
    RelativeAddressNode(RelativeDirection direction,
                        std::size_t distance,
                        SourceLocation location) noexcept
        : direction_(direction), distance_(distance), location_(location) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::RelativeAddress;
    }

    [[nodiscard]] SourceLocation location() const noexcept override {
        return location_;
    }

    [[nodiscard]] RelativeDirection direction() const noexcept {
        return direction_;
    }

    [[nodiscard]] std::size_t distance() const noexcept { return distance_; }

private:
    RelativeDirection direction_;
    std::size_t distance_;
    SourceLocation location_;
};

} // namespace fred
