#include "fred/runtime/CommandExecutor.hpp"

#include "fred/ast/AbsoluteAddressNode.hpp"

#include "fred/runtime/AddressEvaluator.hpp"
#include "fred/runtime/CommandExecutionError.hpp"
#include "fred/runtime/PatternMatcher.hpp"

#include <fstream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace fred {
namespace {

void execute_print(const PrintCommandNode& command, ExecutionContext& context) {
    auto& buffer = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), buffer);
    for (auto line = range.first; line <= range.last; ++line) {
        context.output().write_line(buffer.line(line));
    }
    buffer.set_current_line(range.last);
}

void execute_delete(const DeleteCommandNode& command, ExecutionContext& context) {
    auto& buffer = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), buffer);
    buffer.erase(range.first, range.last);
}

void execute_list(const ListCommandNode& command, ExecutionContext& context) {
    if (!command.filename()) {
        throw CommandExecutionError(
            "L without a filename requires current-file support (not implemented yet)");
    }

    std::ifstream input(*command.filename(), std::ios::binary);
    if (!input) {
        throw CommandExecutionError("cannot open file: " + *command.filename());
    }

    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        context.output().write_line(line);
    }

    if (input.bad()) {
        throw CommandExecutionError("error while reading file: " + *command.filename());
    }
}

Buffer::LineNumber evaluate_append_address(const AppendCommandNode& command,
                                           const Buffer& buffer) {
    const auto* address = command.address();
    if (address == nullptr) {
        return buffer.line_count(); // Historical default: $.
    }
    if (address->kind() == AstNodeKind::RangeAddress) {
        throw CommandExecutionError("A accepts at most one line address");
    }
    if (address->kind() == AstNodeKind::AbsoluteAddress &&
        static_cast<const AbsoluteAddressNode&>(*address).line() == 0) {
        return 0;
    }
    return AddressEvaluator{}.evaluate(address, buffer).first;
}

Buffer::LineNumber evaluate_insert_position(const InsertCommandNode& command,
                                            const Buffer& buffer) {
    const auto* address = command.address();
    if (address == nullptr) {
        if (buffer.empty()) {
            return 0;
        }
        return buffer.current_line() - 1;
    }
    if (address->kind() == AstNodeKind::RangeAddress) {
        throw CommandExecutionError("I accepts at most one line address");
    }
    if (address->kind() == AstNodeKind::AbsoluteAddress &&
        static_cast<const AbsoluteAddressNode&>(*address).line() == 0) {
        return 0;
    }
    return AddressEvaluator{}.evaluate(address, buffer).first - 1;
}

Buffer::LineNumber evaluate_move_destination(const MoveCommandNode& command,
                                             const Buffer& buffer) {
    const auto* destination = command.destination();
    if (destination == nullptr) {
        throw CommandExecutionError("M requires a destination address");
    }
    if (destination->kind() == AstNodeKind::RangeAddress) {
        throw CommandExecutionError("M destination must be a single line address");
    }
    if (destination->kind() == AstNodeKind::AbsoluteAddress &&
        static_cast<const AbsoluteAddressNode&>(*destination).line() == 0) {
        return 0;
    }
    return AddressEvaluator{}.evaluate(destination, buffer).first;
}

Buffer::LineNumber evaluate_transfer_destination(
    const TransferCommandNode& command, const Buffer& buffer) {
    const auto* destination = command.destination();
    if (destination == nullptr) {
        throw CommandExecutionError("T requires a destination address");
    }
    if (destination->kind() == AstNodeKind::RangeAddress) {
        throw CommandExecutionError("T destination must be a single line address");
    }
    if (destination->kind() == AstNodeKind::AbsoluteAddress &&
        static_cast<const AbsoluteAddressNode&>(*destination).line() == 0) {
        return 0;
    }
    return AddressEvaluator{}.evaluate(destination, buffer).first;
}

void execute_move(const MoveCommandNode& command, ExecutionContext& context) {
    auto& buffer = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), buffer);
    auto destination = evaluate_move_destination(command, buffer);

    if (destination >= range.first && destination <= range.last) {
        throw CommandExecutionError("M destination lies inside the moved range");
    }

    std::vector<std::string> moved_lines;
    moved_lines.reserve(range.last - range.first + 1);
    for (auto line = range.first; line <= range.last; ++line) {
        moved_lines.push_back(buffer.line(line));
    }

    const auto moved_count = range.last - range.first + 1;
    buffer.erase(range.first, range.last);
    if (destination > range.last) {
        destination -= moved_count;
    }
    buffer.insert_after(destination, std::move(moved_lines));
}

