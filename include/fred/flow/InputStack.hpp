#pragma once

#include "fred/flow/InputSource.hpp"

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

namespace fred {

/**
 * @brief LIFO stack of nested flow input sources.
 *
 * InputStack implements the "inject a source, then resume its caller" behavior
 * used by flow-buffer expansion. The most recently pushed source is read first.
 * When it reaches EOF it is destroyed and reading resumes from the previous
 * source automatically.
 *
 * @par Ownership
 * push() transfers exclusive ownership of InputSource objects into the stack.
 *
 * @par Depth protection
 * A configurable maximum depth prevents unbounded recursive buffer injection.
 */
class InputStack {
public:
    /**
     * @param maximum_depth Maximum simultaneously stacked sources.
     * @throws std::invalid_argument when maximum_depth is zero.
     */
    explicit InputStack(std::size_t maximum_depth = 256);

    /**
     * @brief Pushes a new source on top of the stack.
     *
     * @param source Source whose ownership is transferred.
     * @throws std::invalid_argument for nullptr.
     * @throws std::runtime_error when maximum_depth() is already reached.
     */
    void push(std::unique_ptr<InputSource> source);

    /**
     * @brief Reads from the top source, popping exhausted sources as needed.
     *
     * @return Next character from the first non-exhausted source, or nullopt
     *         when the entire stack is exhausted.
     *
     * @post Any exhausted top sources encountered are destroyed.
     */
    [[nodiscard]] std::optional<InputCharacter> next();

    /** @return true when no owned InputSource remains. */
    [[nodiscard]] bool empty() const noexcept;

    /** @return Number of currently stacked sources. */
    [[nodiscard]] std::size_t depth() const noexcept;

    /** @return Configured maximum simultaneous source depth. */
    [[nodiscard]] std::size_t maximum_depth() const noexcept;

private:
    /** Owned sources, with the active/top source at back(). */
    std::vector<std::unique_ptr<InputSource>> sources_;
    std::size_t maximum_depth_;
};

} // namespace fred
