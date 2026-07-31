#include "fred/ast/AbsoluteAddressNode.hpp"
#include "fred/ast/CommandNode.hpp"
#include "fred/ast/RangeAddressNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/parser/ParseError.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <string_view>

namespace {

std::unique_ptr<fred::CommandNode> parse(std::string_view source) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    return parser.parse();
}

void expect_error(std::string_view source, std::string_view message) {
    try {
        (void)parse(source);
        assert(false && "expected ParseError");
    } catch (const fred::ParseError& error) {
        assert(std::string_view(error.what()) == message);
    }
}

} // namespace

int main() {
    {
        const auto node = parse("P");
        assert(node->kind() == fred::AstNodeKind::PrintCommand);
        assert(!node->has_address());
    }
    {
        const auto node = parse("L sample.txt\n");
        assert(node->kind() == fred::AstNodeKind::ListCommand);
        assert(!node->has_address());
        const auto& list = static_cast<const fred::ListCommandNode&>(*node);
        assert(list.filename() && *list.filename() == "sample.txt");
    }
    {
        const auto node = parse("D");
        assert(node->kind() == fred::AstNodeKind::DeleteCommand);
        assert(!node->has_address());
    }
    {
        const auto node = parse("d");
        assert(node->kind() == fred::AstNodeKind::DeleteCommand);
        assert(!node->has_address());
    }
    {
        const auto node = parse("1,$d");
        assert(node->kind() == fred::AstNodeKind::DeleteCommand);
        assert(node->has_address());
        assert(node->address()->kind() == fred::AstNodeKind::RangeAddress);
    }
    {
        const auto node = parse("A");
        assert(node->kind() == fred::AstNodeKind::AppendCommand);
        assert(!node->has_address());
    }
    {
        const auto node = parse("0a");
        assert(node->kind() == fred::AstNodeKind::AppendCommand);
        assert(node->has_address());
    }
    {
        const auto node = parse("I");
        assert(node->kind() == fred::AstNodeKind::InsertCommand);
        assert(!node->has_address());
    }
    {
        const auto node = parse("2i");
        assert(node->kind() == fred::AstNodeKind::InsertCommand);
        assert(node->has_address());
    }
    {
        const auto node = parse("C");
        assert(node->kind() == fred::AstNodeKind::ChangeCommand);
        assert(!node->has_address());
    }
    {
        const auto node = parse("2,4c");
        assert(node->kind() == fred::AstNodeKind::ChangeCommand);
        assert(node->has_address());
        assert(node->address()->kind() == fred::AstNodeKind::RangeAddress);
    }
    {
        const auto node = parse("2,3M5");
        assert(node->kind() == fred::AstNodeKind::MoveCommand);
        assert(node->has_address());
        assert(node->address()->kind() == fred::AstNodeKind::RangeAddress);
        const auto& move = static_cast<const fred::MoveCommandNode&>(*node);
        assert(move.destination() != nullptr);
        assert(move.destination()->kind() == fred::AstNodeKind::AbsoluteAddress);
    }
    {
        const auto node = parse("m0");
        assert(node->kind() == fred::AstNodeKind::MoveCommand);
        assert(!node->has_address());
        const auto& move = static_cast<const fred::MoveCommandNode&>(*node);
        const auto& destination =
            static_cast<const fred::AbsoluteAddressNode&>(*move.destination());
        assert(destination.line() == 0);
    }
    {
        const auto node = parse("1p");
        assert(node->kind() == fred::AstNodeKind::PrintCommand);
        assert(node->has_address());
    }
    {
        const auto node = parse("12P");
        assert(node->kind() == fred::AstNodeKind::PrintCommand);
        assert(node->has_address());
        assert(node->address()->kind() == fred::AstNodeKind::AbsoluteAddress);
        const auto& address =
            static_cast<const fred::AbsoluteAddressNode&>(*node->address());
        assert(address.line() == 12);
    }
    {
        const auto node = parse("L");
        assert(node->kind() == fred::AstNodeKind::ListCommand);
        const auto& list = static_cast<const fred::ListCommandNode&>(*node);
        assert(!list.filename());
    }
    {
        const auto registry = fred::make_core_command_registry();
        assert(registry.size() == 7);
        assert(registry.contains('P'));
        assert(registry.contains('L'));
        assert(registry.contains('D'));
        assert(registry.contains('A'));
        assert(registry.contains('I'));
        assert(registry.contains('C'));
        assert(registry.contains('M'));
        assert(!registry.contains('Z'));
        assert(registry.find('P')->name == "Print");
    }

    expect_error("", "expected a command");
    expect_error("1", "expected a command");
    expect_error("Z", "unknown command 'Z'");
    expect_error("P D", "unexpected token after command: 'D'");
    expect_error("M", "M requires a destination address");
    expect_error("1M2,3", "M destination must be a single line address");
    expect_error("1L", "L does not accept a line address");
    expect_error("print", "expected a command, got 'print'");

    std::cout << "command parser tests passed\n";
}