void execute_transfer(const TransferCommandNode& command,
                      ExecutionContext& context) {
    auto& buffer = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), buffer);
    const auto destination = evaluate_transfer_destination(command, buffer);

    std::vector<std::string> copied_lines;
    copied_lines.reserve(range.last - range.first + 1);
    for (auto line = range.first; line <= range.last; ++line) {
        copied_lines.push_back(buffer.line(line));
    }

    buffer.insert_after(destination, std::move(copied_lines));
}



std::string expand_substitution_replacement(std::string_view replacement,
                                            std::string_view matched_text) {
    std::string result;
    for (std::size_t index = 0; index < replacement.size(); ++index) {
        const char value = replacement[index];
        if (value == '\\' && index + 1 < replacement.size()) {
            result.push_back(replacement[++index]);
        } else if (value == '&') {
            result.append(matched_text);
        } else {
            result.push_back(value);
        }
    }
    return result;
}

std::optional<std::string> substitute_all(const PatternNode& pattern,
                                          std::string_view replacement,
                                          std::string_view text) {
    PatternMatcher matcher;
    std::string result;
    std::size_t copied_until = 0;
    std::size_t search_from = 0;
    bool matched = false;

    while (search_from <= text.size()) {
        const auto match = matcher.find(pattern, text, search_from);
        if (!match) {
            break;
        }

        matched = true;
        result.append(text.substr(copied_until, match->start - copied_until));
        result += expand_substitution_replacement(
            replacement, text.substr(match->start, match->end - match->start));

        if (match->end > match->start) {
            copied_until = match->end;
            if (match->end == text.size()) {
                search_from = text.size() + 1;
                break;
            }
            search_from = match->end;
            continue;
        }

        // A zero-length match must still advance through the input to avoid an
        // infinite loop while retaining the character at the match position.
        copied_until = match->start;
        if (match->start == text.size()) {
            search_from = text.size() + 1;
            break;
        }
        result.push_back(text[match->start]);
        copied_until = match->start + 1;
        search_from = copied_until;
    }

    if (!matched) {
        return std::nullopt;
    }
    result.append(text.substr(copied_until));
    return result;
}

void execute_substitute(const SubstituteCommandNode& command,
                        ExecutionContext& context) {
    auto& buffer = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), buffer);
    bool changed = false;
    Buffer::LineNumber last_changed = 0;

    for (auto line = range.first; line <= range.last; ++line) {
        const std::string original = buffer.line(line);
        const auto replacement = substitute_all(
            command.pattern(), command.replacement(), original);
        if (!replacement) {
            continue;
        }
        buffer.replace(line, *replacement);
        changed = true;
        last_changed = line;
    }

    context.set_condition(changed);
    if (!changed) {
        throw CommandExecutionError("no text changed");
    }

    buffer.set_current_line(last_changed);
    if (command.print_after()) {
        context.output().write_line(buffer.line(last_changed));
    }
}

void execute_quit(const QuitCommandNode& command, ExecutionContext& context) {
    context.request_exit(command.immediate());
}

void execute_zap(const ZapCommandNode& command, ExecutionContext& context) {
    auto& buffer = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), buffer);
    if (range.first != range.last) {
        throw CommandExecutionError("Z accepts at most one line address");
    }
    buffer.set_current_line(range.first);
}

LineRange evaluate_global_range(const GlobalCommandNode& command,
                                const Buffer& buffer) {
    if (buffer.empty()) {
        throw CommandExecutionError("current buffer is empty");
    }
    if (command.address() == nullptr) {
        return {1, buffer.line_count()};
    }
    return AddressEvaluator{}.evaluate(command.address(), buffer);
}

