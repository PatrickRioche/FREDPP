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

    {
        const auto node = parse("f?");
        assert(node->kind() == fred::AstNodeKind::FactsCommand);
        const auto& facts = static_cast<const fred::FactsCommandNode&>(*node);
        assert(facts.facts() == fred::FactsKind::Modified);
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

    // F? always shows the selected Buffer first, then other modified Buffers
    // in name order, including modified Buffers without an associated file.
    fred::BufferManager modified_buffers;
    fred::StringOutput modified_output;
    fred::ExecutionContext modified_context(modified_buffers, modified_output);

    execute("F?", executor, modified_context);
    assert(modified_output.content() == "b(0)\n");
    modified_output.clear();

    auto& edited_alpha = modified_buffers.create_or_select("alpha");
    edited_alpha.append("A");
    edited_alpha.associate_file("alpha file.txt", fred::TextEncoding::Utf8,
                                fred::LineEnding::Lf, true);

    auto& clean_beta = modified_buffers.create_or_select("beta");
    clean_beta.append("B");
    clean_beta.associate_file("beta.txt", fred::TextEncoding::Utf8,
                              fred::LineEnding::Lf, true);
    clean_beta.mark_clean();

    auto& edited_zeta = modified_buffers.get_or_create("zeta");
    edited_zeta.append("Z");
    auto& edited_aardvark = modified_buffers.get_or_create("aardvark");
    edited_aardvark.append("a");
    edited_aardvark.associate_file("path with spaces.txt",
                                   fred::TextEncoding::Utf8,
                                   fred::LineEnding::Lf, true);

    modified_buffers.select("beta");
    modified_context.set_counter(73);
    modified_context.set_condition(true);
    const auto before_cursor = modified_buffers.current().current_line();
    execute("f?", executor, modified_context);
    assert(modified_output.content() ==
           "b(beta) beta.txt\n"
           "b(aardvark) path with spaces.txt ?\n"
           "b(alpha) alpha file.txt ?\n"
           "b(zeta) ?\n");
    assert(modified_buffers.current().name() == "beta");
    assert(modified_buffers.current().current_line() == before_cursor);
    assert(modified_context.counter() == 73);
    assert(modified_context.condition());
    assert(!clean_beta.modified());

    modified_output.clear();
    modified_buffers.select("alpha");
    execute("F?", executor, modified_context);
    assert(modified_output.content() ==
           "b(alpha) alpha file.txt ?\n"
           "b(aardvark) path with spaces.txt ?\n"
           "b(zeta) ?\n");
    return 0;
}
