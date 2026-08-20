#include "fred/flow/FlowEngine.hpp"
#include "fred/flow/BufferInputSource.hpp"
#include "fred/flow/InputSource.hpp"
#include "fred/core/Limits.hpp"

#include <memory>
#include <stdexcept>
#include <string>

namespace fred {
namespace {

/**
 * InputSource for the outer string passed to expand_input().
 *
 * The source text is copied so the InputSource owns stable storage even when
 * the caller passed a temporary/string_view into unrelated storage.
 */
class StringInputSource final : public InputSource {
public:
    explicit StringInputSource(std::string_view source, std::size_t level)
        : source_(source), level_(level) {}

    [[nodiscard]] std::optional<InputCharacter> next() override {
        if (position_ >= source_.size()) {
            return std::nullopt;
        }
        return InputCharacter{
            source_[position_++], level_, CharacterInterpretation::Normal};
    }

    [[nodiscard]] std::string_view description() const noexcept override {
        return "string-input";
    }

    [[nodiscard]] std::size_t level() const noexcept override {
        return level_;
    }

private:
    std::string source_;
    std::size_t level_;
    std::size_t position_{0};
};

/**
 * Concatenates logical Buffer lines without introducing newline characters.
 *
 * This is used when a nested \S(...) expansion computes another buffer name:
 * the referenced Buffer contributes literal name characters, not line
 * separators.
 */
std::string literal_buffer_text(const Buffer& buffer) {
    std::string text;
    for (const auto& line : buffer.lines()) {
        text += line;
    }
    return text;
}

/** Extracts only character bytes from a metadata-preserving expansion. */
std::string input_characters_text(
    const std::vector<InputCharacter>& characters) {
    std::string text;
    text.reserve(characters.size());
    for (const auto& character : characters) {
        text.push_back(character.value);
    }
    return text;
}

/**
 * Appends Buffer content with Literal interpretation.
 *
 * \S uses emit_newlines=false and therefore concatenates Buffer lines.
 * \L uses emit_newlines=true and preserves logical line boundaries as '\n'.
 * In both cases Literal metadata prevents the injected text from being
 * interpreted as another flow directive during this command-expansion pass.
 */
void append_literal_buffer_text(
    std::vector<InputCharacter>& output,
    const Buffer& buffer,
    std::size_t level,
    bool emit_newlines) {
    for (const auto& line : buffer.lines()) {
        for (const char value : line) {
            output.push_back(InputCharacter{
                value, level, CharacterInterpretation::Literal});
        }
        if (emit_newlines) {
            output.push_back(InputCharacter{
                '\n', level, CharacterInterpretation::Literal});
        }
    }
}

/**
 * Recursive command-input expander.
 *
 * This path is deliberately separate from expand_current_input(): command
 * parsing needs per-character Literal/ForcedSpecial metadata to survive until
 * Lexer/PatternParser. Expansion is centralized here rather than duplicated in
 * individual command parsers.
 */
std::vector<InputCharacter> expand_command_literal_buffer_segment(
    std::string_view source,
    std::size_t& position,
    const BufferManager& buffers,
    std::size_t depth,
    std::size_t maximum_depth,
    bool stop_at_closing_parenthesis) {
    std::vector<InputCharacter> expanded;

    while (position < source.size()) {
        // In a recursive buffer-name segment an unprotected ')' belongs to the
        // directive syntax, not to the resulting name.
        if (stop_at_closing_parenthesis && source[position] == ')') {
            ++position;
            return expanded;
        }

        const bool is_literal_character_directive =
            source[position] == '\\' &&
            position + 1 < source.size() &&
            (source[position + 1] == 'C' ||
             source[position + 1] == 'c');

        if (is_literal_character_directive) {
            if (position + 2 >= source.size()) {
                throw std::runtime_error(
                    "missing character after \\C");
            }

            // FREDPP keeps the ordinary character byte and carries the
            // historical "literal" effect as metadata.
            expanded.push_back(InputCharacter{
                source[position + 2],
                depth,
                CharacterInterpretation::Literal});
            position += 3;
            continue;
        }

        const bool is_forced_special_character_directive =
            source[position] == '\\' &&
            position + 1 < source.size() &&
            (source[position + 1] == 'O' ||
             source[position + 1] == 'o');

        if (is_forced_special_character_directive) {
            if (position + 2 >= source.size()) {
                throw std::runtime_error(
                    "missing character after \\O");
            }

            expanded.push_back(InputCharacter{
                source[position + 2],
                depth,
                CharacterInterpretation::ForcedSpecial});
            position += 3;
            continue;
        }

        if (source[position] == '\\' &&
            position + 1 < source.size() &&
            source[position + 1] == '\\') {
            // Command-input expansion preserves the two source backslashes.
            // Full-flow expand_current_input() has different escape behavior
            // and collapses "\\" to one output backslash.
            expanded.push_back(InputCharacter{
                '\\', depth, CharacterInterpretation::Normal});
            expanded.push_back(InputCharacter{
                '\\', depth, CharacterInterpretation::Normal});
            position += 2;
            continue;
        }

        const bool is_literal_buffer_directive =
            source[position] == '\\' &&
            position + 2 < source.size() &&
            (source[position + 1] == 'S' ||
             source[position + 1] == 's' ||
             source[position + 1] == 'L' ||
             source[position + 1] == 'l') &&
            source[position + 2] == '(';

        // Unknown/non-command-input directives remain ordinary source text.
        if (!is_literal_buffer_directive) {
            expanded.push_back(InputCharacter{
                source[position++],
                depth,
                CharacterInterpretation::Normal});
            continue;
        }

        if (depth >= maximum_depth) {
            throw std::runtime_error(
                "maximum flow expansion depth exceeded (" +
                std::to_string(maximum_depth) + ")");
        }

        const char directive = source[position + 1];
        const bool emit_newlines =
            directive == 'L' || directive == 'l';

        position += 3; // skip \S( or \L(

        // The buffer name itself is subject to the same central expansion.
        // This enables forms such as \S(\S(nombuf)).
        const auto buffer_name_characters =
            expand_command_literal_buffer_segment(
                source, position, buffers,
                depth + 1, maximum_depth, true);
        const std::string buffer_name =
            input_characters_text(buffer_name_characters);

        if (buffer_name.empty()) {
            throw std::runtime_error(
                std::string("empty buffer name in \\") + directive);
        }
        if (buffer_name.size() > limits::max_buffer_name_length) {
            throw std::runtime_error(
                "buffer name exceeds FREDPP limit of " +
                std::to_string(limits::max_buffer_name_length));
        }

        // \S and \L both inject characters literally.
        // \S suppresses <nl>; \L preserves them.
        append_literal_buffer_text(
            expanded,
            buffers.get(buffer_name),
            depth + 1,
            emit_newlines);
    }

    if (stop_at_closing_parenthesis) {
        throw std::runtime_error(
            "unterminated literal buffer directive in command input");
    }

    return expanded;
}

} // namespace

FlowEngine::FlowEngine(
    const BufferManager& buffers,
    std::size_t maximum_depth)
    : buffers_(&buffers), input_(maximum_depth) {}

std::string FlowEngine::expand_buffer(std::string_view buffer_name) {
    // Full-flow operations share input_; overlapping/reentrant execution would
    // corrupt stack semantics, so reject it explicitly.
    if (!input_.empty()) {
        throw std::logic_error("flow engine is already executing");
    }

    push_buffer(buffer_name, 0);
    return expand_current_input();
}

std::string FlowEngine::expand_input(std::string_view source) {
    if (!input_.empty()) {
        throw std::logic_error("flow engine is already executing");
    }

    input_.push(std::make_unique<StringInputSource>(source, 0));
    return expand_current_input();
}

std::string FlowEngine::expand_command_input(std::string_view source) {
    return input_characters_text(
        expand_command_input_characters(source));
}

std::vector<InputCharacter>
FlowEngine::expand_command_input_characters(std::string_view source) {
    if (!input_.empty()) {
        throw std::logic_error("flow engine is already executing");
    }

    // This helper operates directly on source positions instead of input_ so
    // it can preserve lexical interpretation metadata in its result.
    std::size_t position = 0;
    return expand_command_literal_buffer_segment(
        source, position, *buffers_, 0, input_.maximum_depth(), false);
}

std::string FlowEngine::expand_current_input() {
    std::string output;

    while (auto character = read_raw()) {
        // Literal injected characters bypass directive recognition entirely.
        if (character->interpretation == CharacterInterpretation::Literal ||
            character->value != '\\') {
            output.push_back(character->value);
            continue;
        }

        auto directive = read_raw();
        if (!directive) {
            // A trailing raw backslash is preserved.
            output.push_back('\\');
            break;
        }

        // A directive introducer cannot consume its directive letter from a
        // different nested input level. Preserve both bytes instead.
        if (directive->level != character->level) {
            output.push_back('\\');
            output.push_back(directive->value);
            continue;
        }

        if (directive->value == '\\') {
            output.push_back('\\');
            continue;
        }

        if (directive->value == 'C' ||
            directive->value == 'c') {
            const auto literal = read_raw();
            if (!literal) {
                throw std::runtime_error(
                    "missing character after \\C");
            }
            if (literal->level != character->level) {
                throw std::runtime_error(
                    "literal character after \\C crossed an input level");
            }

            // In text-only full-flow output, consuming the directive and
            // emitting the following byte directly is enough to protect it
            // from this expansion pass.
            output.push_back(literal->value);
            continue;
        }

        if (directive->value == 'O' ||
            directive->value == 'o') {
            const auto special = read_raw();
            if (!special) {
                throw std::runtime_error(
                    "missing character after \\O");
            }
            if (special->level != character->level) {
                throw std::runtime_error(
                    "special character after \\O crossed an input level");
            }

            output.push_back(special->value);
            continue;
        }

        if (directive->value == 'B' || directive->value == 'b') {
            const auto name =
                parse_buffer_name(character->level, 'B');

            // \B is active flow injection: Normal interpretation and generated
            // newlines mean directives inside the injected Buffer may execute.
            push_buffer(name, character->level + 1);
            continue;
        }

        if (directive->value == 'S' || directive->value == 's') {
            const auto name =
                parse_buffer_name(character->level, 'S');

            // \S is literal substitution and suppresses line boundaries.
            push_buffer(name,
                        character->level + 1,
                        false,
                        CharacterInterpretation::Literal);
            continue;
        }

        if (directive->value == 'L' || directive->value == 'l') {
            const auto name =
                parse_buffer_name(character->level, 'L');

            // \L is literal substitution while preserving Buffer line
            // boundaries as generated newline characters.
            push_buffer(name,
                        character->level + 1,
                        true,
                        CharacterInterpretation::Literal);
            continue;
        }

        // Unknown flow directives are preserved rather than silently consumed.
        output.push_back('\\');
        output.push_back(directive->value);
    }

    return output;
}

std::optional<InputCharacter> FlowEngine::read_raw() {
    return input_.next();
}

std::string FlowEngine::parse_buffer_name(
    std::size_t directive_level,
    char directive,
    std::size_t nesting_depth) {
    const std::string prefix = std::string("\\") + directive;
    const auto opening = read_raw();
    if (!opening ||
        opening->level != directive_level ||
        opening->value != '(') {
        throw std::runtime_error("expected '(' after " + prefix);
    }

    std::string name;
    while (auto character = read_raw()) {
        // Parentheses/name syntax may not be completed by a character injected
        // from a different flow level.
        if (character->level != directive_level) {
            throw std::runtime_error("buffer name crossed an input level");
        }

        if (character->value == '\\') {
            const auto nested_directive = read_raw();
            if (!nested_directive ||
                nested_directive->level != directive_level) {
                throw std::runtime_error(
                    "buffer name crossed an input level");
            }

            if (nested_directive->value == 'C' ||
                nested_directive->value == 'c') {
                const auto literal = read_raw();
                if (!literal ||
                    literal->level != directive_level) {
                    throw std::runtime_error(
                        "literal character after \\C crossed an input level");
                }

                // This is how a normally structural ')' can become part of the
                // actual Buffer name.
                name.push_back(literal->value);
                continue;
            }

            if (nested_directive->value == 'S' ||
                nested_directive->value == 's') {
                if (nesting_depth >= input_.maximum_depth()) {
                    throw std::runtime_error(
                        "maximum flow expansion depth exceeded (" +
                        std::to_string(input_.maximum_depth()) + ")");
                }

                const auto nested_name =
                    parse_buffer_name(
                        directive_level, 'S', nesting_depth + 1);

                if (nested_name.size() >
                    limits::max_buffer_name_length) {
                    throw std::runtime_error(
                        "buffer name exceeds FREDPP limit of " +
                        std::to_string(
                            limits::max_buffer_name_length));
                }

                // Nested \S used in a name concatenates referenced logical
                // lines; it does not inject newline separators into the name.
                name += literal_buffer_text(
                    buffers_->get(nested_name));
                continue;
            }

            // Unknown nested escape is retained as literal name text.
            name.push_back('\\');
            name.push_back(nested_directive->value);
            continue;
        }

        if (character->value == ')') {
            if (name.empty()) {
                throw std::runtime_error(
                    "empty buffer name in " + prefix);
            }
            if (name.size() > limits::max_buffer_name_length) {
                throw std::runtime_error(
                    "buffer name exceeds FREDPP limit of " +
                    std::to_string(limits::max_buffer_name_length));
            }
            return name;
        }

        if (character->value == '\n') {
            throw std::runtime_error(
                "unterminated buffer name in " + prefix);
        }

        name.push_back(character->value);
    }

    throw std::runtime_error(
        "unterminated buffer name in " + prefix);
}

void FlowEngine::push_buffer(
    std::string_view buffer_name,
    std::size_t level,
    bool emit_newlines,
    CharacterInterpretation interpretation) {
    const auto& buffer = buffers_->get(buffer_name);
    input_.push(std::make_unique<BufferInputSource>(
        buffer, level, emit_newlines, interpretation));
}

} // namespace fred
