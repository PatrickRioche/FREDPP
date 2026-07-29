#pragma once

#include "fred/ast/AddressNode.hpp"
#include "fred/core/Buffer.hpp"

namespace fred {

struct LineRange {
    Buffer::LineNumber first{};
    Buffer::LineNumber last{};
};

class AddressEvaluator {
public:
    [[nodiscard]] LineRange evaluate(const AddressNode* address,
                                     const Buffer& buffer) const;

private:
    [[nodiscard]] Buffer::LineNumber evaluate_single(const AddressNode& address,
                                                     const Buffer& buffer) const;
};

} // namespace fred
