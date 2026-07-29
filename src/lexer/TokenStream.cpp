#include "fred/lexer/TokenStream.hpp"

#include <stdexcept>
#include <utility>

namespace fred {

TokenStream::TokenStream(Lexer& lexer)
    : lexer_(&lexer) {}

const Token& TokenStream::peek(std::size_t lookahead) {
    ensure(position_ + lookahead);

    const auto requested = position_ + lookahead;
    if (requested < cache_.size()) {
        return cache_[requested];
    }

    return cache_.back();
}

Token TokenStream::consume() {
    const Token token = peek();

    if (token.type != TokenType::End) {
        ++position_;
    }

    return token;
}

bool TokenStream::eof() {
    return peek().type == TokenType::End;
}

std::size_t TokenStream::position() const noexcept {
    return position_;
}

void TokenStream::rewind(std::size_t position) {
    if (position > position_ || position > cache_.size()) {
        throw std::out_of_range("invalid token stream rewind position");
    }
    position_ = position;
}

void TokenStream::ensure(std::size_t index) {
    while (cache_.size() <= index && !end_cached_) {
        auto token = lexer_->next();
        if (token.type == TokenType::End) {
            end_cached_ = true;
        }
        cache_.push_back(std::move(token));
    }
}

} // namespace fred
