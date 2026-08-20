#pragma once

#include "fred/ast/AddressNode.hpp"

#include <cstddef>

namespace fred {

/**
 * @brief AST node representing an absolute numeric line address.
 *
 * Example source forms: `0`, `1`, `125`.
 *
 * @note The parser deliberately preserves numeric zero. Whether a value is a
 *       valid line in a particular Buffer is a later evaluation concern.
 */
class AbsoluteAddressNode final : public AddressNode {
public:
    /**
     * @param line Parsed numeric spelling converted to std::size_t.
     * @param location Location of the first digit.
     */
    AbsoluteAddressNode(std::size_t line, SourceLocation location) noexcept
        : line_(line), location_(location) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::AbsoluteAddress;
    }

    [[nodiscard]] SourceLocation location() const noexcept override {
        return location_;
    }

    /** @return Parsed absolute line number without runtime validation. */
    [[nodiscard]] std::size_t line() const noexcept { return line_; }

private:
    std::size_t line_;
    SourceLocation location_;
};

} // namespace fred
