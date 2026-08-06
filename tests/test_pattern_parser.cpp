#include "fred/ast/PatternNodes.hpp"
#include "fred/parser/ParseError.hpp"
#include "fred/parser/PatternParser.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string_view>

namespace {
void require(bool condition, std::string_view message) {
    if (!condition) { std::cerr << "FAILED: " << message << '\n'; std::exit(1); }
}
std::unique_ptr<fred::PatternNode> parse(std::string_view text) {
    fred::PatternParser parser(text); return parser.parse();
}
void require_error(std::string_view text) {
    try { (void)parse(text); }
    catch (const fred::ParseError&) { return; }
    require(false, "expected ParseError");
}
}

int main() {
    {
        const auto node = parse("/A/");
        require(node->kind() == fred::AstNodeKind::PatternLiteral, "literal kind");
        require(static_cast<const fred::LiteralPatternNode&>(*node).value() == 'A', "literal value");
    }
    {
        const auto node = parse("/ab/");
        require(node->kind() == fred::AstNodeKind::PatternSequence, "sequence kind");
        require(static_cast<const fred::SequencePatternNode&>(*node).elements().size() == 2, "sequence size");
    }
    {
        const auto node = parse("/^a$/");
        require(node->kind() == fred::AstNodeKind::PatternSequence, "anchor sequence");
    }
    {
        const auto node = parse("/B+/");
        require(node->kind() == fred::AstNodeKind::PatternRepetition, "plus repetition");
        require(static_cast<const fred::RepetitionPatternNode&>(*node).type() == fred::RepetitionPatternNode::Type::OneOrMore, "plus type");
    }
    {
        const auto node = parse("/ab*/");
        require(node->kind() == fred::AstNodeKind::PatternSequence, "star sequence");
    }
    {
        const auto node = parse("/A|B/");
        require(node->kind() == fred::AstNodeKind::PatternAlternation, "alternation");
        require(static_cast<const fred::AlternationPatternNode&>(*node).alternatives().size() == 2, "alternative count");
    }
    {
        const auto node = parse("/(AB)+/");
        require(node->kind() == fred::AstNodeKind::PatternRepetition, "group repetition");
    }
    {
        const auto node = parse("/[1234567890]/");
        require(node->kind() == fred::AstNodeKind::PatternCharacterClass, "character class");
        require(static_cast<const fred::CharacterClassPatternNode&>(*node).ranges().size() == 10, "class items");
    }
    {
        const auto node = parse("/[a-z0-9]/");
        const auto& value = static_cast<const fred::CharacterClassPatternNode&>(*node);
        require(value.ranges().size() == 2, "class range count");
        require(value.ranges()[0].first == 'a' && value.ranges()[0].last == 'z', "letter range");
    }
    {
        const auto node = parse("/[^abc]/");
        require(static_cast<const fred::CharacterClassPatternNode&>(*node).negated(), "negated class");
    }
    {
        const auto node = parse("?hello?");
        require(node->kind() == fred::AstNodeKind::PatternSequence, "question delimiter");
    }
    {
        const auto node = parse("!hello!");
        require(node->kind() == fred::AstNodeKind::PatternSequence, "symbolic delimiter");
    }
    {
        const auto node = parse("/\\./");
        require(node->kind() == fred::AstNodeKind::PatternLiteral, "escaped literal");
        require(static_cast<const fred::LiteralPatternNode&>(*node).value() == '.', "escaped dot");
    }

    require_error("");
    require_error("abc");
    require_error("/abc");
    require_error("/A|/");
    require_error("/*A/");
    require_error("/A++/");
    require_error("/()/");
    require_error("/[abc/");
    require_error("/[]/");
    require_error("/A/extra");

    std::cout << "test_pattern_parser: OK\n";
    return 0;
}
