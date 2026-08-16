#pragma once

#include "fred/lexer/CharacterStream.hpp"
#include "fred/lexer/StringCharacterStream.hpp"
#include "fred/lexer/Token.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace fred {

class Lexer {
public:
    explicit Lexer(std::string_view source, std::size_t flow_level = 0);
    explicit Lexer(CharacterStream& stream);

    Lexer(const Lexer&) = delete;
    Lexer& operator=(const Lexer&) = delete;
    Lexer(Lexer&&) = delete;
    Lexer& operator=(Lexer&&) = delete;

    [[nodiscard]] Token next();
    [[nodiscard]] std::vector<Token> tokenize();

private:
    [[nodiscard]] bool at_end() const noexcept;
    [[nodiscard]] char peek(std::size_t lookahead = 0) const noexcept;
    [[nodiscard]] Character advance() noexcept;
    [[nodiscard]] SourceLocation location() const noexcept;

    void skip_horizontal_whitespace() noexcept;

    [[nodiscard]] Token make_token(
        TokenType type,
        std::string lexeme,
        SourceLocation start_location,
        CharacterInterpretation interpretation =
            CharacterInterpretation::Normal) const;

    [[nodiscard]] Token lex_number();
    [[nodiscard]] Token lex_identifier_or_command();

    std::unique_ptr<CharacterStream> owned_stream_;
    CharacterStream* stream_{nullptr};
};

} // namespace fred
