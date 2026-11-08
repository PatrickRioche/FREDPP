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
    for (auto line : lines) {
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

        auto& target = buffers.create_or_select("target");
        fill(target, {"old-1", "old-2"});
        target.set_current_line(1);
        target.mark_clean();
        buffers.select("source");

        execute("1,2ZK(target)", executor, context);

        require(buffers.current().name() == "source",
                "ZK retains source selection");
        require(source.line_count() == 3 &&
                source.line(1) == "alpha" &&
                source.line(2) == "beta" &&
                source.line(3) == "gamma" &&
                source.current_line() == 2 &&
                !source.modified(),
                "ZK preserves source text/dirty flag and selects last source");
        require(target.line_count() == 4 &&
                target.line(1) == "old-1" &&
                target.line(2) == "alpha" &&
                target.line(3) == "beta" &&
                target.line(4) == "old-2" &&
                target.current_line() == 3 &&
                target.modified(),
                "ZK inserts after current target line, preserving original text");
        require(output.empty(), "ZK is silent");

        bool rejected = false;
        try {
            execute("99ZK(target)", executor, context);
        } catch (const std::exception&) {
            rejected = true;
        }
        require(rejected && source.current_line() == 2 &&
                target.line_count() == 4 &&
                target.line(2) == "alpha" &&
                target.current_line() == 3,
                "invalid ZK address has no side effects");
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

        execute("zk(new)", executor, context);
        const auto& target = buffers.get("new");
        require(target.line_count() == 1 &&
                target.line(1) == "two" &&
                target.current_line() == 1,
                "unaddressed ZK copies current source line into new buffer");
        require(source.current_line() == 2 && !source.modified() &&
                buffers.current().name() == "source",
                "unaddressed ZK keeps source and selection");
    }

    {
        fred::BufferManager buffers;
        fred::StringOutput output;
        fred::ExecutionContext context(buffers, output);
        fred::CommandExecutor executor;
        auto& self = buffers.create_or_select("self");
        fill(self, {"A", "B", "C", "D"});
        self.set_current_line(4);
        self.mark_clean();

        execute("1,2ZK(self)", executor, context);
        require(self.line_count() == 6 &&
                self.line(1) == "A" && self.line(2) == "B" &&
                self.line(3) == "C" && self.line(4) == "D" &&
                self.line(5) == "A" && self.line(6) == "B" &&
                self.current_line() == 6 && self.modified(),
                "ZK permits nonoverlapping self-copy with snapshot");
    }

    {
        fred::BufferManager buffers;
        fred::StringOutput output;
        fred::ExecutionContext context(buffers, output);
        fred::CommandExecutor executor;
        auto& self = buffers.create_or_select("self");
        fill(self, {"A", "B", "C"});
        self.set_current_line(2);
        self.mark_clean();

        bool rejected = false;
        try {
            execute("1,2ZK(self)", executor, context);
        } catch (const std::exception&) {
            rejected = true;
        }
        require(rejected && self.line_count() == 3 &&
                self.current_line() == 2 && !self.modified() &&
                self.line(1) == "A" && self.line(2) == "B",
                "self-copy including current line is rejected without mutation");
    }

    {
        fred::BufferManager buffers;
        fred::StringOutput output;
        fred::ExecutionContext context(buffers, output);
        fred::CommandExecutor executor;
        auto& empty = buffers.create_or_select("empty");
        bool rejected = false;
        try {
            execute("ZK(new)", executor, context);
        } catch (const std::exception&) {
            rejected = true;
        }
        require(rejected && empty.empty() && !buffers.contains("new"),
                "empty source is rejected without creating destination");
    }

    return EXIT_SUCCESS;
}
