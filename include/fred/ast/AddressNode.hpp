#pragma once

#include "fred/ast/AstNode.hpp"

namespace fred {

/**
 * @brief Abstract base class for parsed FRED line-address expressions.
 *
 * Concrete address nodes preserve syntax only. They intentionally do not know
 * which Buffer is active and do not check whether a referenced line exists.
 * AddressEvaluator performs that later semantic step.
 */
class AddressNode : public AstNode {
public:
    ~AddressNode() override = default;
};

} // namespace fred
