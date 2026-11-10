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
void expect(bool success, std::string_view description) {
    if (!success) {
        std::cerr << "FAIL: " << description << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void run(std::string_view command, fred::CommandExecutor& executor,
         fred::ExecutionContext& context) {
    const auto registry = fred::make_core_command_registry();
    fred::Lexer lexer(command);
    fred::TokenStream tokens(lexer);
    fred::CommandParser parser(tokens, registry, &context);
    executor.execute(*parser.parse(), context);
}

bool rejects(std::string_view command, fred::CommandExecutor& executor,
             fred::ExecutionContext& context) {
    try { run(command, executor, context); }
    catch (const std::exception&) { return true; }
    return false;
}
}

int main() {
    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    run("E(word)/Alpha/", executor, context);
    run("E(other)/Bravo/", executor, context);
    run("FE", executor, context);
    expect(output.content().find("e(other)/Bravo/\ne(word)/Alpha/\n") !=
           std::string::npos, "FE lists named patterns newest first");
    expect(context.expand_named_patterns("T/\\E(word)/") == "T/Alpha/",
           "E is expanded in search command source");
    expect(rejects("E(abcdefghijklmnop)/x/", executor, context),
           "E rejects a name longer than 15 characters");
    expect(rejects("E(broken)plain", executor, context),
           "E rejects undelimited source");
    expect(rejects("U0 P", executor, context), "U rejects zero iterations");
    output.clear();

    run("U3 JM/again/", executor, context);
    expect(output.content() == "again\nagain\nagain\n", "U repeats a command");
    output.clear();
    run("U5 JB", executor, context);
    run("U4 JO", executor, context);
    expect(output.empty(), "JB and JO control U without output");
    expect(rejects("JB", executor, context), "JB outside U rejected");

    auto& buffer = context.current_buffer();
    buffer.append("Alpha");
    buffer.append("Bravo");
    buffer.set_current_line(1);
    const auto macro = context.expand_named_patterns("1T/\\E(word)/");
    run(macro, executor, context);
    expect(context.condition(), "named E model works in the T pattern parser");
    run("UT T/Alpha/", executor, context);
    expect(context.condition(), "UT terminates on true condition");
    run("UF T/absent/", executor, context);
    expect(!context.condition(), "UF terminates on false condition");
    run("UE INVALID", executor, context);
    expect(!context.last_error().empty(), "UE stops on the first error");

    run("1D", executor, context);
    expect(buffer.line_count() == 1 && buffer.line(1) == "Bravo",
           "deletion before V");
    run("V", executor, context);
    expect(buffer.line_count() == 2 && buffer.line(1) == "Alpha",
           "V restores the buffer after deletion");
    run("ZV", executor, context);
    expect(buffer.line_count() == 1 && buffer.line(1) == "Bravo",
           "ZV reapplies the undone deletion");

    run("O+B3", executor, context);
    expect(context.work_pages() == 6, "OB increases work-page budget");
    run("O-B3", executor, context);
    expect(context.work_pages() == 3, "OB decreases work-page budget");
    expect(rejects("O-B4", executor, context), "OB rejects invalid low budget");

    run("O+O\\F", executor, context);
    expect(context.format_output_line(std::string_view("x\fy", 3)) == "x\\Fy",
           "OO maps a requested output control character");
    run("O-O\\F", executor, context);
    expect(context.format_output_line(std::string_view("x\fy", 3)) == "x\\014y",
           "OO removal preserves the existing octal mode");

    run("O-R", executor, context);
    expect(!context.public_read(), "OR toggles new-file public read policy");
    run("FO", executor, context);
    expect(output.content().find("o-r\n") != std::string::npos,
           "FO reports OR state");
    output.clear();
    run("O+R", executor, context);

    buffer.append("a*b");
    run("O-S*", executor, context);
    expect(!context.pattern_character_special('*'), "OS disables metacharacter");
    run("$T/^a*b$/", executor, context);
    expect(context.condition(), "OS literal star matches an exact star");
    run("O+S*", executor, context);
    expect(context.pattern_character_special('*'), "OS restores metacharacter");
    run("$T/^a*b$/", executor, context);
    expect(!context.condition(), "OS special star restores regex repetition");
    expect(rejects("O-S!", executor, context), "OS rejects unsupported meta symbol");

    // J(label) and conditional JT/JF are procedure-level control flow.
    auto& procedure = buffers.create_or_select("lot103");
    procedure.append("JT JM/skip-true/");
    procedure.append("JF JM/print-on-true/");
    procedure.append("J(done)");
    procedure.append("JM/never/");
    procedure.append("@(done)");
    context.set_condition(true);
    const auto registry = fred::make_core_command_registry();
    fred::ProcedureRunner runner(buffers, context, registry, executor, 8);
    runner.execute_buffer("lot103");
    expect(output.content() == "print-on-true\n",
           "J conditional tail and label jump execute in a procedure");
    output.clear();

    // Both pre-existing commands are now included in the 103-command coverage.
    run("!echo FREDPP_SYSTEM_103", executor, context);
    expect(output.content().find("FREDPP_SYSTEM_103") != std::string::npos,
           "historical ! executes a host command");
    output.clear();
    run("ZG(capture103)JM/captured/", executor, context);
    expect(buffers.contains("capture103") &&
           buffers.get("capture103").line(1) == "captured",
           "ZG gathers output in a named buffer");

    std::cout << "FREDPP lot 91-103 OK\n";
}
