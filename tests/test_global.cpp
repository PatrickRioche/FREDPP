#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
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

    buffers.current().append("alpha");
    buffers.current().append("recherche");
    buffers.current().append("beta recherche");
    buffers.current().append("omega");

    {
        const auto command = parse("1,$G/recherche/P");
        executor.execute(*command, context);
        assert(output.content() == "recherche\nbeta recherche\n");
        assert(buffers.current().current_line() == 3);
        assert(context.counter() == 2);
        output.clear();
    }

    {
        const auto command = parse("1,$G~/recherche$/D");
        executor.execute(*command, context);
        assert(buffers.current().line_count() == 2);
        assert(buffers.current().line(1) == "recherche");
        assert(buffers.current().line(2) == "beta recherche");
        assert(context.counter() == 2);
    }

    {
        const auto command = parse("G/^recherche$/Z");
        executor.execute(*command, context);
        assert(buffers.current().current_line() == 1);
        assert(context.counter() == 1);
    }

    {
        const auto command = parse("G/recherche/B(other)");
        try {
            executor.execute(*command, context);
            assert(false && "unsupported nested command should fail");
        } catch (const fred::CommandExecutionError& error) {
            assert(std::string_view(error.what()) ==
                   "G currently supports nested P, D, Z and S commands");
        }
    }
}
