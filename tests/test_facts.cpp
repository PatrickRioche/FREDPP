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
        const auto node = parse("f");
        assert(node->kind() == fred::AstNodeKind::FactsCommand);
        const auto& facts = static_cast<const fred::FactsCommandNode&>(*node);
        assert(facts.facts() == fred::FactsKind::Current);
    }
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
        const auto node = parse("Fn");
        assert(node->kind() == fred::AstNodeKind::FactsCommand);
        const auto& facts =
            static_cast<const fred::FactsCommandNode&>(*node);
        assert(facts.facts() == fred::FactsKind::Registers);
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

    execute("F", executor, context);
    assert(output.content() == "b(0) 0,0\n");
    output.clear();

    // With no associated files, FF must be silent and leave state untouched.
    execute("FF", executor, context);
    assert(output.content().empty());

    // FN has no header or placeholder when no register was ever created.
    execute("FN", executor, context);
    assert(output.content().empty());

    // FN lists created registers, including zeros, in name order. It neither
    // creates missing registers nor changes their values or procedure state.
    context.set_numeric_register("zeta", -17);
    context.set_numeric_register("alpha", 0);
    context.set_numeric_register("middle", 42);
    context.set_counter(99);
    context.set_condition(true);
    const auto initial_name = buffers.current().name();
    const auto initial_cursor = buffers.current().current_line();
    execute("fn", executor, context);
    assert(output.content() ==
           "n(alpha):0\n"
           "n(middle):42\n"
           "n(zeta):-17\n");
    assert(context.numeric_register_names().size() == 3);
    assert(!context.has_numeric_register("missing"));
    assert(context.numeric_register("zeta") == -17);
    assert(context.counter() == 99);
    assert(context.condition());
    assert(buffers.current().name() == initial_name);
    assert(buffers.current().current_line() == initial_cursor);
    output.clear();
    context.set_numeric_register("middle", -1);
    execute("FN", executor, context);
    assert(output.content() ==
           "n(alpha):0\n"
           "n(middle):-1\n"
           "n(zeta):-17\n");
    output.clear();

    execute("FO", executor, context);
    assert(output.content() == "o+i(\no-m\no-q\no+s&\no-sd\no+t5,9\n");
    output.clear();

    execute("O-I(", executor, context);
    execute("fo", executor, context);
    assert(output.content() == "o-i(\no-m\no-q\no+s&\no-sd\no+t5,9\n");
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

    // Bare F shows the current Buffer's FB-style status but does not alter it.
    modified_output.clear();
    execute("f", executor, modified_context);
    assert(modified_output.content() == "b(alpha) 1,1 alpha file.txt ?\n");
    assert(modified_buffers.current().name() == "alpha");
    assert(edited_alpha.current_line() == 1);
    assert(edited_alpha.modified());

    // F filename changes metadata only: no content, cursor or dirty-flag reset.
    modified_output.clear();
    const auto previous_encoding = edited_alpha.encoding();
    const auto previous_line_ending = edited_alpha.line_ending();
    const auto previous_final_newline = edited_alpha.final_newline();
    execute("F \"new associated file.txt\"", executor, modified_context);
    assert(modified_output.content().empty());
    assert(edited_alpha.associated_file() &&
           *edited_alpha.associated_file() == "new associated file.txt");
    assert(edited_alpha.encoding() == previous_encoding);
    assert(edited_alpha.line_ending() == previous_line_ending);
    assert(edited_alpha.final_newline() == previous_final_newline);
    assert(edited_alpha.line_count() == 1);
    assert(edited_alpha.line(1) == "A");
    assert(edited_alpha.current_line() == 1);
    assert(edited_alpha.modified());
    assert(modified_context.counter() == 73);
    assert(modified_context.condition());

    execute("F", executor, modified_context);
    assert(modified_output.content() ==
           "b(alpha) 1,1 new associated file.txt ?\n");

    // Reassociation must work on an initially clean, fileless Buffer as well.
    modified_buffers.select("0");
    modified_output.clear();
    execute("f next file.txt", executor, modified_context);
    assert(modified_output.content().empty());
    assert(modified_buffers.current().associated_file() &&
           *modified_buffers.current().associated_file() == "next file.txt");
    assert(!modified_buffers.current().modified());
    execute("F", executor, modified_context);
    assert(modified_output.content() == "b(0) 0,0 next file.txt\n");

    // N: assigns numeric registers from interactive commands, and FN makes
    // those assignments observable without an enclosing procedure.
    fred::BufferManager numeric_buffers;
    fred::StringOutput numeric_output;
    fred::ExecutionContext numeric_context(numeric_buffers, numeric_output);
    auto& numeric_buffer = numeric_buffers.current();
    numeric_buffer.append("first");
    numeric_buffer.append("second");
    numeric_buffer.set_current_line(1);
    const auto selected_numeric_buffer = numeric_buffer.name();
    const auto numeric_modified = numeric_buffer.modified();
    numeric_context.set_condition(true);

    execute("N(alpha):5", executor, numeric_context);
    assert(numeric_context.has_numeric_register("alpha"));
    assert(numeric_context.numeric_register("alpha") == 5);
    assert(numeric_context.counter() == 5);
    execute("n(beta):10", executor, numeric_context);
    execute("N(gamma):-3", executor, numeric_context);
    execute("N(copy):#", executor, numeric_context);
    assert(numeric_context.numeric_register("copy") == -3);
    execute("N(lines):$", executor, numeric_context);
    assert(numeric_context.numeric_register("lines") == 2);
    execute("N(cursor):.", executor, numeric_context);
    assert(numeric_context.numeric_register("cursor") == 1);
    execute("N(alpha):0", executor, numeric_context);
    assert(numeric_context.counter() == 0);

    execute("FN", executor, numeric_context);
    assert(numeric_output.content() ==
           "n(alpha):0\n"
           "n(beta):10\n"
           "n(copy):-3\n"
           "n(cursor):1\n"
           "n(gamma):-3\n"
           "n(lines):2\n");
    assert(numeric_buffers.current().name() == selected_numeric_buffer);
    assert(numeric_buffer.current_line() == 1);
    assert(numeric_buffer.line_count() == 2);
    assert(numeric_buffer.line(1) == "first");
    assert(numeric_buffer.line(2) == "second");
    assert(numeric_buffer.modified() == numeric_modified);
    assert(numeric_context.condition());
    assert(numeric_context.counter() == 0);
    return 0;
}
