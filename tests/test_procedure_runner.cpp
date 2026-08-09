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

    return 0;
}
