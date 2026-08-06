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

void append_sample(fred::Buffer& buffer) {
    buffer.append("alpha");
    buffer.append("beta");
    buffer.append("gamma");
}

} // namespace

int main() {
    fred::BufferManager buffers;
    StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    append_sample(buffers.current());

    {
        const auto command = parse("*");
        executor.execute(*command, context);
        assert(output.content() == "alpha\nbeta\ngamma\n");
        assert(buffers.current().current_line() == 3);
        output.clear();
    }

    {
        const auto command = parse("*p");
        executor.execute(*command, context);
        assert(output.content() == "alpha\nbeta\ngamma\n");
        output.clear();
    }

    {
        const auto command = parse("*D");
        executor.execute(*command, context);
        assert(buffers.current().empty());
        assert(buffers.current().current_line() == 0);
    }

    return 0;
}
