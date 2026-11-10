#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

namespace {
void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void execute(std::string_view source, fred::CommandExecutor& executor,
             fred::ExecutionContext& context) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    executor.execute(*parser.parse(), context);
}

bool rejects(std::string_view source, fred::CommandExecutor& executor,
             fred::ExecutionContext& context) {
    try {
        execute(source, executor, context);
    } catch (const std::exception&) {
        return true;
    }
    return false;
}
} // namespace

int main() {
    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;
    auto& buffer = buffers.current();

    execute("#", executor, context);
    require(output.content() == "0\n", "# prints initial counter");
    output.clear();
    buffer.append("Alpha");
    buffer.append("Bravo");
    buffer.append("Charlie");
    buffer.append("Delta");
    buffer.append("Echo");
    buffer.set_current_line(3);
    execute(":1", executor, context);
    require(output.content() == "Bravo\nCharlie\nDelta\n" &&
            buffer.current_line() == 4, ":1 window advances current line");
    output.clear();
    execute(":0", executor, context);
    require(output.content() == "Delta\n", ":0 prints just current line");
    output.clear();
    require(rejects(":100001", executor, context), "window rejects unsafe radius");
    require(rejects("1:2", executor, context), "window rejects addressed form");

    execute("@(LOOP)", executor, context);
    require(rejects("@(ABCDEFGHIJKLMNOP)", executor, context),
            "procedure label respects 15-character limit");
    execute("\\\\0", executor, context);
    require(rejects("\\\\86401", executor, context),
            "wait enforces maximum delay");
    require(rejects("\\0", executor, context),
            "single backslash is not sleep");

    execute("Y", executor, context);
    require(output.content() == "aucune erreur enregistrée\n",
            "Y reports absence of error");
    output.clear();
    execute("JE/Erreur voulue/", executor, context);
    output.clear();
    execute("Y", executor, context);
    require(output.content() == "Erreur voulue\n",
            "Y recalls the most recent user error JE");
    output.clear();
    require(rejects("O+IL%", executor, context) == false,
            "OIL accepts one symbolic escape character");
    require(rejects("O+IU^", executor, context) == false,
            "OIU accepts another escape character");
    require(context.transform_input_line("%A^b normal %Z^q") ==
            "aB normal zQ", "OIL/OIU transform only escaped next character");
    require(rejects("O+IU%", executor, context),
            "OIL/OIU cannot have an ambiguous shared prefix");
    execute("Y", executor, context);
    require(output.content().find("OIL and OIU") != std::string::npos,
            "Y retains executor error");
    output.clear();
    execute("O-IL", executor, context);
    execute("O-IU", executor, context);
    require(context.transform_input_line("%A^b") == "%A^b",
            "disabling input escapes restores literal input");

    execute("O+IA/A> /", executor, context);
    execute("O+IC/CMD> /", executor, context);
    execute("O+IR/RETOUR/", executor, context);
    require(context.append_prompt() == "A> " &&
            context.command_prompt() == "CMD> " &&
            context.return_message() == "RETOUR", "OIA/OIC/OIR store prompts");
    execute("FO", executor, context);
    require(output.content().find("o+ia/A> /") != std::string::npos &&
            output.content().find("o+ic/CMD> /") != std::string::npos &&
            output.content().find("o+ir/RETOUR/") != std::string::npos,
            "FO exposes active input options");
    output.clear();
    execute("O-IA", executor, context);
    execute("O-IC", executor, context);
    execute("O-IR", executor, context);
    require(context.append_prompt().empty() && context.command_prompt().empty() &&
            context.return_message().empty(), "O-I* options restore defaults");
    require(rejects("O+IC/without ending", executor, context),
            "prompt option checks closing delimiter");
    require(rejects("O+ILabc", executor, context),
            "case escape requires exactly one symbol");

    std::cout << "FREDPP lot 71-80 OK\n";
}
