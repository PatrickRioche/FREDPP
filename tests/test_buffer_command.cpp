#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/parser/ParseError.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

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
        assert(false && "expected parse error");
    } catch (const fred::ParseError& error) {
        assert(std::string_view(error.what()) == expected);
    }
}

} // namespace

int main() {
    {
        const auto node = parse("B(travail)");
        assert(node->kind() == fred::AstNodeKind::BufferCommand);
        const auto& command = static_cast<const fred::BufferCommandNode&>(*node);
        assert(command.buffer_name() == "travail");
    }
    {
        const auto node = parse("b(mon buffer)");
        const auto& command = static_cast<const fred::BufferCommandNode&>(*node);
        assert(command.buffer_name() == "mon buffer");
    }

    expect_parse_error("B", "B requires a buffer name in parentheses");
    expect_parse_error("B()", "buffer name must not be empty");
    expect_parse_error("1B(test)", "B does not accept a line address");
    expect_parse_error("B(test", "unterminated buffer name");

    fred::BufferManager manager;
    NullOutput output;
    fred::ExecutionContext context(manager, output);
    fred::CommandExecutor executor;

    auto first = parse("B(alpha)");
    executor.execute(*first, context);
    assert(manager.current().name() == "alpha");
    assert(manager.current().current_line() == 0);

    manager.current().append("A1");
    manager.current().append("A2");
    manager.current().set_current_line(1);

    auto second = parse("B(beta)");
    executor.execute(*second, context);
    assert(manager.current().name() == "beta");
    manager.current().append("B1");

    auto back = parse("B(alpha)");
    executor.execute(*back, context);
    assert(manager.current().name() == "alpha");
    assert(manager.current().line_count() == 2);
    assert(manager.current().current_line() == 1);

    auto transient = parse("B(temp)");
    executor.execute(*transient, context);
    assert(manager.contains("temp"));
    executor.execute(*back, context);
    assert(!manager.contains("temp"));

    bool limit_rejected = false;
    try {
        auto too_long = parse("B(123456789012345)");
        executor.execute(*too_long, context);
    } catch (const std::exception&) {
        limit_rejected = true;
    }
    assert(limit_rejected);

    std::cout << "buffer command tests passed\n";
}
