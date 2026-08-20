#pragma once

#include "fred/ast/AddressNode.hpp"
#include "fred/core/Buffer.hpp"

namespace fred {

/**
 * @brief Inclusive runtime line range produced by address evaluation.
 *
 * Both endpoints use Buffer's one-based LineNumber convention.
 */
struct LineRange {
    Buffer::LineNumber first{};
    Buffer::LineNumber last{};
};

/**
 * @brief Resolves parsed AddressNode syntax against a concrete Buffer.
 *
 * AddressEvaluator is the architectural bridge between parser-owned address AST
 * and mutable editor state. It does not parse source text and does not mutate
 * the Buffer.
 *
 * @par Default address
 * A null AddressNode means the Buffer's current line.
 *
 * @par Validation
 * Except for command-specific insertion rules handled by CommandExecutor,
 * evaluated lines must already exist in the Buffer. Empty-buffer evaluation,
 * line 0, out-of-range lines and reversed ranges raise CommandExecutionError.
 */
class AddressEvaluator {
public:
    /**
     * @param address Address AST, or nullptr for the current-line default.
     * @param buffer Buffer providing current/last line state.
     * @return Inclusive validated LineRange.
     *
     * @throws CommandExecutionError if the Buffer is empty, the current/default
     *         line is invalid, an endpoint is out of range, a relative address
     *         cannot be resolved, a range is reversed, or the AST kind is not a
     *         supported address expression.
     */
    [[nodiscard]] LineRange evaluate(const AddressNode* address,
                                     const Buffer& buffer) const;

private:
    /**
     * @brief Resolves one non-range address node.
     *
     * @return One existing Buffer line number.
     * @throws CommandExecutionError for invalid/out-of-range/unsupported forms.
     */
    [[nodiscard]] Buffer::LineNumber
    evaluate_single(const AddressNode& address,
                    const Buffer& buffer) const;
};

} // namespace fred
