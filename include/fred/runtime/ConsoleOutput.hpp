#pragma once

#include "fred/runtime/Output.hpp"

#include <iosfwd>

namespace fred {

/**
 * @brief Output adapter that forwards runtime-visible text to std::ostream.
 *
 * ConsoleOutput does not own the stream. It is suitable for CLI/terminal
 * integration while preserving the Runtime -> Output abstraction.
 */
class ConsoleOutput final : public Output {
public:
    /**
     * @param stream Destination stream; ownership is not transferred.
     * @pre `stream` must outlive this ConsoleOutput.
     */
    explicit ConsoleOutput(std::ostream& stream) noexcept;

    /**
     * @brief Writes exactly `text.size()` bytes to the borrowed stream.
     *
     * @param text Text to forward.
     *
     * @note Stream failure state is left on the underlying std::ostream; this
     *       method does not translate it to CommandExecutionError.
     */
    void write(std::string_view text) override;

private:
    /** Borrowed stream; never deleted by ConsoleOutput. */
    std::ostream* stream_;
};

} // namespace fred
