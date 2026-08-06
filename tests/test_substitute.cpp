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

    return 0;
}
