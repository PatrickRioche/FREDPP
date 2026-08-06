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
#include <string_view>

namespace {

class NullOutput final : public fred::Output {
public:
    void write(std::string_view) override {}
};

std::unique_ptr<fred::CommandNode> parse(std::string_view source) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    return parser.parse();
}

void expect_parse_error(std::string_view source, std::string_view expected) {
    try {
        (void)parse(source);
        assert(false && "expected ParseError");
    } catch (const fred::ParseError& error) {
        assert(std::string_view(error.what()) == expected);
    }
}

} // namespace

int main() {
    fred::BufferManager buffers;
    NullOutput output;
    fred::CommandExecutor executor;

    {
        fred::ExecutionContext context(buffers, output);
        const auto command = parse("q");
        assert(command->kind() == fred::AstNodeKind::QuitCommand);
        const auto& quit = static_cast<const fred::QuitCommandNode&>(*command);
        assert(!quit.immediate());
        executor.execute(*command, context);
        assert(context.exit_requested());
        assert(!context.immediate_exit_requested());
    }

    buffers.current().append("unsaved");
    {
        fred::ExecutionContext context(buffers, output);
        const auto command = parse("Q");
        try {
            executor.execute(*command, context);
            assert(false && "Q must refuse modified buffers");
        } catch (const fred::CommandExecutionError& error) {
            assert(std::string_view(error.what()).find("modified buffer") !=
                   std::string_view::npos);
        }
        assert(!context.exit_requested());
    }

    {
        fred::ExecutionContext context(buffers, output);
        const auto command = parse("qQ");
        assert(command->kind() == fred::AstNodeKind::QuitCommand);
        const auto& quit = static_cast<const fred::QuitCommandNode&>(*command);
        assert(quit.immediate());
        executor.execute(*command, context);
        assert(context.exit_requested());
        assert(context.immediate_exit_requested());
    }

    expect_parse_error("1Q", "Q does not accept a line address");
    expect_parse_error(
        "Q!bye",
        "Q! requires external TSS command support (not implemented yet)");

    return 0;
}
