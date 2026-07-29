#pragma once

#include <string_view>

namespace fred {

enum class TokenType {
    End,
    Identifier,
    Number,
    Command,
    Comma,
    LeftParenthesis,
    RightParenthesis,
    Backslash,
    NewLine,
    Symbol,
    Unknown
};

[[nodiscard]] constexpr std::string_view token_type_name(TokenType type) noexcept {
    switch (type) {
    case TokenType::End: return "End";
    case TokenType::Identifier: return "Identifier";
    case TokenType::Number: return "Number";
    case TokenType::Command: return "Command";
    case TokenType::Comma: return "Comma";
    case TokenType::LeftParenthesis: return "LeftParenthesis";
    case TokenType::RightParenthesis: return "RightParenthesis";
    case TokenType::Backslash: return "Backslash";
    case TokenType::NewLine: return "NewLine";
    case TokenType::Symbol: return "Symbol";
    case TokenType::Unknown: return "Unknown";
    }
    return "Unknown";
}

} // namespace fred
