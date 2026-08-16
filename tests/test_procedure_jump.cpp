#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"
#include "fred/runtime/ProcedureRunner.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

std::string run_lines(std::vector<std::string> lines,
                      bool condition) {
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
        buffers, context, registry, executor);

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

    std::cout << "procedure jump/label tests passed\n";
}
