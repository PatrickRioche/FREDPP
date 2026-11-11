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

    // Inline active \B is a command-stream injection, not text substitution.
    auto& inline_flow = buffers.create_or_select("inline-flow");
    inline_flow.append("JP!before ! \\B(child) JM!after!");
    runner.execute_buffer("inline-flow");
    assert(output.content() == "before enfant\nafter\n");
    output.clear();

    // A buffer may also be injected between other commands in one line.
    auto& chained_flow = buffers.create_or_select("chained-flow");
    chained_flow.append("B(data) \\B(child) 1P");
    runner.execute_buffer("chained-flow");
    assert(output.content() == "enfant\nalpha\n");
    output.clear();

    // Within an A/I/C text block, active B contributes logical text lines,
    // with nested directives executed before returning to the caller.
    auto& inner_text = buffers.create_or_select("inner-text");
    inner_text.append("one");
    inner_text.append("two");
    auto& outer_text = buffers.create_or_select("outer-text");
    outer_text.append("start");
    outer_text.append("\\B(inner-text)");
    auto& text_flow = buffers.create_or_select("text-flow");
    text_flow.append("B(text-result)");
    text_flow.append("A");
    text_flow.append("pre\\B(outer-text)post");
    text_flow.append("\\F");
    text_flow.append("1,$P");
    runner.execute_buffer("text-flow");
    assert(buffers.current().line_count() == 5);
    assert(buffers.current().line(1) == "prestart");
    assert(buffers.current().line(2) == "one");
    assert(buffers.current().line(3) == "two");
    assert(buffers.current().line(4).empty());
    assert(buffers.current().line(5) == "post");
    assert(output.content() == "prestart\none\ntwo\n\npost\n");
    output.clear();

    // Directive-like text within a delimited message remains message text.
    auto& protected_flow = buffers.create_or_select("protected-flow");
    protected_flow.append("JM!literal \\B(child)!");
    runner.execute_buffer("protected-flow");
    assert(output.content() == "literal \\B(child)\n");
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

    // The exact historical nesting boundary remains available to finite
    // procedures; only an additional nested buffer is rejected.
    const auto check_nested_depth = [](
        std::size_t number_of_buffers,
        std::size_t maximum_depth,
        bool should_succeed) {
        fred::BufferManager chain_buffers;
        for (std::size_t index = 0; index < number_of_buffers; ++index) {
            auto& entry = chain_buffers.create_or_select(
                "level" + std::to_string(index));
            if (index + 1 < number_of_buffers) {
                entry.append("\\B(level" + std::to_string(index + 1) + ")");
            } else {
                entry.append("JM/finished/");
            }
        }
        fred::StringOutput chain_output;
        fred::ExecutionContext chain_context(chain_buffers, chain_output);
        const auto chain_registry = fred::make_core_command_registry();
        fred::CommandExecutor chain_executor;
        fred::ProcedureRunner chain_runner(
            chain_buffers, chain_context, chain_registry,
            chain_executor, maximum_depth);
        bool failed = false;
        try {
            chain_runner.execute_buffer("level0");
        } catch (const std::runtime_error& error) {
            failed = true;
            assert(std::string_view(error.what()).find(
                "maximum procedure buffer-flow depth exceeded") !=
                std::string_view::npos);
        }
        assert(failed == !should_succeed);
        if (should_succeed) {
            assert(chain_output.content() == "finished\n");
        }
    };
    check_nested_depth(15, 16, true);
    check_nested_depth(16, 16, true);
    check_nested_depth(17, 16, false);
    check_nested_depth(255, 256, true);
    check_nested_depth(256, 256, true);
    check_nested_depth(257, 256, false);


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
