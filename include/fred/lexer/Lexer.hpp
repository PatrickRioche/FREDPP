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

/**
 * @brief Converts a CharacterStream into lexical Token objects.
 *
 * Lexer is the lexical-analysis layer between character production and the
 * parser. It recognizes FREDPP's low-level token shapes but does not validate
 * command semantics or access Buffer/Runtime state.
 *
 * Two construction modes are supported:
 * - from std::string_view: Lexer creates and owns a StringCharacterStream;
 * - from CharacterStream&: Lexer borrows an externally owned stream.
 *
 * @par Ownership and lifetime
 * In borrowed-stream mode the referenced CharacterStream must outlive Lexer.
 * Lexer itself is deliberately neither copyable nor movable because it stores
 * an internal raw pointer that can refer to its owned stream.
 *
 * @par Character interpretation
 * Literal punctuation/whitespace is emitted as Symbol rather than being
 * treated as structural syntax. Literal letters, digits and '_' remain
 * eligible to form identifiers/numbers. ForcedSpecial characters retain
 * structural lexical treatment while preserving their interpretation metadata.
 *
 * @par Current lexical limits
 * Classification is ASCII-oriented. Digits are '0'..'9', letters are
 * 'A'..'Z'/'a'..'z', and identifiers continue with ASCII letters, digits or
 * '_'. Printable bytes are defined as ASCII 32..126.
 */
class Lexer {
public:
    /**
     * @brief Creates a lexer owning a copied string-backed character stream.
     *
     * @param source Source text to tokenize. StringCharacterStream copies it,
     *        so the original storage need not outlive Lexer.
     * @param flow_level Flow level attached to all direct source characters.
     */
    explicit Lexer(std::string_view source, std::size_t flow_level = 0);

    /**
     * @brief Creates a lexer borrowing an existing CharacterStream.
     *
     * @param stream Character source. Ownership is not transferred.
     *
     * @pre `stream` must remain alive for the entire lifetime/use of this Lexer.
     */
    explicit Lexer(CharacterStream& stream);

    Lexer(const Lexer&) = delete;
    Lexer& operator=(const Lexer&) = delete;
    Lexer(Lexer&&) = delete;
    Lexer& operator=(Lexer&&) = delete;

    /**
     * @brief Produces the next lexical token.
     *
     * @return Next token. Once the underlying stream reaches EOF, an End token
     *         is returned using CharacterStream::end_location().
     *
     * @par Side effects
     * Advances the underlying CharacterStream past characters belonging to the
     * returned token and skips ordinary horizontal whitespace before lexing.
     *
     * @note Horizontal whitespace currently means Normal ' ', '\t' and '\r'.
     *       Newline is preserved as TokenType::NewLine.
     */
    [[nodiscard]] Token next();

    /**
     * @brief Tokenizes from the current stream position through end of input.
     *
     * @return Vector containing all remaining tokens and exactly one final
     *         TokenType::End token.
     *
     * @note This method consumes the underlying character stream.
     */
    [[nodiscard]] std::vector<Token> tokenize();

private:
    /** @return true when the underlying CharacterStream reports EOF. */
    [[nodiscard]] bool at_end() const noexcept;

    /**
     * @brief Returns a character byte without consuming it.
     *
     * @param lookahead Zero-based lookahead relative to current position.
     * @return Character value, or '\0' when lookahead is beyond EOF.
     */
    [[nodiscard]] char peek(std::size_t lookahead = 0) const noexcept;

    /**
     * @brief Consumes one character.
     *
     * @return Consumed Character. At EOF, returns a synthetic '\0' Character
     *         carrying the stream end location.
     */
    [[nodiscard]] Character advance() noexcept;

    /**
     * @return Current character location, or end_location() at EOF.
     */
    [[nodiscard]] SourceLocation location() const noexcept;

    /**
     * @brief Consumes Normal horizontal whitespace before a token.
     *
     * Literal/ForcedSpecial whitespace is deliberately not skipped.
     */
    void skip_horizontal_whitespace() noexcept;

    /**
     * @brief Constructs a Token value.
     *
     * @param type Lexical category.
     * @param lexeme Owned token text.
     * @param start_location Location of the first token character.
     * @param interpretation Interpretation metadata to preserve.
     * @return Fully constructed Token.
     */
    [[nodiscard]] Token make_token(
        TokenType type,
        std::string lexeme,
        SourceLocation start_location,
        CharacterInterpretation interpretation =
            CharacterInterpretation::Normal) const;

    /**
     * @brief Consumes one or more consecutive ASCII digits.
     *
     * @pre Current character is an ASCII digit.
     * @return TokenType::Number token.
     */
    [[nodiscard]] Token lex_number();

    /**
     * @brief Consumes an identifier or single-letter command candidate.
     *
     * @pre Current character is an ASCII letter or '_'.
     * @return Command for a one-letter ASCII lexeme, otherwise Identifier.
     *
     * @note Command registration/validity is intentionally deferred to the
     *       command parser.
     */
    [[nodiscard]] Token lex_identifier_or_command();

    /**
     * Owned stream used only by the std::string_view constructor.
     * Null when Lexer borrows an external stream.
     */
    std::unique_ptr<CharacterStream> owned_stream_;

    /**
     * Active stream, either owned_stream_.get() or a borrowed external object.
     * Never null after successful construction.
     */
    CharacterStream* stream_{nullptr};
};

} // namespace fred
