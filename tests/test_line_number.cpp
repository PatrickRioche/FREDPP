#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/parser/ParseError.hpp"
#include "fred/runtime/CommandExecutionError.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <string_view>

namespace {

std::unique_ptr<fred::CommandNode> parse(std::string_view text) {
    fred::Lexer lexer(text);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    return parser.parse();
}

void execute(std::string_view text,
             fred::CommandExecutor& executor,
             fred::ExecutionContext& context) {
    const auto command = parse(text);
    executor.execute(*command, context);
}

bool rejects_syntax(std::string_view text) {
    try {
        (void)parse(text);
    } catch (const fred::ParseError&) {
        return true;
    }
    return false;
}

} // namespace

int main() {
    const auto plain = parse("=");
    assert(plain->kind() == fred::AstNodeKind::LineNumberCommand);
    assert(!plain->has_address());

    const auto addressed = parse("$=");
    assert(addressed->kind() == fred::AstNodeKind::LineNumberCommand);
    assert(addressed->has_address());

    // A line-number query accepts no trailing operands or address ranges.
    assert(rejects_syntax("= extra"));
    assert(rejects_syntax("1,2="));

    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    // A fresh buffer has the historical zero cursor, not an address error.
    execute("=", executor, context);
    assert(output.content() == "0\n");
    output.clear();

    auto& buffer = buffers.create_or_select("data");
    buffer.append("alpha");
    buffer.append("beta");
    buffer.append("gamma");
    buffer.set_current_line(1);

    context.set_counter(41);
    context.set_condition(true);
    const auto cursor_before = buffer.current_line();
    const auto selected_before = buffers.current().name();
    const auto modified_before = buffer.modified();

    execute("=", executor, context);
    execute("$=", executor, context);
    execute("2=", executor, context);
    execute("=", executor, context);

    assert(output.content() == "1\n3\n2\n1\n");
    assert(buffer.current_line() == cursor_before);
    assert(buffers.current().name() == selected_before);
    assert(buffer.modified() == modified_before);
    assert(context.counter() == 41);
    assert(context.condition());

    bool invalid_address = false;
    try {
        execute("4=", executor, context);
    } catch (const fred::CommandExecutionError&) {
        invalid_address = true;
    }
    assert(invalid_address);
    assert(output.content() == "1\n3\n2\n1\n");
    assert(buffer.current_line() == cursor_before);
    return 0;
}
