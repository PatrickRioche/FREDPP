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
#include <string>
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
    auto command = parse(source);
    executor.execute(*command, context);
}

} // namespace

int main() {
    {
        const auto node = parse("fb");
        assert(node->kind() == fred::AstNodeKind::FactsCommand);
        const auto& facts =
            static_cast<const fred::FactsCommandNode&>(*node);
        assert(facts.facts() == fred::FactsKind::Buffers);
    }

    {
        const auto node = parse("FO");
        assert(node->kind() == fred::AstNodeKind::FactsCommand);
        const auto& facts =
            static_cast<const fred::FactsCommandNode&>(*node);
        assert(facts.facts() == fred::FactsKind::Options);
    }

    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    execute("FO", executor, context);
    assert(output.content() == "o+i(\no-m\n");
    output.clear();

    execute("O-I(", executor, context);
    execute("fo", executor, context);
    assert(output.content() == "o-i(\no-m\n");
    output.clear();

    auto& alpha = buffers.create_or_select("alpha");
    alpha.append("A1");
    alpha.associate_file("alpha.txt", fred::TextEncoding::Utf8,
                         fred::LineEnding::Lf, true);

    auto& beta = buffers.create_or_select("beta");
    beta.append("B1");
    beta.mark_clean();

    buffers.select("alpha");

    execute("FB", executor, context);

    const std::string expected =
        "b(alpha) 1,1 alpha.txt ?\n"
        "b(beta) 1,1\n"
        "b(0) 0,0\n";

    assert(output.content() == expected);
    return 0;
}
