#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"
#include "fred/runtime/ProcedureRunner.hpp"

#include <cassert>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

std::string run_lines(std::vector<std::string> lines,
                      bool condition,
                      std::size_t maximum_steps = 1'000'000,
                      std::chrono::milliseconds maximum_elapsed =
                          std::chrono::seconds{60}) {
    fred::BufferManager buffers;
    auto& procedure = buffers.create_or_select("proc");
    for (auto& line : lines) {
        procedure.append(std::move(line));
    }

    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);
    context.set_condition(condition);

    const auto registry = fred::make_core_command_registry();
    fred::CommandExecutor executor;
    fred::ProcedureRunner runner(
        buffers, context, registry, executor, 256,
        maximum_steps, maximum_elapsed);

    runner.execute_buffer("proc");
    return output.content();
}

void expect_error(std::vector<std::string> lines,
                  std::string_view expected) {
    try {
        (void)run_lines(std::move(lines), false);
        assert(false && "expected procedure error");
    } catch (const std::runtime_error& error) {
        assert(std::string_view(error.what()) == expected);
    }
}

void expect_safety_error(std::vector<std::string> lines,
                         std::size_t maximum_steps,
                         std::chrono::milliseconds maximum_elapsed,
                         std::string_view expected) {
    bool caught = false;
    try {
        (void)run_lines(std::move(lines), false,
                        maximum_steps, maximum_elapsed);
    } catch (const std::runtime_error& error) {
        caught = true;
        assert(std::string_view(error.what()).find(expected) !=
               std::string_view::npos);
    }
    assert(caught && "expected safety limit to stop procedure");
}

} // namespace

int main() {
    {
        const auto output = run_lines({
            "JM/start/",
            "J(done)",
            "JM/skip/",
            "@(done)",
            "JM/end/",
        }, false);
        assert(output == "start\nend\n");
    }

    {
        const auto output = run_lines({
            "J(yes)T",
            "JM/no/",
            "@(yes)",
            "JM/end/",
        }, true);
        assert(output == "end\n");
    }

    {
        const auto output = run_lines({
            "J(yes)T",
            "JM/no/",
            "@(yes)",
            "JM/end/",
        }, false);
        assert(output == "no\nend\n");
    }

    {
        const auto output = run_lines({
            "J(no)F",
            "JM/true-path/",
            "@(no)",
            "JM/end/",
        }, false);
        assert(output == "end\n");
    }

    {
        const auto output = run_lines({
            "J(no)F",
            "JM/true-path/",
            "@(no)",
            "JM/end/",
        }, true);
        assert(output == "true-path\nend\n");
    }

    // Les exemples historiques montrent des variations de casse.
    {
        const auto output = run_lines({
            "J(FinSi)",
            "JM/skip/",
            "@(finsi)",
            "JM/case-ok/",
        }, false);
        assert(output == "case-ok\n");
    }

    // En présence de doublons, la première étiquette située après J gagne.
    {
        const auto output = run_lines({
            "J(x)",
            "@(x)",
            "JM/first/",
            "J(end)",
            "@(x)",
            "JM/second/",
            "@(end)",
            "JM/done/",
        }, false);
        assert(output == "first\ndone\n");
    }

    // Les sauts vers une etiquette anterieure sont autorises.
    // Ils sont necessaires aux boucles historiques FRED.
    {
        // A countdown is a legitimate backward-jump loop, not a cycle error.
        const auto output = run_lines({
            "N(count):4",
            "@(again)",
            "N(count)-1",
            "N(count)=0J(done)T",
            "J(again)",
            "@(done)",
            "JM/loop-complete/",
        }, false, 25);
        assert(output == "loop-complete\n");
    }

    // A permanent J loop must fail rather than monopolize the process.
    expect_safety_error({
        "@(again)",
        "J(again)",
    }, 25, std::chrono::seconds{60},
       "procedure instruction limit exceeded");

    // Elapsed-time protection is independent from the instruction budget.
    expect_safety_error({
        "@(again)",
        "J(again)",
    }, 1'000'000, std::chrono::milliseconds{0},
       "procedure elapsed time limit exceeded");

    expect_error({
        "J(missing)",
        "JM/unreachable/",
    }, "? label not found");

    expect_error({
        "@(1234567890123456)",
    }, "label name exceeds historical limit of 15 characters");

    expect_error({
        "J(1234567890123456)",
    }, "label name exceeds historical limit of 15 characters");

    expect_error({
        "J(x)X",
        "@(x)",
    }, "J(label) accepts only optional T or F in this lot");

    // Une procédure imbriquée possède son propre espace de lignes/labels.
    {
        fred::BufferManager buffers;

        auto& child = buffers.create_or_select("child");
        child.append("J(child_end)");
        child.append("JM/child-skip/");
        child.append("@(child_end)");
        child.append("JM/child-ok/");

        auto& parent = buffers.create_or_select("parent");
        parent.append("JM/parent-before/");
        parent.append("\\B(child)");
        parent.append("JM/parent-after/");

        fred::StringOutput output;
        fred::ExecutionContext context(buffers, output);
        const auto registry = fred::make_core_command_registry();
        fred::CommandExecutor executor;
        fred::ProcedureRunner runner(
            buffers, context, registry, executor);

        runner.execute_buffer("parent");
        assert(
            output.content() ==
            "parent-before\nchild-ok\nparent-after\n");
    }

    // Each nested \B call spends the same budget as its parent. A fresh
    // top-level invocation starts a new budget, so the runner is reusable.
    {
        fred::BufferManager buffers;
        auto& child = buffers.create_or_select("child");
        child.append("JM/child/");
        auto& parent = buffers.create_or_select("parent");
        parent.append("@(again)");
        parent.append("\\B(child)");
        parent.append("J(again)");

        fred::StringOutput output;
        fred::ExecutionContext context(buffers, output);
        const auto registry = fred::make_core_command_registry();
        fred::CommandExecutor executor;
        fred::ProcedureRunner runner(
            buffers, context, registry, executor,
            256, 13, std::chrono::seconds{60});

        bool stopped = false;
        try {
            runner.execute_buffer("parent");
        } catch (const std::runtime_error& error) {
            stopped = true;
            assert(std::string_view(error.what()).find(
                "procedure instruction limit exceeded") !=
                std::string_view::npos);
        }
        assert(stopped);
        assert(output.content().find("procedure stopped; remaining input:") !=
               std::string::npos);

        output.clear();
        runner.execute_buffer("child");
        runner.execute_buffer("child");
        assert(output.content() == "child\nchild\n");
    }

    std::cout << "procedure jump/label tests passed\n";
}
