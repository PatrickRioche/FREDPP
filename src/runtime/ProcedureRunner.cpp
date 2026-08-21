/**
 * @file ProcedureRunner.cpp
 * @brief Procedure-control orchestration around the shared Flow/Parser/Runtime pipeline.
 *
 * ProcedureRunner handles procedure-only control syntax (buffer calls, labels, jumps, minimal N operations and A/I/C text blocks). Ordinary FRED command lines still follow central Flow expansion -> Lexer -> CommandParser -> CommandExecutor.
 *
 * @note FREDPP_LOT8_CPP_DOCUMENTATION
 */
#include "fred/runtime/ProcedureRunner.hpp"

#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/flow/CommandInputExpansion.hpp"
#include "fred/flow/FlowCharacterStream.hpp"
#include "fred/flow/FlowEngine.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutionError.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include <charconv>
#include <cstdint>
#include <system_error>

#include <cctype>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace fred {
namespace {

/** @brief Returns a non-owning view with procedure-line whitespace trimmed. */
std::string_view trim(std::string_view text) noexcept {
    while (!text.empty() &&
           std::isspace(static_cast<unsigned char>(text.front())) != 0) {
        text.remove_prefix(1);
    }
    while (!text.empty() &&
           std::isspace(static_cast<unsigned char>(text.back())) != 0) {
        text.remove_suffix(1);
    }
    return text;
}

/**
 * @brief Performs an ASCII-oriented case-insensitive prefix comparison.
 *
 * @note This helper is currently unused; Lot 8 documents but deliberately does
 * not remove it because this lot is non-functional.
 */
bool starts_with_ci(std::string_view text, std::string_view prefix) noexcept {
    if (text.size() < prefix.size()) {
        return false;
    }
    for (std::size_t index = 0; index < prefix.size(); ++index) {
        const auto left = static_cast<unsigned char>(text[index]);
        const auto right = static_cast<unsigned char>(prefix[index]);
        if (std::toupper(left) != std::toupper(right)) {
            return false;
        }
    }
    return true;
}

/** @return true when the trimmed procedure line begins with FRED `"`. */
bool is_comment(std::string_view source) noexcept {
    const auto value = trim(source);
    return !value.empty() && value.front() == '"';
}

/**
 * @brief Recognizes and validates standalone procedure `\B(buffer)` control syntax.
 *
 * @return std::nullopt when the line is not a buffer directive.
 */
std::optional<std::string> parse_buffer_directive(std::string_view source) {
    const auto value = trim(source);
    if (value.size() < 3 || value.front() != '\\' ||
        (value[1] != 'B' && value[1] != 'b')) {
        return std::nullopt;
    }

    if (value.size() < 5 || value[2] != '(' || value.back() != ')') {
        throw CommandExecutionError(
            "buffer-flow directive requires \\B(buffer)");
    }

    const auto name = value.substr(3, value.size() - 4);
    if (name.empty()) {
        throw CommandExecutionError("empty buffer name in \\B");
    }
    if (name.find(')') != std::string_view::npos) {
        throw CommandExecutionError(
            "unexpected text after buffer name in \\B");
    }
    return std::string(name);
}

/**
 * @brief Recognizes a trimmed procedure text terminator (`\F` or `\f`).
 */
bool is_text_terminator(std::string_view source) noexcept {
    const auto value = trim(source);
    return value == "\\F" || value == "\\f";
}

} // namespace

