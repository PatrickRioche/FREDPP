#pragma once

#include "fred/ast/AddressNode.hpp"

#include <memory>
#include <utility>

namespace fred {

class RangeAddressNode final : public AddressNode {
public:
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

    [[nodiscard]] const AddressNode& first() const noexcept { return *first_; }
    [[nodiscard]] const AddressNode& last() const noexcept { return *last_; }

private:
    std::unique_ptr<AddressNode> first_;
    std::unique_ptr<AddressNode> last_;
    SourceLocation location_;
};

} // namespace fred
