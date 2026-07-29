#include "fred/ast/AbsoluteAddressNode.hpp"
#include "fred/ast/CurrentAddressNode.hpp"
#include "fred/ast/LastAddressNode.hpp"
#include "fred/ast/RangeAddressNode.hpp"
#include "fred/ast/RelativeAddressNode.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/AddressParser.hpp"
#include "fred/parser/ParseError.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <string>

namespace {

void fail(const std::string& message) {
    std::cerr << "FAILED: " << message << '\n';
    std::exit(EXIT_FAILURE);
}

void expect(bool condition, const std::string& message) {
    if (!condition) {
        fail(message);
    }
}

template <typename Node>
const Node& expect_node(const fred::AddressNode& node,
                        fred::AstNodeKind kind,
                        const std::string& message) {
    expect(node.kind() == kind, message + " kind");
    const auto* typed = dynamic_cast<const Node*>(&node);
    expect(typed != nullptr, message + " dynamic type");
    return *typed;
}

std::unique_ptr<fred::AddressNode> parse(std::string_view source) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    fred::AddressParser parser(tokens);
    return parser.parse();
}

void expect_parse_error(std::string_view source,
                        const std::string& message,
                        std::size_t expected_column = 0) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    fred::AddressParser parser(tokens);

    try {
        (void)parser.parse();
    } catch (const fred::ParseError& error) {
        if (expected_column != 0) {
            expect(error.location().column == expected_column,
                   message + " location");
        }
        expect(tokens.position() == 0,
               message + " rewinds TokenStream");
        return;
    }

    fail(message + " should throw ParseError");
}

} // namespace

int main() {
    {
        const auto node = parse("1");
        const auto& absolute = expect_node<fred::AbsoluteAddressNode>(
            *node, fred::AstNodeKind::AbsoluteAddress, "absolute 1");
        expect(absolute.line() == 1, "absolute value 1");
        expect(absolute.location().column == 1, "absolute location");
    }

    {
        const auto node = parse("12345");
        const auto& absolute = expect_node<fred::AbsoluteAddressNode>(
            *node, fred::AstNodeKind::AbsoluteAddress, "absolute 12345");
        expect(absolute.line() == 12345, "absolute value 12345");
    }

    {
        const auto node = parse("0");
        const auto& absolute = expect_node<fred::AbsoluteAddressNode>(
            *node, fred::AstNodeKind::AbsoluteAddress, "absolute zero");
        expect(absolute.line() == 0, "absolute value zero preserved");
    }

    {
        const auto node = parse(".");
        (void)expect_node<fred::CurrentAddressNode>(
            *node, fred::AstNodeKind::CurrentAddress, "current");
    }

    {
        const auto node = parse("$");
        (void)expect_node<fred::LastAddressNode>(
            *node, fred::AstNodeKind::LastAddress, "last");
    }

    {
        const auto node = parse("+3");
        const auto& relative = expect_node<fred::RelativeAddressNode>(
            *node, fred::AstNodeKind::RelativeAddress, "relative forward");
        expect(relative.direction() == fred::RelativeDirection::Forward,
               "forward direction");
        expect(relative.distance() == 3, "forward distance");
    }

    {
        const auto node = parse("-2");
        const auto& relative = expect_node<fred::RelativeAddressNode>(
            *node, fred::AstNodeKind::RelativeAddress, "relative backward");
        expect(relative.direction() == fred::RelativeDirection::Backward,
               "backward direction");
        expect(relative.distance() == 2, "backward distance");
    }

    {
        const auto node = parse("1,$");
        const auto& range = expect_node<fred::RangeAddressNode>(
            *node, fred::AstNodeKind::RangeAddress, "range 1,$");
        const auto& first = expect_node<fred::AbsoluteAddressNode>(
            range.first(), fred::AstNodeKind::AbsoluteAddress,
            "range first");
        expect(first.line() == 1, "range first value");
        (void)expect_node<fred::LastAddressNode>(
            range.last(), fred::AstNodeKind::LastAddress,
            "range last");
    }

    {
        const auto node = parse(".,+4");
        const auto& range = expect_node<fred::RangeAddressNode>(
            *node, fred::AstNodeKind::RangeAddress, "mixed range");
        (void)expect_node<fred::CurrentAddressNode>(
            range.first(), fred::AstNodeKind::CurrentAddress,
            "mixed range first");
        const auto& last = expect_node<fred::RelativeAddressNode>(
            range.last(), fred::AstNodeKind::RelativeAddress,
            "mixed range last");
        expect(last.distance() == 4, "mixed range distance");
    }

    {
        const auto node = parse("  10 , $  ");
        const auto& range = expect_node<fred::RangeAddressNode>(
            *node, fred::AstNodeKind::RangeAddress, "whitespace range");
        expect(range.location().column == 3, "range source location");
    }

    {
        const auto node = parse("5\n");
        const auto& absolute = expect_node<fred::AbsoluteAddressNode>(
            *node, fred::AstNodeKind::AbsoluteAddress, "trailing newline");
        expect(absolute.line() == 5, "trailing newline value");
    }

    expect_parse_error("", "empty input", 1);
    expect_parse_error(",1", "leading comma", 1);
    expect_parse_error("1,", "missing range end", 3);
    expect_parse_error("+", "missing forward distance", 2);
    expect_parse_error("-", "missing backward distance", 2);
    expect_parse_error("+.", "invalid relative distance", 2);
    expect_parse_error("A", "command is not address", 1);
    expect_parse_error("1 2", "trailing address", 3);
    expect_parse_error("1,$,2", "second comma", 4);
    expect_parse_error("1\n2", "content after newline", 1);

    {
        const std::string too_large =
            std::to_string(std::numeric_limits<std::size_t>::max()) + "0";
        expect_parse_error(too_large, "numeric overflow", 1);
    }

    std::cout << "AddressParser tests passed.\n";
    return EXIT_SUCCESS;
}
