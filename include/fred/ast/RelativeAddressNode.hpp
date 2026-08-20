#pragma once

#include "fred/ast/AddressNode.hpp"

#include <cstddef>

namespace fred {

/**
 * @brief Direction encoded by a relative FRED address.
 */
enum class RelativeDirection {
    /** `+n`: move forward from the current address. */
    Forward,

    /** `-n`: move backward from the current address. */
    Backward
};

/**
 * @brief AST node representing a relative line address such as `+3` or `-2`.
 *
 * This node stores direction and distance only. It does not resolve the
 * resulting line or check Buffer boundaries.
 */
class RelativeAddressNode final : public AddressNode {
public:
    /**
     * @param direction Forward for `+`, Backward for `-`.
     * @param distance Parsed non-negative magnitude.
     * @param location Source location of the sign character.
     */
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

    /** @return Parsed relative direction. */
    [[nodiscard]] RelativeDirection direction() const noexcept {
        return direction_;
    }

    /** @return Parsed distance after the sign. */
    [[nodiscard]] std::size_t distance() const noexcept { return distance_; }

private:
    RelativeDirection direction_;
    std::size_t distance_;
    SourceLocation location_;
};

} // namespace fred
