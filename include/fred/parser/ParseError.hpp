#pragma once

#include "fred/lexer/SourceLocation.hpp"

#include <stdexcept>
#include <string>
#include <utility>

namespace fred {

/**
 * @brief Exception raised when source text cannot be converted to the expected AST.
 *
 * ParseError combines the standard runtime_error message (`what()`) with the
 * SourceLocation at which parsing detected the problem.
 *
 * @note ParseError itself does not format a complete user diagnostic. The
 *       diagnostic layer may combine what() and location() with source text.
 */
class ParseError : public std::runtime_error {
public:
    /**
     * @param message Human-readable parser error.
     * @param location Source position associated with the failure.
     */
    ParseError(std::string message, SourceLocation location)
        : std::runtime_error(std::move(message)), location_(location) {}

    /** @return Location captured when the parse error was created. */
    [[nodiscard]] SourceLocation location() const noexcept {
        return location_;
    }

private:
    SourceLocation location_;
};

} // namespace fred
