#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cassert>
#include <initializer_list>
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

void fill(fred::Buffer& buffer,
          std::initializer_list<std::string_view> lines) {
    for (const auto line : lines) {
        buffer.append(std::string(line));
    }
}

} // namespace

int main() {
    {
        fred::BufferManager buffers;
        fred::StringOutput output;
        fred::ExecutionContext context(buffers, output);
        fred::CommandExecutor executor;

        auto& source = buffers.create_or_select("source");
        fill(source, {"alpha", "beta", "gamma", "delta"});

        auto& destination = buffers.create_or_select("dest");
        fill(destination, {"ancien-1", "ancien-2"});

        buffers.select("source");
        execute("2,3M(dest)", executor, context);

        assert(buffers.current().name() == "source");

        const auto& source_after = buffers.get("source");
        assert(source_after.line_count() == 2);
        assert(source_after.line(1) == "alpha");
        assert(source_after.line(2) == "delta");
        assert(source_after.current_line() == 2);

        const auto& dest_after = buffers.get("dest");
        assert(dest_after.line_count() == 2);
        assert(dest_after.line(1) == "beta");
        assert(dest_after.line(2) == "gamma");
        assert(dest_after.current_line() == 2);
    }

    {
        fred::BufferManager buffers;
        fred::StringOutput output;
        fred::ExecutionContext context(buffers, output);
        fred::CommandExecutor executor;

        auto& source = buffers.create_or_select("source");
        fill(source, {"un", "deux", "trois"});
        source.set_current_line(2);

        execute("M(chemin)", executor, context);

        const auto& source_after = buffers.get("source");
        assert(source_after.line_count() == 2);
        assert(source_after.line(1) == "un");
        assert(source_after.line(2) == "trois");
        assert(source_after.current_line() == 2);

        const auto& destination = buffers.get("chemin");
        assert(destination.line_count() == 1);
        assert(destination.line(1) == "deux");
        assert(destination.current_line() == 1);
    }

    {
        fred::BufferManager buffers;
        fred::StringOutput output;
        fred::ExecutionContext context(buffers, output);
        fred::CommandExecutor executor;

        auto& source = buffers.create_or_select("source");
        fill(source, {"A", "B", "C"});

        execute("2M(source)", executor, context);

        const auto& after = buffers.get("source");
        assert(after.line_count() == 1);
        assert(after.line(1) == "B");
        assert(after.current_line() == 1);
        assert(buffers.current().name() == "source");
    }

    return 0;
}
