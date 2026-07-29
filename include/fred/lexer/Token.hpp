#pragma once

#include "fred/lexer/SourceLocation.hpp"
#include "fred/lexer/TokenType.hpp"

#include <string>

namespace fred {

struct Token {
    TokenType type{TokenType::Unknown};
    std::string lexeme;
    SourceLocation location;

    friend bool operator==(const Token&, const Token&) = default;
};

} // namespace fred
