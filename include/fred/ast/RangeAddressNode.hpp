#pragma once

#include "fred/ast/AddressNode.hpp"

#include <memory>
#include <utility>

namespace fred {

/**
 * @brief AST node representing a two-endpoint address range `first,last`.
 *
 * RangeAddressNode takes exclusive ownership of both endpoint nodes. The range
 * is syntactic: ordering, existence and Buffer bounds are not checked here.
 */
class RangeAddressNode final : public AddressNode {
public:
    /**
     * @param first First endpoint; ownership is transferred to this node.
     * @param last Last endpoint; ownership is transferred to this node.
     * @param location Start location of the complete range (normally `first`).
     *
     * @pre `first` and `last` must be non-null. The constructor does not enforce
     *      this invariant, while first()/last() dereference the stored pointers.
     */
    RangeAddressNode(std::unique_ptr<AddressNode> first,
                     std::unique_ptr<AddressNode> last,
                     SourceLocation location) noexcept
        : first_(std::move(first)),
          last_(std::move(last)),
          location_(location) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::RangeAddress;
    }

    [[nodiscard]] SourceLocation location() const noexcept override {
        return location_;
    }

    /** @return Non-owning reference to the first endpoint. */
    [[nodiscard]] const AddressNode& first() const noexcept { return *first_; }

    /** @return Non-owning reference to the last endpoint. */
    [[nodiscard]] const AddressNode& last() const noexcept { return *last_; }

private:
    std::unique_ptr<AddressNode> first_;
    std::unique_ptr<AddressNode> last_;
    SourceLocation location_;
};

} // namespace fred
