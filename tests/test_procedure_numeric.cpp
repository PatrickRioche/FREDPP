#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"
#include "fred/runtime/ProcedureRunner.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

struct RunResult {
    std::string output;
    std::int64_t np{};
    std::int64_t counter{};
    bool condition{};
};

RunResult run_case(
    std::size_t parameter_lines,
    std::vector<std::string> procedure_lines) {
    fred::BufferManager buffers;

    auto& parameters = buffers.create_or_select("0");
    for (std::size_t index = 0; index < parameter_lines; ++index) {
        parameters.append("arg");
    }

    auto& procedure = buffers.get_or_create("proc");
    for (auto& line : procedure_lines) {
        procedure.append(std::move(line));
    }

    buffers.select("0");

    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);

    const auto registry = fred::make_core_command_registry();
    fred::CommandExecutor executor;
    fred::ProcedureRunner runner(
        buffers, context, registry, executor);

    runner.execute_buffer("proc");

    return {
        output.content(),
        context.numeric_register("np"),
        context.counter(),
        context.condition(),
    };
}

void expect_error(
    std::vector<std::string> procedure_lines,
    std::string_view expected) {
    try {
        (void)run_case(0, std::move(procedure_lines));
        assert(false && "expected procedure error");
    } catch (const std::runtime_error& error) {
        assert(std::string_view(error.what()) == expected);
    }
}

} // namespace

int main() {
    {
        const auto result = run_case(2, {"N(np):$>0"});
        assert(result.np == 2);
        assert(result.counter == 2);
        assert(result.condition);
    }

    {
        const auto result = run_case(0, {"N(np):$>0"});
        assert(result.np == 0);
        assert(result.counter == 0);
        assert(!result.condition);
    }

    {
        const auto result = run_case(3, {"N(np):$=3"});
        assert(result.np == 3);
        assert(result.condition);
    }

    {
        const auto result = run_case(1, {"N(np):$<2"});
        assert(result.np == 1);
        assert(result.condition);
    }

    {
        const auto result = run_case(0, {"N(np):+12=12"});
        assert(result.np == 12);
        assert(result.condition);
    }

    {
        const auto result = run_case(0, {"N(np):-3<0"});
        assert(result.np == -3);
        assert(result.counter == -3);
        assert(result.condition);
    }

    {
        const auto result = run_case(0, {
            "N(zeta):10",
            "N(alpha):-3",
            "N(empty):0",
            "FN",
        });
        assert(result.output ==
               "n(alpha):-3\n"
               "n(empty):0\n"
               "n(zeta):10\n");
    }

    // Procedure-only chained syntax must match interactive N operations.
    {
        const auto result = run_case(2, {
            "N(np):5^2~P",
            "N(np):7{2}1P",
            "N(np):7D4F.B2P",
            "N(np)A P",
            "N(np)L P",
            "1,$N(np)L",
            "N(np)P",
        });
        assert(result.output == "-8\n14\n.111\n..10\n..11\n.110\n");
        assert(result.np == 6 && result.counter == 6);
    }
    {
        const auto result = run_case(0, {"N(np):7D3FxP"});
        assert(result.output == "xx7\n" && result.np == 7);
    }

    {
        const auto result = run_case(1, {
            "N(np):$>0 J(param)T",
            "JM/SHOULD-NOT-PRINT/",
            "@(param)",
            "JM/PARAM-OK/",
        });
        assert(result.output == "PARAM-OK\n");
    }

    {
        const auto result = run_case(0, {
            "N(np):$>0 J(param)T",
            "JM/NO-PARAM/",
            "J(done)",
            "@(param)",
            "JM/SHOULD-NOT-PRINT/",
            "@(done)",
            "JM/DONE/",
        });
        assert(result.output == "NO-PARAM\nDONE\n");
    }

    expect_error(
        {"N():1"},
        "numeric register name must not be empty");

    expect_error(
        {"N(123456789012345):1"},
        "numeric register name exceeds historical limit of 14 characters");

    {
        const auto result = run_case(0, {
            "N(rhs):5", "N(np):8", "N(np)+\\N(rhs)",
            "N(np)*2", "N(np)-6", "N(np)/4", "N(np)%3",
            "N(np)&3", "N(np)|8", "N(np)=10", "N(np)>9", "N(np)<11"
        });
        assert(result.np == 10);
        assert(result.counter == 10);
        assert(result.condition);
    }
    {
        const auto result = run_case(0, {
            "N(rhs):41",
            "JM/value=\\N(rhs)/",
            "N(rhs)+1",
            "JM/updated=\\N(rhs)/",
            "N(rhs)B2",
            "JM/formatted=\\N(rhs)/",
            "N(rhs)B10",
            "N(np):\\N(rhs)",
            "N(np)P",
        });
        assert(result.output == "value=41\nupdated=42\nformatted=101010\n42\n");
        assert(result.np == 42);
    }
    {
        const auto result = run_case(0, {
            "N(rhs):-3",
            "N(np):5",
            "N(np)+\\N(rhs)",
            "JM/negative=\\N(rhs)/",
        });
        assert(result.np == 2);
        assert(result.output == "negative=-3\n");
    }
    {
        const auto result = run_case(0, {"N(np):-12", "N(np)|"});
        assert(result.np == 12);
    }

    expect_error({"N(np):5", "N(np)/0"}, "numeric division by zero");
    expect_error({"N(np):9223372036854775807", "N(np)+1"},
                 "numeric addition overflow");

    expect_error(
        {"N(np):1 JM/not-yet/"},
        "Lot 4 allows only J(label)[T|F] after N on the same line");

    std::cout << "procedure numeric minimal tests passed\n";
}
