#pragma once

#include "fred/core/BufferManager.hpp"
#include "fred/flow/InputCharacter.hpp"
#include "fred/flow/InputStack.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace fred {

/**
 * @brief Expands FRED flow directives over strings and Buffer content.
 *
 * FlowEngine is the central implementation of flow/substitution semantics.
 * It borrows BufferManager for named-buffer lookup and uses InputStack for
 * nested full-flow input.
 *
 * @par Architectural rule: expansion before parsing
 * When historical FRED syntax allows flow directives inside command arguments,
 * those directives must be expanded centrally before Lexer/CommandParser sees
 * the command. Callers should use CommandInputExpansion helpers rather than add
 * command-specific substitution logic.
 *
 * Example:
 *
 * @code
 * // B(nombuf) contains: doc1
 * B(\S(nombuf))  ->  B(doc1)
 * @endcode
 *
 * The same expansion stage applies independently of which command later parses
 * the resulting argument.
 *
 * @par Full-flow APIs
 * expand_buffer() and expand_input() execute the general nested flow stream,
 * including active `\B`, literal `\S`/`\L`, `\C`, `\O` and escaped backslash
 * handling.
 *
 * @par Command-input APIs
 * expand_command_input_characters() preserves CharacterInterpretation metadata
 * needed by Lexer/PatternParser. `\S`/`\L` inserted bytes are Literal, `\C`
 * marks the following byte Literal and `\O` marks it ForcedSpecial.
 *
 * @par Historical representation vs FREDPP
 * Historical FRED implementations could encode flow-control effects using
 * machine character values. FREDPP keeps ordinary text bytes and carries the
 * semantic effect separately through CharacterInterpretation metadata.
 *
 * @par Reentrancy/lifetime
 * The BufferManager is borrowed and must outlive FlowEngine. A FlowEngine does
 * not support overlapping expansion operations.
 */
class FlowEngine {
public:
    /**
     * @param buffers Buffer namespace used by flow directives.
     * @param maximum_depth Recursive/nested input protection.
     *
     * @throws std::invalid_argument indirectly when maximum_depth is zero.
     * @pre `buffers` must outlive this FlowEngine.
     */
    explicit FlowEngine(const BufferManager& buffers,
                        std::size_t maximum_depth = 256);

    /**
     * @brief Expands one named Buffer using the full flow engine.
     *
     * @param buffer_name Existing Buffer name.
     * @return Expanded text.
     * @throws std::out_of_range for an unknown referenced Buffer.
     * @throws std::runtime_error for malformed directives/depth overflow.
     * @throws std::logic_error if this engine already has active input.
     *
     * @note Normal buffer injection emits a newline after every logical line.
     */
    [[nodiscard]] std::string
    expand_buffer(std::string_view buffer_name);

    /**
     * @brief Expands arbitrary text using the full flow engine.
     *
     * @param source Source text copied into an internal StringInputSource.
     * @return Expanded text.
     * @throws std::runtime_error for malformed directives/depth overflow.
     * @throws std::logic_error if this engine already has active input.
     */
    [[nodiscard]] std::string expand_input(std::string_view source);

    /**
     * @brief Expands command-input directives and returns text only.
     *
     * Equivalent to taking `.value` from
     * expand_command_input_characters().
     *
     * @warning This overload discards CharacterInterpretation metadata. Parser
     *          pipelines that need literal/forced-special semantics should use
     *          expand_command_input_characters() or
     *          expand_command_input_with_metadata().
     */
    [[nodiscard]] std::string
    expand_command_input(std::string_view source);

    /**
     * @brief Performs central command-input expansion with metadata.
     *
     * @param source Raw command text before lexing/parsing.
     * @return Expanded characters carrying value, flow level and
     *         CharacterInterpretation.
     *
     * @throws std::out_of_range for unknown referenced buffers.
     * @throws std::runtime_error for malformed `\S`/`\L`/`\C`/`\O`,
     *         overlong names or expansion-depth overflow.
     * @throws std::logic_error when a full-flow input stack is already active.
     *
     * @par Current command-input directives
     * - `\S(name)` injects all Buffer lines concatenated, as Literal;
     * - `\L(name)` injects Buffer lines with generated newlines, as Literal;
     * - `\C x` (without the space in source syntax) removes the directive and
     *   marks the following byte Literal;
     * - `\O x` removes the directive and marks the following byte ForcedSpecial;
     * - nested `\S`/`\L` may compute buffer names;
     * - inserted Literal bytes are not expanded a second time.
     *
     * @note Unknown directives are left as ordinary source text by the current
     *       command-input expansion helper.
     */
    [[nodiscard]] std::vector<InputCharacter>
    expand_command_input_characters(std::string_view source);

private:
    /**
     * @brief Drains input_ while interpreting full-flow directives.
     *
     * @return Expanded text.
     *
     * @warning If an exception escapes while full-flow sources remain stacked,
     *          the current implementation does not reset input_. Reusing that
     *          same FlowEngine instance may then report "already executing".
     *          High-level command helpers avoid this by constructing a fresh
     *          FlowEngine per expansion.
     */
    [[nodiscard]] std::string expand_current_input();

    /** @return Next raw character from the nested InputStack. */
    [[nodiscard]] std::optional<InputCharacter> read_raw();

    /**
     * @brief Parses a parenthesized buffer name for a full-flow directive.
     *
     * Nested `\S(...)` is expanded while computing the name; `\C` can protect
     * a character such as `)` so it becomes part of the name.
     *
     * @param directive_level Level at which the name syntax must remain.
     * @param directive Directive letter used in error messages.
     * @param nesting_depth Nested name-expansion depth.
     * @return Non-empty buffer name not exceeding the FREDPP limit.
     */
    [[nodiscard]] std::string
    parse_buffer_name(std::size_t directive_level,
                      char directive,
                      std::size_t nesting_depth = 0);

    /**
     * @brief Pushes a Buffer as a new nested InputSource.
     *
     * @param buffer_name Existing Buffer name.
     * @param level Flow level attached to its characters.
     * @param emit_newlines Whether to synthesize newline after each line.
     * @param interpretation Metadata assigned to all injected characters.
     */
    void push_buffer(
        std::string_view buffer_name,
        std::size_t level,
        bool emit_newlines = true,
        CharacterInterpretation interpretation =
            CharacterInterpretation::Normal);

    /** Borrowed read-only buffer namespace. */
    const BufferManager* buffers_;

    /** Owned stack used by full-flow expand_input()/expand_buffer(). */
    InputStack input_;
};

} // namespace fred
