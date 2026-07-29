#include "fred/runtime/AddressEvaluator.hpp"

#include "fred/ast/AbsoluteAddressNode.hpp"
#include "fred/ast/CurrentAddressNode.hpp"
#include "fred/ast/LastAddressNode.hpp"
#include "fred/ast/RangeAddressNode.hpp"
#include "fred/ast/RelativeAddressNode.hpp"
#include "fred/runtime/CommandExecutionError.hpp"

#include <string>

namespace fred {
namespace {

void require_existing(Buffer::LineNumber line, const Buffer& buffer) {
    if (line == 0 || line > buffer.line_count()) {
        throw CommandExecutionError("line address out of range: " + std::to_string(line));
    }
}

} // namespace

LineRange AddressEvaluator::evaluate(const AddressNode* address,
                                     const Buffer& buffer) const {
    if (buffer.empty()) {
        throw CommandExecutionError("current buffer is empty");
    }

    if (address == nullptr) {
        require_existing(buffer.current_line(), buffer);
        return {buffer.current_line(), buffer.current_line()};
    }

    if (address->kind() == AstNodeKind::RangeAddress) {
        const auto& range = static_cast<const RangeAddressNode&>(*address);
        const auto first = evaluate_single(range.first(), buffer);
        const auto last = evaluate_single(range.last(), buffer);
        if (first > last) {
            throw CommandExecutionError("first address exceeds last address");
        }
        return {first, last};
    }

    const auto line = evaluate_single(*address, buffer);
    return {line, line};
}

Buffer::LineNumber AddressEvaluator::evaluate_single(const AddressNode& address,
                                                      const Buffer& buffer) const {
    Buffer::LineNumber result{};
    switch (address.kind()) {
    case AstNodeKind::AbsoluteAddress:
        result = static_cast<const AbsoluteAddressNode&>(address).line();
        break;
    case AstNodeKind::CurrentAddress:
        result = buffer.current_line();
        break;
    case AstNodeKind::LastAddress:
        result = buffer.line_count();
        break;
    case AstNodeKind::RelativeAddress: {
        const auto& relative = static_cast<const RelativeAddressNode&>(address);
        const auto current = buffer.current_line();
        if (relative.direction() == RelativeDirection::Forward) {
            result = current + relative.distance();
        } else {
            if (relative.distance() >= current) {
                throw CommandExecutionError("relative line address out of range");
            }
            result = current - relative.distance();
        }
        break;
    }
    default:
        throw CommandExecutionError("unsupported address expression");
    }

    require_existing(result, buffer);
    return result;
}

} // namespace fred
