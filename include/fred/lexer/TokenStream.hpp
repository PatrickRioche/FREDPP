#pragma once

#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/Token.hpp"

#include <cstddef>
#include <vector>

namespace fred {

/**
 * @brief Lazy, rewindable token buffer layered over Lexer.
 *
 * TokenStream provides parser-friendly lookahead without forcing the entire
 * input to be tokenized up front. Tokens requested by peek()/consume() are
 * cached and remain available for backward rewind.
 *
 * @par Ownership and lifetime
 * TokenStream borrows Lexer through a raw pointer. The Lexer must outlive the
 * TokenStream. Cached Token values are owned by TokenStream.
 *
 * @par Rewind model
 * Unlike CharacterStream, TokenStream rewind is intentionally backward-only:
 * a caller may return to an already visited position but may not use rewind()
 * to move the cursor forward.
 *
 * @par End-of-input stability
 * The first End token is cached permanently. Consuming End never advances the
 * cursor, and arbitrarily large lookahead past EOF returns that same cached
 * End token.
 */
class TokenStream {
public:
    /**
     * @brief Creates a token stream over an existing Lexer.
     *
     * @param lexer Lexer to consume lazily. Ownership is not transferred.
     *
     * @pre `lexer` must outlive this TokenStream.
     */
    explicit TokenStream(Lexer& lexer);

    /**
     * @brief Observes a token without advancing the stream.
     *
     * @param lookahead Token offset from the current cursor; 0 is current.
     * @return Reference to a cached Token.
     *
     * @note The returned reference remains valid while operations do not cause
     *       cache_ reallocation. Callers should therefore treat it as a
     *       short-lived observation and not persist it across further
     *       token-producing lookahead/consume operations.
     *
     * @note Lookahead beyond EOF returns the cached End token.
     */
    [[nodiscard]] const Token& peek(std::size_t lookahead = 0);

    /**
     * @brief Returns the current token and advances when it is not End.
     *
     * @return Token value copied from the cache.
     *
     * @post position() increases by one for non-End tokens.
     * @post position() is unchanged for TokenType::End.
     */
    [[nodiscard]] Token consume();

    /**
     * @brief Tests whether the current token is End.
     *
     * @return true when peek().type == TokenType::End.
     *
     * @note This may ask the underlying Lexer for a token in order to populate
     *       the cache.
     */
    [[nodiscard]] bool eof();

    /**
     * @return Current zero-based token cursor.
     */
    [[nodiscard]] std::size_t position() const noexcept;

    /**
     * @brief Rewinds to a previously reached token position.
     *
     * @param position Target cursor.
     *
     * @throws std::out_of_range if `position` is greater than the current
     *         cursor or greater than the number of cached tokens.
     *
     * @note This API cannot be used to seek forward.
     */
    void rewind(std::size_t position);

private:
    /**
     * @brief Ensures that cache_ contains the requested token index or End.
     *
     * @param index Absolute zero-based token index.
     *
     * Repeatedly calls Lexer::next() until the requested index exists or an
     * End token has been cached.
     */
    void ensure(std::size_t index);

    /** Borrowed lexer; never owned by TokenStream. */
    Lexer* lexer_;

    /** Lazily populated token cache used for lookahead and rewind. */
    std::vector<Token> cache_;

    /** Current zero-based token cursor. */
    std::size_t position_{0};

    /** true after the first TokenType::End has been cached. */
    bool end_cached_{false};
};

} // namespace fred
