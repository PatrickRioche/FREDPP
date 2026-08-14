#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

namespace {

std::unique_ptr<fred::CommandNode> parse(
    std::string_view source,
    const fred::CommandRegistry& registry) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    fred::CommandParser parser(tokens, registry);
    return parser.parse();
}

} // namespace

int main() {
    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;
    const auto registry = fred::make_core_command_registry();

    {
        const auto command = parse("!echo FREDPP_SYSTEM", registry);
        executor.execute(*command, context);
        assert(output.content().find("FREDPP_SYSTEM") != std::string::npos);
        assert(context.condition());
    }

    output.clear();

    {
        const auto command =
            parse("ZG(capture)!echo FREDPP_ZG", registry);
        executor.execute(*command, context);
        assert(output.empty());
        assert(buffers.contains("capture"));

        const auto& capture = buffers.get("capture");
        assert(!capture.empty());

        bool found = false;
        for (const auto& line : capture.lines()) {
            if (line.find("FREDPP_ZG") != std::string::npos) {
                found = true;
                break;
            }
        }
        assert(found);
    }

    {
        auto& flow_value = buffers.create_or_select("flow-s");
        flow_value.append("FREDPP_FLOW_SYSTEM");

        const auto command =
            parse("ZG(flowcap)!echo \\S(flow-s)", registry);
        executor.execute(*command, context);

        assert(buffers.contains("flowcap"));
        const auto& capture = buffers.get("flowcap");
        assert(!capture.empty());

        bool found = false;
        for (const auto& line : capture.lines()) {
            if (line.find("FREDPP_FLOW_SYSTEM") != std::string::npos) {
                found = true;
                break;
            }
        }
        assert(found);
    }

    {
        const auto command = parse("JM/RESTORED/", registry);
        executor.execute(*command, context);
        assert(output.content() == "RESTORED\n");
    }

    std::cout << "System command and ZG tests passed.\n";
    return EXIT_SUCCESS;
}
