#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutionError.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void execute(std::string_view text, fred::CommandExecutor& executor,
             fred::ExecutionContext& context) {
    fred::Lexer lexer(text);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    const auto command = parser.parse();
    executor.execute(*command, context);
}

} // namespace

int main() {
    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    auto& buffer = buffers.current();
    buffer.append("alpha");
    buffer.append("beta");
    buffer.append("gamma");
    buffer.set_current_line(1);
    context.set_counter(17);

    // An unaddressed T searches only the current line and sets condition.
    execute("T/alpha/", executor, context);
    require(context.condition(), "T finds a pattern on the current line");
    execute("T/beta/", executor, context);
    require(!context.condition(), "T reports no match on current line");

    // The inversion applies to the existence of a match over the whole range.
    execute("T~/beta/", executor, context);
    require(context.condition(), "T~ succeeds when no line matches");
    execute("T~/alpha/", executor, context);
    require(!context.condition(), "T~ fails when a line matches");
    execute("1,3T/beta/", executor, context);
    require(context.condition(), "T finds a match anywhere in a range");
    execute("1,1T/beta/", executor, context);
    require(!context.condition(), "T rejects a match outside the range");
    execute("1,3T~?missing?", executor, context);
    require(context.condition(), "T~ supports alternate pattern delimiter");
    execute("2T?^beta?", executor, context);
    require(context.condition(), "T supports anchored patterns");

    // T is a read-only condition command; it must not change unrelated state.
    require(buffer.line_count() == 3 && buffer.line(2) == "beta",
            "T does not modify text");
    require(buffer.current_line() == 1, "T does not move the current line");
    require(context.counter() == 17, "T preserves the numeric counter");
    require(output.empty(), "T produces no output");

    // Invalid addresses reject the command before altering condition state.
    context.set_condition(true);
    bool rejected = false;
    try {
        execute("4T/alpha/", executor, context);
    } catch (const fred::CommandExecutionError&) {
        rejected = true;
    }
    require(rejected && context.condition(),
            "invalid address preserves the prior condition");

    fred::BufferManager empty_buffers;
    empty_buffers.create_or_select("empty");
    fred::ExecutionContext empty_context(empty_buffers, output);
    rejected = false;
    try {
        execute("T/alpha/", executor, empty_context);
    } catch (const fred::CommandExecutionError&) {
        rejected = true;
    }
    require(rejected, "T rejects an empty buffer");
    return EXIT_SUCCESS;
}
