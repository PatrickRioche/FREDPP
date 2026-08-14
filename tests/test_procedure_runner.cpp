#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"
#include "fred/runtime/ProcedureRunner.hpp"

#include <cassert>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

void expect_error(auto&& action, std::string_view fragment) {
    try {
        action();
        assert(false && "expected procedure error");
    } catch (const std::exception& error) {
        assert(std::string_view(error.what()).find(fragment) !=
               std::string_view::npos);
    }
}

} // namespace

int main() {
    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;
    const auto registry = fred::make_core_command_registry();
    fred::ProcedureRunner runner(buffers, context, registry, executor, 8);

    auto& proc = buffers.create_or_select("proc");
    proc.append("\" procédure principale");
    proc.append("JM/Début/");
    proc.append("B(data)");
    proc.append("A");
    proc.append("alpha");
    proc.append("beta");
    proc.append("\\F");
    proc.append("1,$P");
    proc.append("JP/Fin / JM/OK/");

    runner.execute_buffer("proc");
    assert(output.content() ==
           "Début\nalpha\nbeta\nFin OK\n");

    output.clear();

    auto& hello = buffers.create_or_select("hello-s");
    hello.append("Hello");
    auto& word = buffers.create_or_select("word-s");
    word.append("Word");

    auto& flow_message = buffers.create_or_select("flow-message");
    flow_message.append("JP/ \\S(hello-s) \\S(word-s)/");
    flow_message.append("JM/!/");

    runner.execute_buffer("flow-message");
    assert(output.content() == " Hello Word!\n");

    output.clear();

    auto& child = buffers.create_or_select("child");
    child.append("JM/enfant/");

    auto& parent = buffers.create_or_select("parent");
    parent.append("JM/parent/");
    parent.append("\\B(child)");
    parent.append("JM/fin/");

    runner.execute_buffer("parent");
    assert(output.content() == "parent\nenfant\nfin\n");

    output.clear();

    auto& monitored = buffers.create_or_select("monitored");
    monitored.append("O+M");
    monitored.append("JM/Bonjour/");
    monitored.append("O-M");
    monitored.append("JM/Après/");

    runner.execute_buffer("monitored");
    assert(output.content() ==
           "JM/Bonjour/\n"
           "Bonjour\n"
           "O-M\n"
           "Après\n");
    assert(!context.monitor_commands());

    auto& bad_text = buffers.create_or_select("bad-text");
    bad_text.append("A");
    bad_text.append("jamais terminé");
    expect_error([&] { runner.execute_buffer("bad-text"); },
                 "end of procedure before \\F");

    expect_error([&] { runner.execute_buffer("missing"); },
                 "unknown procedure buffer");

    auto& recursive = buffers.create_or_select("recursive");
    recursive.append("\\B(recursive)");
    expect_error([&] { runner.execute_buffer("recursive"); },
                 "maximum procedure buffer-flow depth exceeded");


    {
        fred::BufferManager chain_buffers;
        fred::StringOutput chain_output;
        fred::ExecutionContext chain_context(chain_buffers, chain_output);
        fred::CommandExecutor chain_executor;
        const auto chain_registry = fred::make_core_command_registry();
        fred::ProcedureRunner chain_runner(
            chain_buffers,
            chain_context,
            chain_registry,
            chain_executor,
            8);

        auto& chain_proc =
            chain_buffers.create_or_select("chain-proc");
        chain_proc.append("B(buff) A");
        chain_proc.append("alpha");
        chain_proc.append("\\F");
        chain_proc.append("B(other) B(buff) *");

        chain_runner.execute_buffer("chain-proc");

        assert(chain_buffers.current().name() == "buff");
        assert(chain_buffers.current().line_count() == 1);
        assert(chain_buffers.current().line(1) == "alpha");
        assert(chain_output.content() == "alpha\n");
    }


    output.clear();
    auto& arbitrary_messages =
        buffers.create_or_select("arb-msg");
    arbitrary_messages.append("JM!Mode d'emploi : !");
    arbitrary_messages.append("JP:Input A/B: JM?OK?");
    arbitrary_messages.append("B(after-message)");

    runner.execute_buffer("arb-msg");

    assert(output.content() ==
           "Mode d'emploi : \n"
           "Input A/BOK\n");
    assert(buffers.current().name() == "after-message");

    return 0;
}
