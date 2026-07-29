#pragma once

#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/Token.hpp"

#include <cstddef>
#include <vector>

namespace fred {

class TokenStream {
public:
    explicit TokenStream(Lexer& lexer);

    [[nodiscard]] const Token& peek(std::size_t lookahead = 0);
    [[nodiscard]] Token consume();
    [[nodiscard]] bool eof();

    [[nodiscard]] std::size_t position() const noexcept;
    void rewind(std::size_t position);

private:
    void ensure(std::size_t index);

    Lexer* lexer_;
    std::vector<Token> cache_;
    std::size_t position_{0};
    bool end_cached_{false};
};

} // namespace fred
