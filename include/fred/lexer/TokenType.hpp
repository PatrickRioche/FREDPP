#pragma once

#include <string_view>

namespace fred {

/**
 * @brief Lexical categories emitted by Lexer.
 *
 * TokenType describes syntax only. Command meaning and validity are resolved
 * later by parser/registry layers.
 */
enum class TokenType {
    /** Synthetic token marking end of input. */
    End,

    /** Multi-character name: ASCII letter/underscore followed by identifier characters. */
    Identifier,

    /** One or more ASCII decimal digits. */
    Number,

    /**
     * Single ASCII letter treated as a command mnemonic candidate.
     *
     * Whether the mnemonic is actually registered is a CommandParser concern.
     */
    Command,

    /** ',' */
    Comma,

    /** '(' */
    LeftParenthesis,

    /** ')' */
    RightParenthesis,

    /** '\' when it remains structural rather than Literal. */
    Backslash,

    /** Newline character '\n'. */
    NewLine,

    /** Printable character without a dedicated structural token type. */
    Symbol,

    /** Non-printable/unclassified byte not otherwise recognized. */
    Unknown
};

/**
 * @brief Converts a TokenType to its stable diagnostic/debug name.
 *
 * @param type Token category to describe.
 * @return Static string view; the caller does not own the returned storage.
 *
 * @note The fallback is "Unknown", including for an invalid enum value.
 */
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
