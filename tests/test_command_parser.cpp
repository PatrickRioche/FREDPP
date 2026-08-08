#include "fred/ast/AbsoluteAddressNode.hpp"
#include "fred/ast/CommandNode.hpp"
#include "fred/ast/LastAddressNode.hpp"
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
        const auto node = parse("*");
        assert(node->kind() == fred::AstNodeKind::PrintCommand);
        assert(node->has_address());
        assert(node->address()->kind() == fred::AstNodeKind::RangeAddress);
        const auto& range =
            static_cast<const fred::RangeAddressNode&>(*node->address());
        assert(range.first().kind() == fred::AstNodeKind::AbsoluteAddress);
        assert(static_cast<const fred::AbsoluteAddressNode&>(range.first()).line() == 1);
        assert(range.last().kind() == fred::AstNodeKind::LastAddress);
    }
    {
        const auto node = parse("*d");
        assert(node->kind() == fred::AstNodeKind::DeleteCommand);
        assert(node->has_address());
        assert(node->address()->kind() == fred::AstNodeKind::RangeAddress);
    }
    {
        const auto node = parse("*G/recherche/P");
        assert(node->kind() == fred::AstNodeKind::GlobalCommand);
        assert(node->has_address());
        assert(node->address()->kind() == fred::AstNodeKind::RangeAddress);
    }
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
        const auto node = parse("B(mon buffer)");
        assert(node->kind() == fred::AstNodeKind::BufferCommand);
        const auto& buffer = static_cast<const fred::BufferCommandNode&>(*node);
        assert(buffer.buffer_name() == "mon buffer");
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
        const auto node = parse("2,3T5");
        assert(node->kind() == fred::AstNodeKind::TransferCommand);
        assert(node->has_address());
        assert(node->address()->kind() == fred::AstNodeKind::RangeAddress);
        const auto& transfer = static_cast<const fred::TransferCommandNode&>(*node);
        assert(transfer.destination() != nullptr);
        assert(transfer.destination()->kind() == fred::AstNodeKind::AbsoluteAddress);
    }
    {
        const auto node = parse("t0");
        assert(node->kind() == fred::AstNodeKind::TransferCommand);
        assert(!node->has_address());
        const auto& transfer = static_cast<const fred::TransferCommandNode&>(*node);
        const auto& destination =
            static_cast<const fred::AbsoluteAddressNode&>(*transfer.destination());
        assert(destination.line() == 0);
    }
    {
        const auto node = parse("1,$G/recherche$/D");
        assert(node->kind() == fred::AstNodeKind::GlobalCommand);
        assert(node->has_address());
        const auto& global = static_cast<const fred::GlobalCommandNode&>(*node);
        assert(!global.inverted());
        assert(global.nested_command().kind() == fred::AstNodeKind::DeleteCommand);
    }
    {
        const auto node = parse("G~/recherche$/P");
        assert(node->kind() == fred::AstNodeKind::GlobalCommand);
        const auto& global = static_cast<const fred::GlobalCommandNode&>(*node);
        assert(global.inverted());
        assert(global.nested_command().kind() == fred::AstNodeKind::PrintCommand);
    }
    {
        const auto node = parse("2Z");
        assert(node->kind() == fred::AstNodeKind::ZapCommand);
        assert(node->has_address());
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
        const auto node = parse("R \"sample file.txt\"");
        assert(node->kind() == fred::AstNodeKind::ReadCommand);
        const auto& read = static_cast<const fred::ReadCommandNode&>(*node);
        assert(read.filename() == "sample file.txt");
    }
    {
        const auto node = parse("W");
        assert(node->kind() == fred::AstNodeKind::WriteCommand);
        const auto& write = static_cast<const fred::WriteCommandNode&>(*node);
        assert(!write.filename());
        assert(write.mode() == fred::FileWriteMode::Preserve);
    }
    {
        const auto node = parse("1,$WA \"ascii output.txt\"");
        assert(node->kind() == fred::AstNodeKind::WriteCommand);
        assert(node->has_address());
        const auto& write = static_cast<const fred::WriteCommandNode&>(*node);
        assert(write.filename() && *write.filename() == "ascii output.txt");
        assert(write.mode() == fred::FileWriteMode::Ascii);
    }
    {
        const auto node = parse("WU utf8.txt");
        const auto& write = static_cast<const fred::WriteCommandNode&>(*node);
        assert(write.mode() == fred::FileWriteMode::Utf8);
    }
    {
        const auto node = parse("WB legacy.bcd");
        const auto& write = static_cast<const fred::WriteCommandNode&>(*node);
        assert(write.mode() == fred::FileWriteMode::BcdUnsupported);
    }
    {
        const auto registry = fred::make_core_command_registry();
        assert(registry.size() == 18);
        assert(registry.contains('P'));
        assert(registry.contains('J'));
        assert(registry.contains('L'));
        assert(registry.contains('D'));
        assert(registry.contains('A'));
        assert(registry.contains('I'));
        assert(registry.contains('C'));
        assert(registry.contains('B'));
        assert(registry.contains('M'));
        assert(registry.contains('T'));
        assert(registry.contains('F'));
        assert(registry.contains('G'));
        assert(registry.contains('Z'));
        assert(registry.contains('S'));
        assert(registry.contains('Q'));
        assert(registry.contains('R'));
        assert(registry.contains('W'));
        assert(registry.contains('O'));
        assert(registry.find('P')->name == "Print");
    }

    expect_error("", "expected a command");
    expect_error("1", "expected a command");
    expect_error("P D", "unexpected token after command: 'D'");
    expect_error("G/a/", "G requires a command after the pattern");
    expect_error("G/a/1P", "addressed commands inside G are not supported yet");
    expect_error("1,2Z", "Z accepts at most one line address");
    expect_error("M", "M requires a destination address");
    expect_error("1M2,3", "M destination must be a single line address");
    expect_error("T", "T requires a destination address");
    expect_error("1T2,3", "T destination must be a single line address");
    expect_error("1L", "L does not accept a line address");
    expect_error("R", "R requires a filename");
    expect_error("1R file.txt", "R does not accept a line address");
    expect_error("print", "expected a command, got 'print'");

    std::cout << "command parser tests passed\n";
}
