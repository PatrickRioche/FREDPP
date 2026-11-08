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

    {
        const auto node = parse("fF");
        assert(node->kind() == fred::AstNodeKind::FactsCommand);
        const auto& facts =
            static_cast<const fred::FactsCommandNode&>(*node);
        assert(facts.facts() == fred::FactsKind::Files);
    }

    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    // With no associated files, FF must be silent and leave state untouched.
    execute("FF", executor, context);
    assert(output.content().empty());

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
    output.clear();

    // The never-selected gamma buffer must still appear in FF. The
    // unassociated beta and the initial buffer 0 must not appear.
    auto& gamma = buffers.get_or_create("gamma");
    gamma.associate_file("gamma file.txt", fred::TextEncoding::Utf8,
                         fred::LineEnding::Lf, true);
    gamma.mark_clean();

    context.set_counter(37);
    context.set_condition(true);
    const auto selected_before = buffers.current().name();
    const auto cursor_before = buffers.current().current_line();

    execute("ff", executor, context);
    const std::string file_facts =
        "b(alpha) alpha.txt ?\n"
        "b(gamma) gamma file.txt\n";
    assert(output.content() == file_facts);
    assert(buffers.current().name() == selected_before);
    assert(buffers.current().current_line() == cursor_before);
    assert(context.counter() == 37);
    assert(context.condition());
    assert(alpha.modified());
    assert(!gamma.modified());

    output.clear();
    execute("FF", executor, context);
    assert(output.content() == file_facts);
    return 0;
}
