#pragma once

#include "fred/ast/CommandNode.hpp"
#include "fred/runtime/CommandExecutionError.hpp"
#include "fred/runtime/ExecutionContext.hpp"

#include <bit>
#include <cstdint>
#include <limits>
#include <string_view>

namespace fred {

/**
 * @brief Applies one numeric command, preserving state on arithmetic errors.
 *
 * Comparisons update only the boolean condition and the shared counter.
 * Mutations update the named register and counter, leaving condition intact.
 * Missing registers read as zero, consistently with N: and procedure code.
 */
inline void apply_numeric_operation(ExecutionContext& context,
                                    std::string_view name,
                                    NumericOperationKind operation,
                                    std::int64_t operand = 0) {
    using Limit = std::numeric_limits<std::int64_t>;
    const auto current = context.numeric_register(name);
    std::int64_t result = current;

    switch (operation) {
    case NumericOperationKind::Equal:
        context.set_condition(current == operand);
        context.set_counter(current);
        return;
    case NumericOperationKind::Less:
        context.set_condition(current < operand);
        context.set_counter(current);
        return;
    case NumericOperationKind::Greater:
        context.set_condition(current > operand);
        context.set_counter(current);
        return;
    case NumericOperationKind::Add:
        if ((operand > 0 && current > Limit::max() - operand) ||
            (operand < 0 && current < Limit::min() - operand)) {
            throw CommandExecutionError("numeric addition overflow");
        }
        result = current + operand;
        break;
    case NumericOperationKind::Subtract:
        if ((operand < 0 && current > Limit::max() + operand) ||
            (operand > 0 && current < Limit::min() + operand)) {
            throw CommandExecutionError("numeric subtraction overflow");
        }
        result = current - operand;
        break;
    case NumericOperationKind::Multiply:
        if (current != 0 && operand != 0) {
            if ((current == -1 && operand == Limit::min()) ||
                (operand == -1 && current == Limit::min()) ||
                (current > 0 && operand > 0 && current > Limit::max() / operand) ||
                (current > 0 && operand < 0 && operand < Limit::min() / current) ||
                (current < 0 && operand > 0 && current < Limit::min() / operand) ||
                (current < 0 && operand < 0 && current < Limit::max() / operand)) {
                throw CommandExecutionError("numeric multiplication overflow");
            }
        }
        result = current * operand;
        break;
    case NumericOperationKind::Divide:
    case NumericOperationKind::Modulo:
        if (operand == 0) {
            throw CommandExecutionError("numeric division by zero");
        }
        if (current == Limit::min() && operand == -1) {
            throw CommandExecutionError("numeric division overflow");
        }
        result = operation == NumericOperationKind::Divide
            ? current / operand : current % operand;
        break;
    case NumericOperationKind::BitAnd:
        result = std::bit_cast<std::int64_t>(
            std::bit_cast<std::uint64_t>(current) &
            std::bit_cast<std::uint64_t>(operand));
        break;
    case NumericOperationKind::BitOr:
        result = std::bit_cast<std::int64_t>(
            std::bit_cast<std::uint64_t>(current) |
            std::bit_cast<std::uint64_t>(operand));
        break;
    case NumericOperationKind::Complement:
        result = std::bit_cast<std::int64_t>(
            ~std::bit_cast<std::uint64_t>(current));
        break;
    case NumericOperationKind::BitXor:
        result = std::bit_cast<std::int64_t>(
            std::bit_cast<std::uint64_t>(current) ^
            std::bit_cast<std::uint64_t>(operand));
        break;
    case NumericOperationKind::ShiftLeft:
    case NumericOperationKind::ShiftRight:
        if (operand < 0 || operand >= 64) {
            throw CommandExecutionError("numeric shift count must be between 0 and 63");
        }
        // Left shift is a 64-bit bit-pattern operation (high bits discarded).
        // C++20 defines signed right shift of negatives as sign-extending.
        result = operation == NumericOperationKind::ShiftLeft
            ? std::bit_cast<std::int64_t>(
                std::bit_cast<std::uint64_t>(current) << operand)
            : current >> operand;
        break;
    case NumericOperationKind::Absolute:
        if (current == Limit::min()) {
            throw CommandExecutionError("numeric absolute value overflow");
        }
        result = current < 0 ? -current : current;
        break;
    case NumericOperationKind::CurrentLine:
    case NumericOperationKind::ChangeBase:
    case NumericOperationKind::MinimumDigits:
    case NumericOperationKind::FillCharacter:
    case NumericOperationKind::CountCharacters:
    case NumericOperationKind::Print:
        throw CommandExecutionError("numeric operation requires runtime context");
    }

    context.set_numeric_register(std::string(name), result);
    context.set_counter(result);
}

} // namespace fred
