#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cstdlib>
#include <exception>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void execute(std::string_view source,
             fred::CommandExecutor& executor,
             fred::ExecutionContext& context) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    const auto command = parser.parse();
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
        fill(source, {"alpha", "beta", "gamma"});
        source.mark_clean();

        auto& destination = buffers.create_or_select("target");
        fill(destination, {"obsolete", "old"});
        destination.mark_clean();
        buffers.select("source");

        execute("1,2K(target)", executor, context);

        require(buffers.current().name() == "source",
                "K preserves the selected source buffer");
        require(source.line_count() == 3 &&
                source.line(1) == "alpha" &&
                source.line(2) == "beta" &&
                source.line(3) == "gamma",
                "K preserves all source text");
        require(source.current_line() == 2 && !source.modified(),
                "K sets source cursor but keeps its clean state");
        require(destination.line_count() == 2 &&
                destination.line(1) == "alpha" &&
                destination.line(2) == "beta" &&
                destination.current_line() == 2 &&
                destination.modified(),
                "K replaces destination and selects its last line");
        require(output.empty(), "K does not print");

        // An invalid source address must not modify either Buffer.
        bool rejected = false;
        try {
            execute("99K(target)", executor, context);
        } catch (const std::exception&) {
            rejected = true;
        }
        require(rejected && source.line_count() == 3 &&
                source.current_line() == 2 &&
                destination.line_count() == 2 &&
                destination.line(1) == "alpha",
                "invalid range preserves both buffers");
    }

    {
        fred::BufferManager buffers;
        fred::StringOutput output;
        fred::ExecutionContext context(buffers, output);
        fred::CommandExecutor executor;
        auto& source = buffers.create_or_select("source");
        fill(source, {"one", "two", "three"});
        source.set_current_line(2);
        source.mark_clean();

        execute("k(target)", executor, context);
        const auto& destination = buffers.get("target");
        require(destination.line_count() == 1 &&
                destination.line(1) == "two" &&
                destination.current_line() == 1,
                "unaddressed K copies the current line");
        require(source.current_line() == 2 && !source.modified(),
                "unaddressed K leaves source content and modification state");
    }

    {
        fred::BufferManager buffers;
        fred::StringOutput output;
        fred::ExecutionContext context(buffers, output);
        fred::CommandExecutor executor;
        auto& source = buffers.create_or_select("self");
        fill(source, {"A", "B", "C"});
        source.mark_clean();

        execute("2K(self)", executor, context);
        require(source.line_count() == 1 &&
                source.line(1) == "B" &&
                source.current_line() == 1 &&
                source.modified(),
                "same-buffer K snapshots and replaces content");
    }

    {
        fred::BufferManager buffers;
        fred::StringOutput output;
        fred::ExecutionContext context(buffers, output);
        fred::CommandExecutor executor;
        auto& source = buffers.create_or_select("empty");
        bool rejected = false;
        try {
            execute("K(target)", executor, context);
        } catch (const std::exception&) {
            rejected = true;
        }
        require(rejected && source.empty() &&
                !buffers.contains("target"),
                "K rejects an empty source without creating destination");
    }

    return EXIT_SUCCESS;
}
