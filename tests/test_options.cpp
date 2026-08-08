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
#include <iostream>
#include <memory>
#include <string>
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

void execute(std::string_view source,
             fred::CommandExecutor& executor,
             fred::ExecutionContext& context) {
    auto command = parse(source);
    executor.execute(*command, context);
}

void expect_parse_error(std::string_view source) {
    bool rejected = false;
    try {
        (void)parse(source);
    } catch (const fred::ParseError&) {
        rejected = true;
    }
    assert(rejected);
}

void expect_execution_error(std::string_view source,
                            std::string_view expected,
                            fred::CommandExecutor& executor,
                            fred::ExecutionContext& context) {
    bool rejected = false;
    try {
        execute(source, executor, context);
    } catch (const fred::CommandExecutionError& error) {
        rejected = true;
        assert(std::string_view(error.what()) == expected);
    }
    assert(rejected);
}

} // namespace

int main() {
    {
        const auto node = parse("O-I(");
        assert(node->kind() == fred::AstNodeKind::OptionCommand);
        const auto& command =
            static_cast<const fred::OptionCommandNode&>(*node);
        assert(command.option() == fred::OptionKind::InputParenthesis);
        assert(!command.enabled());
    }

    {
        const auto node = parse("o+i(");
        const auto& command =
            static_cast<const fred::OptionCommandNode&>(*node);
        assert(command.enabled());
    }

    {
        const auto node = parse("B(a)");
        const auto& command =
            static_cast<const fred::BufferCommandNode&>(*node);
        assert(command.buffer_name() == "a");
        assert(!command.short_form());
    }

    {
        const auto node = parse("Ba");
        const auto& command =
            static_cast<const fred::BufferCommandNode&>(*node);
        assert(command.buffer_name() == "a");
        assert(command.short_form());
    }

    {
        const auto node = parse("B0");
        const auto& command =
            static_cast<const fred::BufferCommandNode&>(*node);
        assert(command.buffer_name() == "0");
        assert(command.short_form());
    }

    expect_parse_error("B 0");
    expect_parse_error("B12");
    expect_parse_error("OI(");
    expect_parse_error("O-I)");

    fred::BufferManager manager;
    NullOutput output;
    fred::ExecutionContext context(manager, output);
    fred::CommandExecutor executor;

    // Valeur implicite historique : O+I(.
    assert(context.input_parentheses_required());

    expect_execution_error(
        "B0", "buff/reg name invalid", executor, context);
    expect_execution_error(
        "Ba", "buff/reg name invalid", executor, context);

    execute("B(a)", executor, context);
    assert(manager.current().name() == "a");

    // O-I( autorise le nom court d'un caractère.
    execute("O-I(", executor, context);
    assert(!context.input_parentheses_required());

    execute("Ba", executor, context);
    assert(manager.current().name() == "a");
    execute("Bc", executor, context);
    assert(manager.current().name() == "c");
    execute("B0", executor, context);
    assert(manager.current().name() == "0");
    execute("B1", executor, context);
    assert(manager.current().name() == "1");
    execute("B.", executor, context);
    assert(manager.current().name() == ".");

    // O+I( rétablit l'obligation.
    execute("O+I(", executor, context);
    assert(context.input_parentheses_required());
    expect_execution_error(
        "Bc", "buff/reg name invalid", executor, context);

    execute("B(c)", executor, context);
    assert(manager.current().name() == "c");

    std::cout << "OI( option and short buffer-name tests passed\n";
}
