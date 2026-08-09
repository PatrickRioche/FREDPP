#include "fred/runtime/ProcedureRunner.hpp"

#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/flow/FlowEngine.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutionError.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"

#include <cctype>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace fred {
namespace {

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

bool is_comment(std::string_view source) noexcept {
    const auto value = trim(source);
    return !value.empty() && value.front() == '"';
}

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

bool is_text_terminator(std::string_view source) noexcept {
    const auto value = trim(source);
    return value == "\\F" || value == "\\f";
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

void ProcedureRunner::load_and_execute_file(const std::string& filename,
                                            std::string buffer_name) {
    if (buffer_name.empty()) {
        throw CommandExecutionError("script buffer name must not be empty");
    }

    auto& buffer = buffers_->create_or_select(std::move(buffer_name));
    if (!buffer.empty() || buffer.modified() || buffer.has_associated_file()) {
        throw CommandExecutionError(
            "script buffer must be empty, unassociated and unmodified");
    }

    ReadCommandNode read_command(filename, SourceLocation{});
    executor_->execute(read_command, *context_);
    execute_buffer(buffer.name());
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

void ProcedureRunner::execute_procedure_line(
    const std::vector<std::string>& lines,
    std::size_t& index,
    std::size_t depth) {
    const std::string_view source = lines.at(index);
    const auto value = trim(source);
    if (value.empty()) {
        return;
    }

    if (const auto nested = parse_buffer_directive(value)) {
        if (context_->monitor_commands()) {
            context_->output().write_line(value);
        }
        execute_buffer_impl(*nested, depth + 1);
        return;
    }

    if (execute_message_sequence(source, &lines, &index)) {
        return;
    }

    execute_single_command(source, &lines, &index);
}

bool ProcedureRunner::execute_message_sequence(
    std::string_view source,
    const std::vector<std::string>* lines,
    std::size_t* index) {
    const auto value = trim(source);
    if (!(starts_with_ci(value, "JM/") || starts_with_ci(value, "JP/"))) {
        return false;
    }

    std::size_t position = 0;
    bool executed = false;

    while (position < value.size()) {
        while (position < value.size() &&
               std::isspace(static_cast<unsigned char>(value[position])) != 0) {
            ++position;
        }
        if (position == value.size()) {
            break;
        }

        const auto rest = value.substr(position);
        if (!(starts_with_ci(rest, "JM/") || starts_with_ci(rest, "JP/"))) {
            throw CommandExecutionError(
                "only JM/JP sequences are supported on one procedure line");
        }

        const std::size_t closing = value.find('/', position + 3);
        if (closing == std::string_view::npos) {
            throw CommandExecutionError("unterminated J message");
        }

        const bool newline =
            std::toupper(static_cast<unsigned char>(value[position + 1])) == 'M';
        const auto message =
            value.substr(position + 3, closing - (position + 3));

        FlowEngine flow(*buffers_, maximum_depth_);
        const std::string expanded = flow.expand_input(message);

        if (context_->monitor_commands()) {
            context_->output().write_line(
                value.substr(position, closing - position + 1));
        }
        if (newline) {
            context_->output().write_line(expanded);
        } else {
            context_->output().write(expanded);
        }

        executed = true;
        position = closing + 1;

        if (context_->exit_requested()) {
            break;
        }
    }

    return executed;
}

void ProcedureRunner::execute_single_command(
    std::string_view source,
    const std::vector<std::string>* lines,
    std::size_t* index) {
    const auto value = trim(source);
    if (value.empty()) {
        return;
    }

    if (context_->monitor_commands() && !is_comment(value)) {
        context_->output().write_line(value);
    }

    Lexer lexer(value);
    TokenStream tokens(lexer);
    CommandParser parser(tokens, *registry_);
    const auto node = parser.parse();

    if (node->kind() != AstNodeKind::AppendCommand &&
        node->kind() != AstNodeKind::InsertCommand &&
        node->kind() != AstNodeKind::ChangeCommand) {
        executor_->execute(*node, *context_);
        return;
    }

    if (lines == nullptr || index == nullptr) {
        throw CommandExecutionError(
            "text input command requires a procedure source");
    }

    std::vector<std::string> text_lines;
    bool terminated = false;
    for (std::size_t cursor = *index + 1; cursor < lines->size(); ++cursor) {
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

} // namespace fred
