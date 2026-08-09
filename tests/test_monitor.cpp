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
#include <memory>
#include <string_view>

namespace {

std::unique_ptr<fred::CommandNode> parse(std::string_view source) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    return parser.parse();
}

void execute(std::string_view source,
             fred::CommandExecutor& executor,
             fred::ExecutionContext& context) {
    auto node = parse(source);
    executor.execute(*node, context);
}

} // namespace

int main() {
    {
        const auto node = parse("O+M");
        assert(node->kind() == fred::AstNodeKind::OptionCommand);
        const auto& option =
            static_cast<const fred::OptionCommandNode&>(*node);
        assert(option.option() == fred::OptionKind::Monitor);
        assert(option.enabled());
    }

    {
        const auto node = parse("o-m");
        const auto& option =
            static_cast<const fred::OptionCommandNode&>(*node);
        assert(option.option() == fred::OptionKind::Monitor);
        assert(!option.enabled());
    }

    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    assert(!context.monitor_commands());

    execute("FO", executor, context);
    assert(output.content() == "o+i(\no-m\n");
    output.clear();

    execute("O+M", executor, context);
    assert(context.monitor_commands());

    execute("FO", executor, context);
    assert(output.content() == "o+i(\no+m\n");
    output.clear();

    execute("O-M", executor, context);
    assert(!context.monitor_commands());

    return 0;
}
