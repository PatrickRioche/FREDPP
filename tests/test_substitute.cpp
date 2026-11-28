#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/parser/ParseError.hpp"
#include "fred/runtime/CommandExecutionError.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <string_view>

namespace {

class StringOutput final : public fred::Output {
public:
    void write(std::string_view text) override { content_.append(text); }
    [[nodiscard]] const std::string& content() const noexcept { return content_; }
    void clear() noexcept { content_.clear(); }

private:
    std::string content_;
};

std::unique_ptr<fred::CommandNode> parse(std::string_view source) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    return parser.parse();
}

} // namespace

int main() {
    fred::BufferManager buffers;
    StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    buffers.current().append("alpha alpha");
    {
        const auto command = parse("S!alpha!omega!");
        assert(command->kind() == fred::AstNodeKind::SubstituteCommand);
        executor.execute(*command, context);
        assert(buffers.current().line(1) == "omega omega");
        assert(buffers.current().current_line() == 1);
        assert(context.condition());
    }

    buffers.current().append("beta");
    buffers.current().append("gamma");
    {
        const auto command = parse("1,2S/$/!/");
        executor.execute(*command, context);
        assert(buffers.current().line(1) == "omega omega!");
        assert(buffers.current().line(2) == "beta!");
        assert(buffers.current().line(3) == "gamma");
        assert(buffers.current().current_line() == 2);
    }

    {
        const auto command = parse("3S/.*/& &/P");
        executor.execute(*command, context);
        assert(buffers.current().line(3) == "gamma gamma");
        assert(output.content() == "gamma gamma\n");
        output.clear();
    }

    {
        const auto command = parse("2S?beta??");
        executor.execute(*command, context);
        assert(buffers.current().line(2) == "!");
    }

    {
        const auto command = parse("1S/omega/chemin\\/omega/");
        executor.execute(*command, context);
        assert(buffers.current().line(1) == "chemin/omega chemin/omega!");
    }

    {
        const auto command = parse("1S;^;> ;");
        executor.execute(*command, context);
        assert(buffers.current().line(1) == "> chemin/omega chemin/omega!");
    }

    {
        const auto command = parse("1S/introuvable/x/");
        try {
            executor.execute(*command, context);
            assert(false && "missing substitution should fail");
        } catch (const fred::CommandExecutionError& error) {
            assert(std::string_view(error.what()) == "no text changed");
            assert(!context.condition());
        }
    }

    (void)buffers.create_or_select("global");
    buffers.current().append("alpha one");
    buffers.current().append("skip");
    buffers.current().append("alpha two");
    {
        const auto command = parse("1,$G/alpha/S/alpha/x/P");
        executor.execute(*command, context);
        assert(buffers.current().line(1) == "x one");
        assert(buffers.current().line(2) == "skip");
        assert(buffers.current().line(3) == "x two");
        assert(output.content() == "x one\nx two\n");
        assert(context.counter() == 2);
        output.clear();
    }

    {
        const auto command = parse("*S/x/X/");
        executor.execute(*command, context);
        assert(buffers.current().line(1) == "X one");
        assert(buffers.current().line(3) == "X two");
    }

    (void)buffers.create_or_select("occurrences");
    auto& buffer = buffers.current();
    buffer.append("Ajout  ligne de texte numero 1");
    executor.execute(*parse("1s/ligne/colonne/"), context);
    executor.execute(*parse("1s/.*/& &/"), context);
    executor.execute(*parse("1s1/colonne/ligne/p"), context);
    assert(output.content() ==
           "Ajout  ligne de texte numero 1 Ajout  colonne de texte numero 1\n");
    executor.execute(*parse("1s-1/de texte.*$//"), context);
    assert(buffer.line(1) == "Ajout  ligne de texte numero 1 Ajout  colonne ");

    buffer.append("x x x x");
    buffer.append("x x x");
    executor.execute(*parse("2,3s2/x/y/"), context);
    executor.execute(*parse("2,3s-2/x/z/"), context);
    assert(buffer.line(2) == "x y z x");
    assert(buffer.line(3) == "z y x");
    try {
        executor.execute(*parse("2s-3/x/o/"), context);
        assert(false && "missing numbered occurrence should fail");
    } catch (const fred::CommandExecutionError& error) {
        assert(std::string_view(error.what()) == "no text changed");
        assert(buffer.line(2) == "x y z x");
        assert(!context.condition());
    }

    // The dev options must also apply to numbered substitutions.
    output.clear();
    (void)buffers.create_or_select("occurrence_options");
    auto& options_buffer = buffers.current();
    options_buffer.append("CHAT chat Chat");
    context.set_case_insensitive_search(true);
    executor.execute(*parse("1s-2/chat/[&]/p"), context);
    assert(options_buffer.line(1) == "CHAT [chat] Chat");
    assert(output.content() == "CHAT [chat] Chat\n");
    output.clear();

    context.set_substitution_ampersand(false);
    executor.execute(*parse("1s1/chat/&/"), context);
    assert(options_buffer.line(1) == "& [chat] Chat");

    const auto unclosed = parse("1s-1/chat/Z");
    try {
        executor.execute(*unclosed, context);
        assert(false && "O+S/ must reject an unclosed numbered replacement");
    } catch (const fred::CommandExecutionError&) {
        assert(options_buffer.line(1) == "& [chat] Chat");
    }
    context.set_substitution_separator_required(false);
    executor.execute(*unclosed, context);
    assert(options_buffer.line(1) == "& [chat] Z");
    context.set_substitution_separator_required(true);
    context.set_substitution_ampersand(true);
    context.set_case_insensitive_search(false);

    // A '-' delimiter followed by a numeric pattern keeps its existing meaning.
    options_buffer.append("2 2");
    executor.execute(*parse("2s-2-X-"), context);
    assert(options_buffer.line(2) == "X X");

    // Empty matches progress in either direction and preserve intervening text.
    options_buffer.append("abc");
    executor.execute(*parse("3s2/x*/Q/"), context);
    assert(options_buffer.line(3) == "aQbc");
    executor.execute(*parse("3s-2/x*/Z/"), context);
    assert(options_buffer.line(3) == "aQbZc");

    for (const auto source : {"1s0/x/y/", "1s-0/x/y/", "1s999999999999/x/y/"}) {
        try {
            (void)parse(source);
            assert(false && "zero and overflowing occurrence ranks must be rejected");
        } catch (const fred::ParseError&) {
        }
    }

    return 0;
}
