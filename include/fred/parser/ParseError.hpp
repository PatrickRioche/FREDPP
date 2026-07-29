#pragma once

#include "fred/lexer/SourceLocation.hpp"

#include <stdexcept>
#include <string>
#include <utility>

namespace fred {

class ParseError : public std::runtime_error {
public:
    ParseError(std::string message, SourceLocation location)
        : std::runtime_error(std::move(message)), location_(location) {}

    [[nodiscard]] SourceLocation location() const noexcept {
        return location_;
    }

private:
    SourceLocation location_;
};

} // namespace fred
