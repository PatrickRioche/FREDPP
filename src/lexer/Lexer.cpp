#include "fred/lexer/Lexer.hpp"

#include <stdexcept>
#include <utility>

namespace fred {

namespace {

bool is_ascii_digit(char value) noexcept {
    return value >= '0' && value <= '9';
}

bool is_ascii_lower(char value) noexcept {
    return value >= 'a' && value <= 'z';
}

bool is_ascii_upper(char value) noexcept {
    return value >= 'A' && value <= 'Z';
}

bool is_ascii_letter(char value) noexcept {
    return is_ascii_lower(value) || is_ascii_upper(value);
}

bool is_identifier_continue(char value) noexcept {
    return is_ascii_letter(value) || is_ascii_digit(value) || value == '_';
}

bool is_printable_ascii(char value) noexcept {
    const auto byte = static_cast<unsigned char>(value);
    return byte >= 32 && byte <= 126;
}

bool is_literal_lexical_syntax(
    const Character& character) noexcept {
    if (character.interpretation !=
        CharacterInterpretation::Literal) {
        return false;
    }

    const char value = character.value;

    // Keep letters, digits and '_' eligible for normal identifiers/numbers
    // produced by \S and \L. Literal protection neutralizes characters
    // that would otherwise be lexical structure.
    return !is_ascii_letter(value) &&
           !is_ascii_digit(value) &&
           value != '_';
}

} // namespace

Lexer::Lexer(std::string_view source, std::size_t flow_level)
    : owned_stream_(
          std::make_unique<StringCharacterStream>(source, flow_level)),
      stream_(owned_stream_.get()) {}

Lexer::Lexer(CharacterStream& stream)
    : stream_(&stream) {}

Token Lexer::next() {
    skip_horizontal_whitespace();

    if (at_end()) {
        return Token{TokenType::End, {}, location()};
    }

    const auto start_location = location();
    const auto current_character = stream_->peek();
    const char current = peek();

    if (current_character &&
        is_literal_lexical_syntax(*current_character)) {
        const Character consumed = advance();
        return make_token(
            TokenType::Symbol,
            std::string(1, consumed.value),
            start_location
        );
    }

    if (is_ascii_digit(current)) {
        return lex_number();
    }

    // A historical command can be immediately followed by a numeric
    // argument (for example, 2,3M5). Keep the command letter separate
    // instead of lexing the pair as an identifier such as "M5".
    if (is_ascii_letter(current) && is_ascii_digit(peek(1))) {
        const Character consumed = advance();
        return make_token(TokenType::Command,
                          std::string(1, consumed.value),
                          start_location);
    }

    if (is_ascii_letter(current) || current == '_') {
        return lex_identifier_or_command();
    }

    const Character consumed = advance();

    switch (current) {
    case ',':
        return make_token(TokenType::Comma,
                          std::string(1, consumed.value),
                          start_location);
    case '(':
        return make_token(TokenType::LeftParenthesis,
                          std::string(1, consumed.value),
                          start_location);
    case ')':
        return make_token(TokenType::RightParenthesis,
                          std::string(1, consumed.value),
                          start_location);
    case '\\':
        return make_token(TokenType::Backslash,
                          std::string(1, consumed.value),
                          start_location);
    case '\n':
        return make_token(TokenType::NewLine,
                          std::string(1, consumed.value),
                          start_location);
    default:
        return make_token(
            is_printable_ascii(current) ? TokenType::Symbol
                                        : TokenType::Unknown,
            std::string(1, consumed.value),
            start_location
        );
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> result;

    while (true) {
        auto token = next();
        const bool done = token.type == TokenType::End;
        result.push_back(std::move(token));
        if (done) {
            break;
        }
    }

    return result;
}

bool Lexer::at_end() const noexcept {
    return stream_->eof();
}

char Lexer::peek(std::size_t lookahead) const noexcept {
    const auto character = stream_->peek(lookahead);
    return character ? character->value : '\0';
}

Character Lexer::advance() noexcept {
    const auto character = stream_->consume();
    if (character) {
        return *character;
    }
    return Character{'\0', stream_->end_location()};
}

SourceLocation Lexer::location() const noexcept {
    const auto character = stream_->peek();
    return character ? character->location : stream_->end_location();
}

void Lexer::skip_horizontal_whitespace() noexcept {
    while (!at_end()) {
        const auto character = stream_->peek();
        if (!character ||
            character->interpretation ==
                CharacterInterpretation::Literal) {
            break;
        }

        const char value = character->value;
        if (value == ' ' || value == '\t' || value == '\r') {
            (void)advance();
        } else {
            break;
        }
    }
}

Token Lexer::make_token(TokenType type,
                        std::string lexeme,
                        SourceLocation start_location) const {
    return Token{type, std::move(lexeme), start_location};
}

Token Lexer::lex_number() {
    const auto start_location = location();
    std::string lexeme;

    while (is_ascii_digit(peek())) {
        lexeme.push_back(advance().value);
    }

    return make_token(TokenType::Number,
                      std::move(lexeme),
                      start_location);
}

Token Lexer::lex_identifier_or_command() {
    const auto start_location = location();
    std::string lexeme;

    while (is_identifier_continue(peek())) {
        lexeme.push_back(advance().value);
    }

    const bool command =
        lexeme.size() == 1 && is_ascii_letter(lexeme.front());

    return make_token(
        command ? TokenType::Command : TokenType::Identifier,
        std::move(lexeme),
        start_location
    );
}

} // namespace fred
