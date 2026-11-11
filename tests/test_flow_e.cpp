#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/flow/CommandInputExpansion.hpp"
#include "fred/flow/FlowCharacterStream.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cassert>
#include <stdexcept>
#include <string_view>

int main() {
    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);
    const auto registry = fred::make_core_command_registry();
    fred::CommandExecutor executor;

    const auto execute = [&](std::string_view source) {
        auto expanded = fred::expand_command_input_with_metadata(source, context);
        fred::FlowCharacterStream characters(std::move(expanded.characters));
        fred::Lexer lexer(characters);
        fred::TokenStream tokens(lexer);
        fred::CommandParser parser(tokens, registry, &context);
        auto command = parser.parse();
        executor.execute(*command, context);
    };

    buffers.current().append("cat123");
    execute("E(d)/[0-9]+/");
    execute("E(p)/cat|dog/");
    execute("T/\\E(d)/");
    assert(context.condition());
    execute("T/^\\E(p)[0-9]+$/");
    assert(context.condition());

    // A named alternation must be grouped when embedded in larger patterns.
    execute("T/^pre\\E(p)suffix$/");
    assert(!context.condition());
    execute("S/\\E(d)/X/");
    assert(buffers.current().line(1) == "catX");

    // E is not special in message bodies, even when a pattern is defined.
    output.clear();
    execute("JM!literal=\\E(d)!");
    assert(output.content() == "literal=\\E(d)\n");

    // An undefined reference must not match arbitrary user text.
    execute("T/\\E(undefined)/");
    assert(!context.condition());

    // Names can reference other defined patterns without pre-expansion.
    execute("E(outer)/\\E(p)[0-9]+/");
    buffers.current().replace(1, "dog42");
    execute("T/^\\E(outer)$/");
    assert(context.condition());

    // Escaped pattern directives are literals, not a named lookup.
    execute("T/\\\\E(d)/");
    assert(!context.condition());

    // An E(name) command removes the definition; the resulting reference
    // is a known non-matching pattern, not an arbitrary-text expansion.
    execute("E(d)");
    assert(!context.named_pattern_source("d").has_value());
    execute("T/\E(d)/");
    assert(!context.condition());

    // A direct recursive named definition must fail with a clear bound.
    execute("E(loop)/\\E(loop)/");
    bool recursive_rejected = false;
    try {
        execute("T/\\E(loop)/");
    } catch (const std::exception& error) {
        recursive_rejected =
            std::string_view(error.what()).find("recursive E") !=
            std::string_view::npos;
    }
    assert(recursive_rejected);

    return 0;
}
