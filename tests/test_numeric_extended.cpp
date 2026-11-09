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
#include <string>
#include <string_view>

namespace {

void execute(std::string_view source, fred::ExecutionContext& context) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    const auto command = parser.parse();
    fred::CommandExecutor{}.execute(*command, context);
}

void expect_runtime_error(std::string_view source, std::string_view message,
                          fred::ExecutionContext& context) {
    bool rejected = false;
    try {
        execute(source, context);
    } catch (const fred::CommandExecutionError& error) {
        rejected = error.what() == message;
    }
    assert(rejected);
}

void expect_parse_error(std::string_view source,
                        fred::ExecutionContext& context) {
    bool rejected = false;
    try {
        execute(source, context);
    } catch (const fred::ParseError&) {
        rejected = true;
    }
    assert(rejected);
}

} // namespace

int main() {
    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);

    execute("N(bit):1", context);
    execute("N(bit)~", context);
    assert(context.numeric_register("bit") == -2);
    execute("N(bit)^7", context);
    assert(context.numeric_register("bit") == -7);
    execute("N(bit)&7", context);
    assert(context.numeric_register("bit") == 1);
    execute("N(bit){3", context);
    assert(context.numeric_register("bit") == 8);
    execute("N(bit)}2", context);
    assert(context.numeric_register("bit") == 2);
    execute("N(bit):-8", context);
    execute("N(bit)}2", context);
    assert(context.numeric_register("bit") == -2);
    execute("N(bit):-1", context);
    execute("N(bit){1", context);
    assert(context.numeric_register("bit") == -2);
    execute("N(bit)^\\N(bit)", context);
    assert(context.numeric_register("bit") == 0);

    const auto counter_before = context.counter();
    expect_runtime_error("N(bit){64",
                         "numeric shift count must be between 0 and 63",
                         context);
    expect_runtime_error("N(bit)}-1",
                         "numeric shift count must be between 0 and 63",
                         context);
    assert(context.numeric_register("bit") == 0 &&
           context.counter() == counter_before);

    execute("N(fmt):7", context);
    execute("N(fmt)D4", context);
    execute("N(fmt)P", context);
    assert(output.content() == "0007\n");
    output.clear();
    execute("N(fmt)F.", context);
    execute("N(fmt)B2", context);
    execute("N(fmt)P", context);
    assert(output.content() == ".111\n");
    output.clear();
    execute("N(fmt)Fz", context);
    execute("N(fmt)P", context);
    assert(output.content() == "z111\n");
    output.clear();
    execute("N(fmt)F.", context);
    execute("N(fmt)B36", context);
    execute("N(fmt):35", context);
    execute("N(fmt)P", context);
    assert(output.content() == "...Z\n");
    output.clear();
    execute("N(fmt):-35", context);
    execute("N(fmt)P", context);
    assert(output.content() == "-...Z\n");
    output.clear();

    execute("N(fmt)D0", context);
    execute("N(fmt)B10", context);
    execute("N(fmt)P", context);
    assert(output.content() == "-35\n");
    output.clear();
    execute("FN", context);
    assert(output.content().find("n(fmt):-35\n") != std::string::npos);
    output.clear();

    execute("N(min):-9223372036854775808", context);
    execute("N(min)B16", context);
    execute("N(min)P", context);
    assert(output.content() == "-8000000000000000\n");
    output.clear();

    const auto value_before = context.numeric_register("fmt");
    const auto counter_format_before = context.counter();
    expect_runtime_error("N(fmt)B1", "numeric base must be between 2 and 36",
                         context);
    expect_runtime_error("N(fmt)B37", "numeric base must be between 2 and 36",
                         context);
    expect_runtime_error("N(fmt)D-1",
                         "numeric minimum digits must be between 0 and 1024",
                         context);
    expect_runtime_error("N(fmt)D1025",
                         "numeric minimum digits must be between 0 and 1024",
                         context);
    assert(context.numeric_register("fmt") == value_before &&
           context.counter() == counter_format_before);
    expect_parse_error("N(fmt)F", context);
    expect_parse_error("N(fmt)Fabc", context);
    expect_parse_error("N(fmt)P7", context);
    expect_parse_error("1N(fmt)P", context);

    auto& buffer = context.current_buffer();
    buffer.append("abc");
    buffer.append("caf\xC3\xA9");
    execute("N(line)A", context);
    assert(context.numeric_register("line") == 2);
    execute("N(chars)L", context);
    assert(context.numeric_register("chars") == 4);
    execute("1,$N(chars)L", context);
    assert(context.numeric_register("chars") == 7);
    assert(buffer.current_line() == 2 && buffer.line_count() == 2);
    execute("N(chars)P", context);
    assert(output.content() == "7\n");
    output.clear();

    execute("N(fresh)D3", context);
    assert(context.has_numeric_register("fresh"));
    const auto previous_counter = context.counter();
    execute("N(fresh)P", context);
    assert(output.content() == "000\n" &&
           context.counter() == previous_counter);

    std::cout << "numeric operations (41-50) passed\n";
}
