#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"
#include "fred/runtime/ProcedureRunner.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

namespace {
void check(bool good, std::string_view what) {
    if (!good) { std::cerr << "FAIL: " << what << '\n'; std::exit(1); }
}
void run(std::string_view source, fred::CommandExecutor& executor,
         fred::ExecutionContext& context) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    executor.execute(*parser.parse(), context);
}
bool fails(std::string_view source, fred::CommandExecutor& executor,
           fred::ExecutionContext& context) {
    try { run(source, executor, context); }
    catch (const std::exception&) { return true; }
    return false;
}
}

int main() {
    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext ctx(buffers, output);
    fred::CommandExecutor executor;
    auto& buffer = buffers.current();
    buffer.append("Alpha");
    buffer.append("Bravo");
    buffer.append("Charlie");
    buffer.set_current_line(1);
    check(ctx.output_octal(), "OOO historically enabled by default");

    run("O+C/Bonjour/Salut/", executor, ctx);
    check(ctx.transform_input_line("Bonjour ! Bonjour") == "Salut ! Salut",
          "OC transforms each full literal occurrence");
    run("O-C", executor, ctx);
    check(ctx.transform_input_line("Bonjour") == "Bonjour", "O-C clears mapping");
    check(fails("O+C//x/", executor, ctx), "OC rejects empty source");

    run("O-I\\F", executor, ctx);
    check(!ctx.input_terminator_required(), "OI\\F toggles inline input");
    run("O+I\\F", executor, ctx);
    check(ctx.input_terminator_required(), "OI\\F restores terminator");
    run("O+IE!", executor, ctx);
    check(ctx.input_escape() == '!', "OIE changes text terminator character");
    run("O-IE", executor, ctx);
    check(ctx.input_escape() == '\\', "OIE resets escape");
    run("O+IE\\", executor, ctx);
    check(ctx.input_escape() == '\\', "OIE accepts historical default explicitly");

    run("O-IF", executor, ctx);
    check(!ctx.input_feed(), "OIF disables return-message line feed");
    run("O+IF", executor, ctx);
    check(ctx.input_feed(), "OIF restores return-message line feed");

    run("O+OL", executor, ctx);
    check(ctx.format_output_line("Abc") == "A%B%C", "OOL encodes lowercase");
    check(fails("O+OU", executor, ctx), "conflicting output case modes rejected");
    run("O-OL", executor, ctx);
    run("O+OU", executor, ctx);
    check(ctx.format_output_line("AbC") == "^ab^c", "OOU encodes uppercase");
    run("O-OU", executor, ctx);
    run("O+OO", executor, ctx);
    check(ctx.format_output_line(std::string_view("a\f", 2)) == "a\\014",
          "OOO represents special bytes in octal");
    run("O-OO", executor, ctx);
    check(!ctx.output_octal(), "O-OO disables historical octal mode");

    run("O+P2", executor, ctx);
    check(ctx.page_size() == 2, "OP sets page size");
    run("1,$P", executor, ctx);
    check(output.content().find("Alpha\nBravo\n-- page suivante --\nCharlie\n") !=
          std::string::npos, "P marks nonblocking page boundaries");
    output.clear();
    run("O-P", executor, ctx);
    check(ctx.page_size() == 0, "O-P disables pages");
    check(fails("O+P0", executor, ctx), "OP rejects zero page size");

    check(fails("1S/Alpha/Done", executor, ctx),
          "S missing closing delimiter rejected by default");
    run("O-S/", executor, ctx);
    run("1S/Alpha/Done", executor, ctx);
    check(buffer.line(1) == "Done", "O-S/ permits omitted S final delimiter");
    run("O+S/", executor, ctx);
    check(ctx.substitution_separator_required(), "O+S/ restores delimiter");

    const auto registry = fred::make_core_command_registry();
    fred::ProcedureRunner runner(buffers, ctx, registry, executor, 8);
    auto& procedure = buffers.create_or_select("proc-jo");
    procedure.append("JM/start/");
    procedure.append("JO");
    procedure.append("JM/never/");
    output.clear();
    runner.execute_buffer("proc-jo");
    check(output.content() == "start\n", "JO exits current procedure");

    auto& on_true = buffers.create_or_select("proc-jot");
    on_true.append("JOT");
    on_true.append("JM/after-true/");
    output.clear();
    ctx.set_condition(true);
    runner.execute_buffer("proc-jot");
    check(output.content().empty(), "JOT exits on true condition");
    ctx.set_condition(false);
    runner.execute_buffer("proc-jot");
    check(output.content() == "after-true\n", "JOT continues on false condition");

    auto& on_false = buffers.create_or_select("proc-jof");
    on_false.append("JOF");
    on_false.append("JM/after-false/");
    output.clear();
    ctx.set_condition(false);
    runner.execute_buffer("proc-jof");
    check(output.content().empty(), "JOF exits on false condition");
    ctx.set_condition(true);
    runner.execute_buffer("proc-jof");
    check(output.content() == "after-false\n", "JOF continues on true condition");

    auto& nested = buffers.create_or_select("proc-jo-nested");
    nested.append("JM/parent-before/");
    nested.append("\\B(proc-jo)");
    nested.append("JM/parent-after/");
    output.clear();
    runner.execute_buffer("proc-jo-nested");
    check(output.content() == "parent-before\nstart\nparent-after\n",
          "JO exits only the current nested procedure");

    auto& text_input = buffers.create_or_select("proc-oi-inline");
    text_input.append("B(input-result)");
    text_input.append("O-I\\F");
    text_input.append("O+C/bonjour/salut/");
    text_input.append("A bonjour");
    text_input.append("O-C");
    text_input.append("O+I\\F");
    text_input.append("O+IE!");
    text_input.append("A");
    text_input.append("bloc");
    text_input.append("!F");
    text_input.append("O-IE");
    output.clear();
    runner.execute_buffer("proc-oi-inline");
    check(buffers.get("input-result").line(1) == "salut",
          "inline input and OC operate inside a procedure");
    check(buffers.get("input-result").line(2) == "bloc",
          "OIE terminator is honored by procedure block input");

    check(fails("O+IFgarbage", executor, ctx), "malformed option rejected");

    std::cout << "FREDPP lot 81-90 OK\n";
}
