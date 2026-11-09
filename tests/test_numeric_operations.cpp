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
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

void execute(std::string_view source, fred::ExecutionContext& context) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    fred::CommandExecutor executor;
    const auto command = parser.parse();
    executor.execute(*command, context);
}

void expect_error(std::string_view source,
                  std::string_view expected,
                  fred::ExecutionContext& context) {
    try {
        execute(source, context);
        assert(false && "expected numeric command error");
    } catch (const fred::CommandExecutionError& error) {
        assert(std::string_view(error.what()) == expected);
    }
}

} // namespace

int main() {
    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);

    execute("N(x):20", context);
    execute("n(x)+5", context);
    assert(context.numeric_register("x") == 25 && context.counter() == 25);
    execute("N(x)-3", context);
    assert(context.numeric_register("x") == 22);
    execute("N(x)*4", context);
    assert(context.numeric_register("x") == 88);
    execute("N(x)/8", context);
    assert(context.numeric_register("x") == 11);
    execute("N(x)%4", context);
    assert(context.numeric_register("x") == 3);
    execute("N(x)&6", context);
    assert(context.numeric_register("x") == 2);
    execute("N(x)|8", context);
    assert(context.numeric_register("x") == 10);

    execute("N(copy)+\\N(x)", context);
    assert(context.numeric_register("copy") == 10);
    execute("N(copy)+#", context);
    assert(context.numeric_register("copy") == 20);
    execute("N(copy)-\\N(x)", context);
    assert(context.numeric_register("copy") == 10);

    context.set_condition(false);
    execute("N(x)=10", context);
    assert(context.condition() && context.counter() == 10);
    execute("N(x)<10", context);
    assert(!context.condition() && context.numeric_register("x") == 10);
    execute("N(x)<11", context);
    assert(context.condition());
    execute("N(x)>10", context);
    assert(!context.condition());
    execute("N(x)>9", context);
    assert(context.condition());

    execute("N(negative):-5", context);
    execute("N(negative)|", context);
    assert(context.numeric_register("negative") == 5);
    execute("N(negative)|8", context);
    assert(context.numeric_register("negative") == 13);
    execute("N(mask):-1", context);
    execute("N(mask)&7", context);
    assert(context.numeric_register("mask") == 7);
    execute("N(signed):-11", context);
    execute("N(signed)/3", context);
    assert(context.numeric_register("signed") == -3);
    execute("N(signed)%2", context);
    assert(context.numeric_register("signed") == -1);

    // Reject undefined signed arithmetic without corrupting register/counter.
    execute("N(max):9223372036854775807", context);
    const auto before = context.counter();
    expect_error("N(max)+1", "numeric addition overflow", context);
    expect_error("N(max)*2", "numeric multiplication overflow", context);
    expect_error("N(max)/0", "numeric division by zero", context);
    expect_error("N(max)%0", "numeric division by zero", context);
    assert(context.numeric_register("max") == before &&
           context.counter() == before);
    execute("N(min):-9223372036854775808", context);
    const auto minimum = std::numeric_limits<std::int64_t>::min();
    expect_error("N(min)-1", "numeric subtraction overflow", context);
    expect_error("N(min)/-1", "numeric division overflow", context);
    expect_error("N(min)%-1", "numeric division overflow", context);
    expect_error("N(min)|", "numeric absolute value overflow", context);
    assert(context.numeric_register("min") == minimum &&
           context.counter() == minimum);

    // Symbolic line operands are resolved at execution, not parsing time.
    execute("N(last)+$", context);
    assert(context.numeric_register("last") == 0);
    execute("N(here)+.", context);
    assert(context.numeric_register("here") == 0);

    std::cout << "numeric operations (31-40) passed\n";
}
