#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cstdlib>
#include <exception>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>

namespace {
void check(bool value, std::string_view message) {
    if (!value) {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}
void run(std::string_view source, fred::CommandExecutor& executor,
         fred::ExecutionContext& context) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    const auto node = parser.parse();
    executor.execute(*node, context);
}
bool fails(std::string_view source, fred::CommandExecutor& executor,
           fred::ExecutionContext& context) {
    try { run(source, executor, context); }
    catch (const std::exception&) { return true; }
    return false;
}
void fill(fred::Buffer& b, std::initializer_list<std::string_view> lines) {
    for (const auto line : lines) b.append(std::string(line));
}
} // namespace

int main() {
    fred::CommandExecutor executor;
    { // Buffer copy/print, including a snapshot of the same buffer.
        fred::BufferManager buffers;
        fred::StringOutput out;
        fred::ExecutionContext context(buffers, out);
        auto& a = buffers.create_or_select("a");
        fill(a, {"alpha", "beta"});
        a.mark_clean();
        auto& b = buffers.create_or_select("b");
        fill(b, {"head", "tail"});
        b.set_current_line(1);
        b.mark_clean();
        run("ZP(a)", executor, context);
        check(out.content() == "alpha\nbeta\n" && b.current_line() == 1 &&
              !b.modified(), "ZP prints without selecting or changing buffer");
        run("ZA(a)", executor, context);
        check(b.line_count() == 4 && b.line(2) == "alpha" &&
              b.line(3) == "beta" && b.current_line() == 3 &&
              a.line_count() == 2 && !a.modified(), "ZA copies after cursor");
        run("0ZA(a)", executor, context);
        check(b.line(1) == "alpha" && b.line(2) == "beta",
              "ZA explicit address zero");
        check(fails("1,2ZA(a)", executor, context) &&
              fails("ZA(absent)", executor, context), "ZA bad address/unknown buffer");
        buffers.select("a");
        run("ZA(a)", executor, context);
        check(a.line_count() == 4 && a.line(3) == "alpha", "ZA self-copy snapshot");
    }
    { // Sort order, condition flag and dirty tracking.
        fred::BufferManager buffers;
        fred::StringOutput out;
        fred::ExecutionContext context(buffers, out);
        auto& b = buffers.current();
        fill(b, {"c", "a", "b", "b"});
        b.set_current_line(2);
        b.mark_clean();
        run("1,3ZS", executor, context);
        check(b.line(1) == "a" && b.line(3) == "c" && b.line(4) == "b" &&
              !context.condition() && b.current_line() == 2 && b.modified(),
              "ZS sorts range and preserves cursor");
        run("1,3ZB", executor, context);
        check(b.line(1) == "c" && b.line(3) == "a" && !context.condition(),
              "ZB descending order");
        run("2,4ZS", executor, context);
        run("3,4ZS", executor, context);
        check(context.condition(), "sort on identical lines sets condition");
        check(fails("0ZB", executor, context), "sort rejects invalid address");
        run("ZC", executor, context);
        check(context.condition() && !b.modified(), "ZC clears dirty flag");
        run("ZC", executor, context);
        check(!context.condition() && fails("1ZC", executor, context),
              "ZC condition and syntax");
    }
    { // Inter-buffer moves, same-buffer moves, deletion.
        fred::BufferManager buffers;
        fred::StringOutput out;
        fred::ExecutionContext context(buffers, out);
        auto& a = buffers.create_or_select("a");
        fill(a, {"one", "two", "three", "four"});
        a.set_current_line(3);
        auto& b = buffers.create_or_select("b");
        fill(b, {"head", "tail"});
        b.set_current_line(1);
        buffers.select("a");
        run("1,2ZM(b)", executor, context);
        check(a.line_count() == 2 && a.line(1) == "three" && a.current_line() == 1 &&
              b.line_count() == 4 && b.line(2) == "one" && b.line(3) == "two" &&
              b.current_line() == 3, "ZM moves addressed lines between buffers");
        check(fails("1ZM(a)", executor, context), "ZM self-move rejects cursor");
        a.set_current_line(2);
        run("1ZM(a)", executor, context);
        check(a.line(1) == "four" && a.line(2) == "three", "ZM self-move reorders");
        check(fails("ZD(missing)", executor, context), "ZD missing target");
        run("ZD(b)", executor, context);
        check(!buffers.contains("b") && buffers.current().name() == "a",
              "ZD removes buffer without selecting another");
        check(fails("1ZD(a)", executor, context), "ZD refuses line addresses");
    }
    { // Case with/without patterns; ASCII mapping and counter.
        fred::BufferManager buffers;
        fred::StringOutput out;
        fred::ExecutionContext context(buffers, out);
        auto& b = buffers.current();
        fill(b, {"ABC DEF", "Abc ABC", "éclair"});
        run("1,2ZL", executor, context);
        check(b.line(1) == "abc def" && b.line(2) == "abc abc" &&
              context.counter() == 2 && b.current_line() == 2,
              "ZL modifies lines and counts lines");
        run("1,2ZU/abc/", executor, context);
        check(b.line(1) == "ABC def" && b.line(2) == "ABC ABC" &&
              context.counter() == 3 && b.current_line() == 2,
              "ZU converts matching strings and counts changed matches");
        check(fails("1,2ZU/xyz/", executor, context) && context.counter() == 0,
              "ZU rejects no changes");
        run("1,2ZT/AB/de/", executor, context);
        check(b.line(1) == "deC def" && b.line(2) == "deC deC" &&
              context.counter() == 6 && b.current_line() == 2,
              "ZT maps characters and counts changed bytes");
        check(fails("ZT/AAB/xyz/", executor, context) &&
              fails("ZT/AB/c/", executor, context) &&
              fails("ZT/é/é/", executor, context),
              "ZT refuses malformed or non-ASCII mappings");
        run("3ZU", executor, context);
        check(b.line(3) == "éCLAIR" && context.counter() == 1,
              "ZU leaves non-ASCII UTF-8 bytes unchanged");
    }
    std::cout << "extended Z commands OK\n";
}
