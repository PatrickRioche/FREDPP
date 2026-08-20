#pragma once

#include "fred/ast/AddressNode.hpp"

namespace fred {

/**
 * @brief AST node representing the last-line address `$`.
 *
 * The actual last line depends on Buffer state and is not resolved by this
 * syntax node.
 */
class LastAddressNode final : public AddressNode {
public:
    /** @param location Source location of the `$` token. */
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