namespace {

/**
 * @brief Parsed procedure jump target plus optional required condition state.
 */
struct JumpDirective {
    std::string label;
    std::optional<bool> required_condition;
};

/** @brief Performs ASCII-oriented case-insensitive exact comparison. */
bool equals_ci(std::string_view left, std::string_view right) noexcept {
    if (left.size() != right.size()) {
        return false;
    }

    for (std::size_t index = 0; index < left.size(); ++index) {
        const auto a = static_cast<unsigned char>(left[index]);
        const auto b = static_cast<unsigned char>(right[index]);
        if (std::toupper(a) != std::toupper(b)) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Enforces the currently preserved historical 15-character label limit.
 */
std::string validate_label_name(std::string_view name) {
    if (name.empty()) {
        throw CommandExecutionError("label name must not be empty");
    }
    if (name.size() > 15) {
        throw CommandExecutionError(
            "label name exceeds historical limit of 15 characters");
    }
    if (name.find('(') != std::string_view::npos ||
        name.find(')') != std::string_view::npos) {
        throw CommandExecutionError("invalid parenthesis in label name");
    }
    return std::string(name);
}

/**
 * @brief Parses `@(label)`; non-strict mode is used while scanning jump targets.
 */
std::optional<std::string> parse_label_definition(
    std::string_view source,
    bool strict = true) {
    const auto value = trim(source);
    if (value.empty() || value.front() != '@') {
        return std::nullopt;
    }

    if (value.size() < 4 || value[1] != '(' || value.back() != ')') {
        if (strict) {
            throw CommandExecutionError("label requires @(label)");
        }
        return std::nullopt;
    }

    const auto name = value.substr(2, value.size() - 3);
    if (!strict) {
        if (name.empty() || name.size() > 15 ||
            name.find('(') != std::string_view::npos ||
            name.find(')') != std::string_view::npos) {
            return std::nullopt;
        }
        return std::string(name);
    }

    return validate_label_name(name);
}

/**
 * @brief Parses `J(label)` with the currently implemented optional T/F suffix.
 */
std::optional<JumpDirective> parse_jump_directive(
    std::string_view source) {
    const auto value = trim(source);
    if (value.size() < 2 ||
        (value[0] != 'J' && value[0] != 'j') ||
        value[1] != '(') {
        return std::nullopt;
    }

    const auto closing = value.find(')', 2);
    if (closing == std::string_view::npos) {
        throw CommandExecutionError("J requires J(label)");
    }

    JumpDirective jump;
    jump.label = validate_label_name(
        value.substr(2, closing - 2));

    auto suffix = trim(value.substr(closing + 1));
    if (suffix.empty()) {
        return jump;
    }

    if (suffix.size() != 1) {
        throw CommandExecutionError(
            "J(label) accepts only optional T or F in this lot");
    }

    const char condition = static_cast<char>(std::toupper(
        static_cast<unsigned char>(suffix.front())));
    if (condition == 'T') {
        jump.required_condition = true;
        return jump;
    }
    if (condition == 'F') {
        jump.required_condition = false;
        return jump;
    }

    throw CommandExecutionError(
        "J(label) accepts only optional T or F in this lot");
}

/**
 * @brief Finds a label using forward search followed by wrap-around backward search.
 *
 * The wrap permits historical procedure loops that jump to earlier labels.
 */
std::size_t find_jump_target(
    const std::vector<std::string>& lines,
    std::size_t current_index,
    std::string_view label) {
    // Preserve the existing forward-search behaviour first.
    for (std::size_t cursor = current_index + 1;
         cursor < lines.size();
         ++cursor) {
        const auto candidate =
            parse_label_definition(lines[cursor], false);
        if (candidate && equals_ci(*candidate, label)) {
            return cursor;
        }
    }

    // Historical procedures also use backward jumps to implement loops.
    // If the label was not found after the current instruction, wrap the
    // search to the beginning of the procedure.
    for (std::size_t cursor = 0;
         cursor < current_index;
         ++cursor) {
        const auto candidate =
            parse_label_definition(lines[cursor], false);
        if (candidate && equals_ci(*candidate, label)) {
            return cursor;
        }
    }

    throw CommandExecutionError("? label not found");
}

/**
 * @brief Enforces the historical 14-character numeric-register name limit.
 */
std::string validate_numeric_register_name(
    std::string_view name) {
    if (name.empty()) {
        throw CommandExecutionError(
            "numeric register name must not be empty");
    }
    if (name.size() > 14) {
        throw CommandExecutionError(
            "numeric register name exceeds historical limit of 14 characters");
    }
    if (name.find('(') != std::string_view::npos ||
        name.find(')') != std::string_view::npos) {
        throw CommandExecutionError("invalid numeric register name");
    }
    return std::string(name);
}

/**
 * @brief Parses one signed decimal procedure operand with overflow reporting.
 */
std::int64_t parse_decimal_integer(
    std::string_view source,
    std::size_t& position) {
    const std::size_t original_start = position;
    bool explicit_plus = false;

    if (position < source.size() && source[position] == '+') {
        explicit_plus = true;
        ++position;
    } else if (position < source.size() && source[position] == '-') {
        ++position;
    }

    const std::size_t digits = position;
    while (position < source.size() &&
           std::isdigit(
               static_cast<unsigned char>(source[position])) != 0) {
        ++position;
    }

    if (digits == position) {
        position = original_start;
        throw CommandExecutionError(
            "N requires an integer or one of $, . and #");
    }

    const char* first =
        source.data() + (explicit_plus ? digits : original_start);
    const char* last = source.data() + position;

    std::int64_t result = 0;
    const auto parsed = std::from_chars(first, last, result);

    if (parsed.ec != std::errc{} || parsed.ptr != last) {
        throw CommandExecutionError(
            "numeric value is outside supported integer range");
    }

    return result;
}

/**
 * @brief Resolves the implemented N operand set: integer, `$`, `.`, or `#`.
 */
std::int64_t parse_numeric_operand(
    std::string_view source,
    std::size_t& position,
    const ExecutionContext& context) {
    while (position < source.size() &&
           std::isspace(
               static_cast<unsigned char>(source[position])) != 0) {
        ++position;
    }

    if (position >= source.size()) {
        throw CommandExecutionError("N requires a numeric value");
    }

    const char operand = source[position];

    if (operand == '$') {
        ++position;
        return static_cast<std::int64_t>(
            context.current_buffer().line_count());
    }

    if (operand == '.') {
        ++position;
        return static_cast<std::int64_t>(
            context.current_buffer().current_line());
    }

    if (operand == '#') {
        ++position;
        return context.counter();
    }

    return parse_decimal_integer(source, position);
}

/**
 * @brief Executes the currently implemented `N(register)` operation sequence.
 *
 * Supported operators are `:`, `=`, `<` and `>`. The returned tail is reserved
 * for the currently supported same-line J(label)[T|F] continuation.
 */
std::optional<std::string_view> execute_numeric_sequence(
    std::string_view source,
    ExecutionContext& context) {
    const auto value = trim(source);

    if (value.size() < 2 ||
        (value[0] != 'N' && value[0] != 'n') ||
        value[1] != '(') {
        return std::nullopt;
    }

    const auto closing = value.find(')', 2);
    if (closing == std::string_view::npos) {
        throw CommandExecutionError(
            "N requires a parenthesized register name");
    }

    const std::string register_name =
        validate_numeric_register_name(
            value.substr(2, closing - 2));

    std::size_t position = closing + 1;
    bool executed = false;

    while (true) {
        while (position < value.size() &&
               std::isspace(
                   static_cast<unsigned char>(value[position])) != 0) {
            ++position;
        }

        if (position >= value.size()) {
            break;
        }

        const char operation = value[position];

        if (operation != ':' &&
            operation != '=' &&
            operation != '<' &&
            operation != '>') {
            break;
        }

        ++position;

        const std::int64_t operand =
            parse_numeric_operand(value, position, context);

        if (operation == ':') {
            context.set_numeric_register(register_name, operand);
        } else {
            const std::int64_t current =
                context.numeric_register(register_name);

            if (operation == '=') {
                context.set_condition(current == operand);
            } else if (operation == '<') {
                context.set_condition(current < operand);
            } else {
                context.set_condition(current > operand);
            }
        }

        context.set_counter(
            context.numeric_register(register_name));

        executed = true;
    }

    if (!executed) {
        throw CommandExecutionError(
            "N minimal supports :, =, < and >");
    }

    return trim(value.substr(position));
}

/**
 * @brief Applies a parsed jump only when its optional condition requirement matches.
 */
void apply_jump(
    const JumpDirective& jump,
    const std::vector<std::string>& lines,
    std::size_t& index,
    ExecutionContext& context) {
    const bool take_jump =
        !jump.required_condition.has_value() ||
        context.condition() == *jump.required_condition;

    if (take_jump) {
        index = find_jump_target(lines, index, jump.label);
    }
}

/**
 * @brief Marker exception indicating that procedure context was already printed.
 */
class ReportedProcedureError final : public std::runtime_error {
public:
    explicit ReportedProcedureError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief Emits the current and following procedure lines after an execution failure.
 */
void report_procedure_error_context(
    ExecutionContext& context,
    const std::vector<std::string>& lines,
    std::size_t index) {
    context.output().write_line("procedure stopped; remaining input:");

    std::size_t shown = 0;
    for (std::size_t cursor = index;
         cursor < lines.size() && shown < 3;
         ++cursor, ++shown) {
        context.output().write_line(lines[cursor]);
    }

    if (index + shown < lines.size()) {
        context.output().write_line("...");
    }
}

} // namespace

ProcedureRunner::ProcedureRunner(BufferManager& buffers,
                                 ExecutionContext& context,
                                 const CommandRegistry& registry,
                                 const CommandExecutor& executor,
                                 std::size_t maximum_depth) noexcept
    : buffers_(&buffers),
      context_(&context),
      registry_(&registry),
      executor_(&executor),
      maximum_depth_(maximum_depth) {}

void ProcedureRunner::execute_buffer(std::string_view buffer_name) {
    execute_buffer_impl(buffer_name, 0);
}

void ProcedureRunner::load_and_execute_file(
    const std::string& filename,
    std::string buffer_name) {
    if (buffer_name.empty()) {
        throw CommandExecutionError("script buffer name must not be empty");
    }

    const std::string previous_buffer_name =
        buffers_->current().name();

    auto& buffer = buffers_->create_or_select(std::move(buffer_name));
    if (!buffer.empty() || buffer.modified() || buffer.has_associated_file()) {
        buffers_->select(previous_buffer_name);
        throw CommandExecutionError(
            "script buffer must be empty, unassociated and unmodified");
    }

    const std::string procedure_buffer_name = buffer.name();
    ReadCommandNode read_command(
        nullptr, filename, SourceLocation{});

    try {
        executor_->execute(read_command, *context_);
    } catch (...) {
        buffers_->select(previous_buffer_name);
        throw;
    }

    buffers_->select(previous_buffer_name);
    execute_buffer(procedure_buffer_name);
}

bool ProcedureRunner::execute_buffer_directive(std::string_view source) {
    const auto buffer_name = parse_buffer_directive(source);
    if (!buffer_name) {
        return false;
    }

    if (context_->monitor_commands()) {
        context_->output().write_line(trim(source));
    }
    execute_buffer_impl(*buffer_name, 0);
    return true;
}

/**
 * @brief Executes a snapshot of a procedure Buffer with depth protection.
 *
 * The line vector is copied before execution so edits to Buffer state do not
 * mutate the physical procedure source currently being iterated.
 */
void ProcedureRunner::execute_buffer_impl(std::string_view buffer_name,
                                          std::size_t depth) {
    if (depth >= maximum_depth_) {
        throw CommandExecutionError(
            "maximum procedure buffer-flow depth exceeded");
    }

    if (!buffers_->contains(buffer_name)) {
        throw CommandExecutionError(
            "unknown procedure buffer: " + std::string(buffer_name));
    }

    const std::vector<std::string> lines =
        buffers_->get(buffer_name).lines();

    for (std::size_t index = 0;
         index < lines.size() && !context_->exit_requested();
         ++index) {
        execute_procedure_line(lines, index, depth);
    }
}

/**
 * @brief Expands and dispatches one physical procedure line.
 *
 * Comments bypass expansion; other lines use the central metadata-preserving
 * command-input expansion before procedure-control recognition and normal
 * command parsing.
 */
void ProcedureRunner::execute_procedure_line(
    const std::vector<std::string>& lines,
    std::size_t& index,
    std::size_t depth) {
    const std::string_view source = lines.at(index);
    const auto raw_value = trim(source);
    if (raw_value.empty()) {
        return;
    }

    const ExpandedCommandInput expanded_source =
        is_comment(raw_value)
            ? make_command_input(raw_value)
            : expand_command_input_with_metadata(
                  raw_value, *buffers_, maximum_depth_);
    const auto value = trim(expanded_source.text);

    try {
        if (const auto nested = parse_buffer_directive(value)) {
            if (context_->monitor_commands()) {
                context_->output().write_line(value);
            }
            execute_buffer_impl(*nested, depth + 1);
            return;
        }

        if (const auto label = parse_label_definition(value)) {
            (void)label;
            if (context_->monitor_commands()) {
                context_->output().write_line(value);
            }
            return;
        }

        if (const auto numeric_tail =
                execute_numeric_sequence(value, *context_)) {
            if (context_->monitor_commands()) {
                context_->output().write_line(value);
            }

            if (numeric_tail->empty()) {
                return;
            }

            const auto jump = parse_jump_directive(*numeric_tail);
            if (!jump) {
                throw CommandExecutionError(
                    "Lot 4 allows only J(label)[T|F] after N on the same line");
            }

            apply_jump(*jump, lines, index, *context_);
            return;
        }

        if (const auto jump = parse_jump_directive(value)) {
            if (context_->monitor_commands()) {
                context_->output().write_line(value);
            }

            apply_jump(*jump, lines, index, *context_);
            return;
        }


        execute_single_command(
            expanded_source, &lines, &index);
    } catch (const ReportedProcedureError&) {
        // A nested procedure has already reported its exact source context.
        throw;
    } catch (const std::exception& error) {
        report_procedure_error_context(*context_, lines, index);
        throw ReportedProcedureError(error.what());
    }
}



/**
 * @brief Runs ordinary FRED command syntax through the shared parser/executor path.
 *
 * A/I/C nodes consume subsequent physical procedure lines through the trimmed
 * `\F`/`\f` terminator; all other nodes execute directly.
 */
void ProcedureRunner::execute_single_command(
    const ExpandedCommandInput& source,
    const std::vector<std::string>* lines,
    std::size_t* index) {
    const auto raw_value = trim(source.text);
    if (raw_value.empty()) {
        return;
    }

    if (context_->monitor_commands() && !is_comment(raw_value)) {
        context_->output().write_line(raw_value);
    }

    FlowCharacterStream character_stream(source.characters);
    Lexer lexer(character_stream);
    TokenStream tokens(lexer);
    CommandParser parser(tokens, *registry_);

    while (!tokens.eof() && !context_->exit_requested()) {
        const auto node = parser.parse_one();

        if (node->kind() != AstNodeKind::AppendCommand &&
            node->kind() != AstNodeKind::InsertCommand &&
            node->kind() != AstNodeKind::ChangeCommand) {
            executor_->execute(*node, *context_);
            continue;
        }

        if (lines == nullptr || index == nullptr) {
            throw CommandExecutionError(
                "text input command requires a procedure source");
        }

        std::vector<std::string> text_lines;
        bool terminated = false;

        for (std::size_t cursor = *index + 1;
             cursor < lines->size();
             ++cursor) {
            if (is_text_terminator(lines->at(cursor))) {
                *index = cursor;
                terminated = true;
                break;
            }
            text_lines.push_back(lines->at(cursor));
        }

        if (!terminated) {
            throw CommandExecutionError(
                "end of procedure before \\F; text command cancelled");
        }

        if (node->kind() == AstNodeKind::AppendCommand) {
            executor_->execute_append(
                static_cast<const AppendCommandNode&>(*node),
                *context_, std::move(text_lines));
        } else if (node->kind() == AstNodeKind::InsertCommand) {
            executor_->execute_insert(
                static_cast<const InsertCommandNode&>(*node),
                *context_, std::move(text_lines));
        } else {
            executor_->execute_change(
                static_cast<const ChangeCommandNode&>(*node),
                *context_, std::move(text_lines));
        }
    }
}

} // namespace fred
