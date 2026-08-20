#pragma once

#include "fred/ast/AddressNode.hpp"

namespace fred {

/**
 * @brief AST node representing the current-line address `.`.
 *
 * The node stores only the source location. Resolution to the current line is
 * intentionally deferred to AddressEvaluator/runtime state.
 */
class CurrentAddressNode final : public AddressNode {
public:
    /** @param location Source location of the `.` token. */
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