void execute_global(const GlobalCommandNode& command,
                    ExecutionContext& context) {
    auto& buffer = context.current_buffer();
    auto range = evaluate_global_range(command, buffer);
    const auto& nested = command.nested_command();

    if (nested.has_address()) {
        throw CommandExecutionError(
            "addressed commands inside G are not supported yet");
    }
    if (nested.kind() != AstNodeKind::PrintCommand &&
        nested.kind() != AstNodeKind::DeleteCommand &&
        nested.kind() != AstNodeKind::ZapCommand &&
        nested.kind() != AstNodeKind::SubstituteCommand) {
        throw CommandExecutionError(
            "G currently supports nested P, D, Z and S commands");
    }

    PatternMatcher matcher;
    std::size_t selected_count = 0;
    auto line = range.first;
    auto last = range.last;

    while (line <= last && !buffer.empty()) {
        const bool pattern_found = matcher.search(command.pattern(), buffer.line(line));
        const bool selected = command.inverted() ? !pattern_found : pattern_found;
        if (!selected) {
            ++line;
            continue;
        }

        ++selected_count;
        buffer.set_current_line(line);

        if (nested.kind() == AstNodeKind::PrintCommand) {
            context.output().write_line(buffer.line(line));
            ++line;
            continue;
        }
        if (nested.kind() == AstNodeKind::ZapCommand) {
            ++line;
            continue;
        }
        if (nested.kind() == AstNodeKind::SubstituteCommand) {
            execute_substitute(
                static_cast<const SubstituteCommandNode&>(nested), context);
            ++line;
            continue;
        }

        buffer.erase(line, line);
        --last;
        if (line > buffer.line_count()) {
            break;
        }
    }

    context.set_counter(selected_count);
}

void execute_buffer(const BufferCommandNode& command, ExecutionContext& context) {
    try {
        (void)context.buffers().create_or_select(command.buffer_name());
    } catch (const std::exception& error) {
        throw CommandExecutionError(error.what());
    }
}

} // namespace

void CommandExecutor::execute(const CommandNode& command,
                              ExecutionContext& context) const {
    switch (command.kind()) {
    case AstNodeKind::PrintCommand:
        execute_print(static_cast<const PrintCommandNode&>(command), context);
        return;
    case AstNodeKind::ListCommand:
        execute_list(static_cast<const ListCommandNode&>(command), context);
        return;
    case AstNodeKind::DeleteCommand:
        execute_delete(static_cast<const DeleteCommandNode&>(command), context);
        return;
    case AstNodeKind::AppendCommand:
        throw CommandExecutionError("A requires text input mode");
    case AstNodeKind::InsertCommand:
        throw CommandExecutionError("I requires text input mode");
    case AstNodeKind::ChangeCommand:
        throw CommandExecutionError("C requires text input mode");
    case AstNodeKind::MoveCommand:
        execute_move(static_cast<const MoveCommandNode&>(command), context);
        return;
    case AstNodeKind::TransferCommand:
        execute_transfer(static_cast<const TransferCommandNode&>(command), context);
        return;
    case AstNodeKind::BufferCommand:
        execute_buffer(static_cast<const BufferCommandNode&>(command), context);
        return;
    case AstNodeKind::GlobalCommand:
        execute_global(static_cast<const GlobalCommandNode&>(command), context);
        return;
    case AstNodeKind::ZapCommand:
        execute_zap(static_cast<const ZapCommandNode&>(command), context);
        return;
    case AstNodeKind::SubstituteCommand:
        execute_substitute(static_cast<const SubstituteCommandNode&>(command),
                           context);
        return;
    case AstNodeKind::QuitCommand:
        execute_quit(static_cast<const QuitCommandNode&>(command), context);
        return;
    default:
        throw CommandExecutionError("unsupported command node");
    }
}

void CommandExecutor::execute_append(const AppendCommandNode& command,
                                     ExecutionContext& context,
                                     std::vector<std::string> lines) const {
    auto& buffer = context.current_buffer();
    const auto after = evaluate_append_address(command, buffer);
    buffer.insert_after(after, std::move(lines));
}

void CommandExecutor::execute_insert(const InsertCommandNode& command,
                                     ExecutionContext& context,
                                     std::vector<std::string> lines) const {
    auto& buffer = context.current_buffer();
    const auto after = evaluate_insert_position(command, buffer);
    buffer.insert_after(after, std::move(lines));
}

void CommandExecutor::execute_change(const ChangeCommandNode& command,
                                     ExecutionContext& context,
                                     std::vector<std::string> lines) const {
    auto& buffer = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), buffer);
    const auto insertion_after = range.first - 1;

    buffer.erase(range.first, range.last);
    if (!lines.empty()) {
        buffer.insert_after(insertion_after, std::move(lines));
    }
}

} // namespace fred
