#pragma once

#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/lexer/TokenStream.hpp"

#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace fred {

/**
 * @brief Parses one FRED command and its operands into a CommandNode AST.
 *
 * CommandParser is the integration parser above AddressParser and
 * PatternParser. It may consume an optional address prefix, resolves a command
 * mnemonic against CommandRegistry, parses command-specific operands, and
 * returns an owned command AST.
 *
 * @par Ownership and lifetime
 * TokenStream and CommandRegistry are borrowed. Both must outlive the parser.
 * Returned CommandNode trees are exclusively owned by the caller through
 * std::unique_ptr.
 *
 * @par Rollback
 * parse() records the incoming TokenStream position and rewinds to that mark if
 * any exception escapes. This makes command parsing transactional from the
 * caller's point of view.
 *
 * @par Execution boundary
 * CommandParser builds syntax only. It does not execute commands and must not
 * access Buffer/Runtime state.
 *
 * @par Current implementation note
 * The original Step 7.1 registry architecture described in SPEC-008 has since
 * been extended. The registry still validates/resolves mnemonics and provides
 * generic AST constructors, while this parser now contains explicit operand
 * parsing for commands whose syntax requires additional structure.
 *
 * @par Current notable limits
 * - addressed nested commands inside G and ZG are rejected;
 * - Q! external TSS command support is not implemented;
 * - only FB/FO facts forms are implemented;
 * - only OI( and OM option forms are implemented;
 * - J messages are limited to the historical 2000-character maximum;
 * - WB is represented as FileWriteMode::BcdUnsupported.
 */
class CommandParser {
public:
    /**
     * @param tokens Token stream to consume; ownership is not transferred.
     * @param registry Command registry consulted in read-only mode.
     *
     * @pre Both referenced objects must outlive CommandParser.
     */
    CommandParser(TokenStream& tokens,
                  const CommandRegistry& registry) noexcept;

    /**
     * @brief Parses one command and normally requires end-of-input.
     *
     * @return Owned CommandNode AST.
     * @throws ParseError for invalid command syntax, unsupported forms, unknown
     *         mnemonics or unexpected trailing input.
     *
     * @post On any escaping exception, TokenStream position is restored to the
     *       value observed on entry.
     */
    [[nodiscard]] std::unique_ptr<CommandNode> parse();

    /**
     * @brief Parses one command while permitting following commands.
     *
     * Temporarily enables trailing-command mode so callers can parse command
     * chains from a shared TokenStream.
     *
     * @return Owned AST for exactly one command.
     * @throws ParseError with the same rollback behavior as parse().
     *
     * @note The previous trailing-command mode is restored on both success and
     *       failure.
     */
    [[nodiscard]] std::unique_ptr<CommandNode> parse_one();

private:
    /** @return true when token can begin an AddressParser expression. */
    [[nodiscard]] bool begins_address(const Token& token) const noexcept;

    /**
     * @brief Parses `(buffer name)` and reconstructs skipped spaces.
     *
     * @return Non-empty buffer name without parentheses.
     * @throws ParseError for missing opening parenthesis, missing closing
     *         parenthesis or empty name.
     *
     * @note Horizontal whitespace skipped by Lexer is reconstructed as spaces
     *       from SourceLocation column gaps; original tab/CR bytes are therefore
     *       not preserved verbatim.
     */
    [[nodiscard]] std::string parse_parenthesized_buffer_name();

    /**
     * @brief Parses an optional quoted or unquoted filename until line/end.
     *
     * @return std::nullopt when no filename follows; otherwise reconstructed
     *         filename text.
     * @throws ParseError for empty/unterminated quoted filename or trailing text
     *         after a closing quoted filename.
     */
    [[nodiscard]] std::optional<std::string> parse_optional_filename();

    /**
     * @brief Reconstructs and parses a G pattern delimited by `/` or `?`.
     *
     * @return Owned PatternNode tree.
     * @throws ParseError for invalid/unterminated delimiter syntax or any
     *         PatternParser failure.
     *
     * Literal delimiter metadata and escaped-delimiter parity are preserved
     * while rebuilding the pattern source from tokens.
     */
    [[nodiscard]] std::unique_ptr<PatternNode> parse_delimited_pattern();

    /**
     * @brief Parses the pattern and replacement portions of S.
     *
     * @return Pair `{pattern AST, replacement text}`.
     * @throws ParseError for missing symbolic delimiter or unterminated pattern
     *         / replacement.
     *
     * S accepts a one-character Symbol as its delimiter. Literal delimiters do
     * not terminate either portion.
     */
    [[nodiscard]] std::pair<std::unique_ptr<PatternNode>, std::string>
        parse_substitution_parts();

    /**
     * @brief Enforces command termination according to current parser mode.
     *
     * Consumes one trailing NewLine when present. In normal parse() mode the
     * following token must then be End. parse_one() temporarily allows trailing
     * command tokens.
     */
    void require_command_end();

    /** Borrowed token stream. */
    TokenStream* tokens_;

    /** Borrowed read-only command registry. */
    const CommandRegistry* registry_;

    /** Internal mode used by parse_one() for command chaining. */
    bool allow_trailing_commands_{false};
};

} // namespace fred
