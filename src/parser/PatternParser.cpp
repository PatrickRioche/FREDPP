#include "fred/parser/PatternParser.hpp"
#include "fred/parser/ParseError.hpp"

#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace fred {

namespace {

std::string quote_char(char value) {
    switch (value) {
    case '\\': return "\\\\";
    case '\n': return "\\n";
    case '\t': return "\\t";
    case '\r': return "\\r";
    case '\'': return "\\'";
    default: return std::string(1, value);
    }
}

std::string describe_impl(const PatternNode& node) {
    switch (node.kind()) {
    case AstNodeKind::PatternLiteral: {
        const auto& literal = static_cast<const LiteralPatternNode&>(node);
        return "Literal('" + quote_char(literal.value()) + "')";
    }
    case AstNodeKind::PatternAnyCharacter:
        return "AnyCharacter(.)";
    case AstNodeKind::PatternAnchor: {
        const auto& anchor = static_cast<const AnchorPatternNode&>(node);
        return anchor.type() == AnchorPatternNode::Type::StartOfLine
            ? "Anchor(^)" : "Anchor($)";
    }
    case AstNodeKind::PatternSequence: {
        const auto& sequence = static_cast<const SequencePatternNode&>(node);
        std::string result = "Sequence(";
        for (std::size_t i = 0; i < sequence.elements().size(); ++i) {
            if (i != 0) result += ", ";
            result += describe_impl(*sequence.elements()[i]);
        }
        return result + ")";
    }
    case AstNodeKind::PatternAlternation: {
        const auto& alternation = static_cast<const AlternationPatternNode&>(node);
        std::string result = "Alternation(";
        for (std::size_t i = 0; i < alternation.alternatives().size(); ++i) {
            if (i != 0) result += ", ";
            result += describe_impl(*alternation.alternatives()[i]);
        }
        return result + ")";
    }
    case AstNodeKind::PatternRepetition: {
        const auto& repetition = static_cast<const RepetitionPatternNode&>(node);
        const char symbol = repetition.type() == RepetitionPatternNode::Type::ZeroOrMore ? '*' : '+';
        return std::string("Repeat(") + symbol + ", " + describe_impl(repetition.operand()) + ")";
    }
    case AstNodeKind::PatternCharacterClass: {
        const auto& character_class = static_cast<const CharacterClassPatternNode&>(node);
        std::string result = character_class.negated() ? "CharacterClass(^: " : "CharacterClass(";
        for (std::size_t i = 0; i < character_class.ranges().size(); ++i) {
            if (i != 0) result += ", ";
            const auto range = character_class.ranges()[i];
            result += "'" + quote_char(range.first) + "'";
            if (range.first != range.last) {
                result += "-'" + quote_char(range.last) + "'";
            }
        }
        return result + ")";
    }
    case AstNodeKind::PatternGroup: {
        const auto& group = static_cast<const GroupPatternNode&>(node);
        return "Group(" + describe_impl(group.expression()) + ")";
    }
    default:
        return "<non-pattern-node>";
    }
}

} // namespace

PatternParser::PatternParser(std::string_view source, std::size_t flow_level) noexcept
    : source_(source), flow_level_(flow_level) {}

std::unique_ptr<PatternNode> PatternParser::parse() {
    if (source_.empty()) fail("expected a delimited FRED pattern");
    delimiter_ = consume();
    if (delimiter_ != '/' && delimiter_ != '?') {
        fail("a FRED pattern must begin with '/' or '?'");
    }

    auto expression = parse_alternation();
    if (at_end() || peek() != delimiter_) fail("unterminated FRED pattern");
    consume();
    if (!at_end()) fail("unexpected character after FRED pattern");
    return expression;
}

std::unique_ptr<PatternNode> PatternParser::parse_alternation() {
    const auto start = location();
    std::vector<std::unique_ptr<PatternNode>> alternatives;
    alternatives.push_back(parse_sequence());
    while (!at_end() && peek() == '|') {
        consume();
        if (at_end() || peek() == delimiter_ || peek() == ')' || peek() == '|') {
            fail("expected a pattern after '|'");
        }
        alternatives.push_back(parse_sequence());
    }
    if (alternatives.size() == 1) return std::move(alternatives.front());
    return std::make_unique<AlternationPatternNode>(std::move(alternatives), start);
}

std::unique_ptr<PatternNode> PatternParser::parse_sequence() {
    const auto start = location();
    std::vector<std::unique_ptr<PatternNode>> elements;
    while (!at_end() && peek() != delimiter_ && peek() != ')' && peek() != '|') {
        elements.push_back(parse_repetition());
    }
    if (elements.empty()) {
        return std::make_unique<SequencePatternNode>(std::move(elements), start);
    }
    if (elements.size() == 1) return std::move(elements.front());
    return std::make_unique<SequencePatternNode>(std::move(elements), start);
}

std::unique_ptr<PatternNode> PatternParser::parse_repetition() {
    const auto start = location();
    auto atom = parse_atom();
    if (!at_end() && (peek() == '*' || peek() == '+')) {
        const char operator_character = consume();
        const auto type = operator_character == '*'
            ? RepetitionPatternNode::Type::ZeroOrMore
            : RepetitionPatternNode::Type::OneOrMore;
        atom = std::make_unique<RepetitionPatternNode>(std::move(atom), type, start);
        if (!at_end() && (peek() == '*' || peek() == '+')) {
            fail("a pattern may not have more than one repetition operator");
        }
    }
    return atom;
}

std::unique_ptr<PatternNode> PatternParser::parse_atom() {
    const auto start = location();
    if (at_end()) fail("expected a pattern atom");
    const char value = consume();
    switch (value) {
    case '.': return std::make_unique<AnyCharacterPatternNode>(start);
    case '^': return std::make_unique<AnchorPatternNode>(AnchorPatternNode::Type::StartOfLine, start);
    case '$': return std::make_unique<AnchorPatternNode>(AnchorPatternNode::Type::EndOfLine, start);
    case '(': --position_; return parse_group();
    case '[': --position_; return parse_character_class();
    case '\\':
        if (at_end() || peek() == delimiter_) fail("expected a character after escape");
        return std::make_unique<LiteralPatternNode>(consume(), start);
    case '*':
    case '+':
        fail("repetition operator has no preceding pattern");
    default:
        return std::make_unique<LiteralPatternNode>(value, start);
    }
}

std::unique_ptr<PatternNode> PatternParser::parse_group() {
    const auto start = location();
    consume(); // (
    if (!at_end() && peek() == ')') fail("empty parenthesized pattern");
    auto expression = parse_alternation();
    if (at_end() || peek() != ')') fail("unterminated parenthesized pattern");
    consume();
    return std::make_unique<GroupPatternNode>(std::move(expression), start);
}

std::unique_ptr<PatternNode> PatternParser::parse_character_class() {
    const auto start = location();
    consume(); // [
    bool negated = false;
    if (!at_end() && peek() == '^') {
        negated = true;
        consume();
    }
    std::vector<CharacterClassRange> ranges;
    while (!at_end() && peek() != ']') {
        auto read_class_character = [this]() -> char {
            if (at_end() || peek() == ']') fail("expected a character in character class");
            char value = consume();
            if (value == '\\') {
                if (at_end()) fail("expected a character after escape in character class");
                value = consume();
            }
            return value;
        };

        const char first = read_class_character();
        if (!at_end() && peek() == '-' && position_ + 1 < source_.size() && source_[position_ + 1] != ']') {
            consume();
            const char last = read_class_character();
            ranges.push_back(CharacterClassRange{first, last});
        } else {
            ranges.push_back(CharacterClassRange{first, first});
        }
    }
    if (at_end() || peek() != ']') fail("unterminated character class");
    consume();
    if (ranges.empty()) fail("empty character class");
    return std::make_unique<CharacterClassPatternNode>(negated, std::move(ranges), start);
}

bool PatternParser::at_end() const noexcept { return position_ >= source_.size(); }
char PatternParser::peek(std::size_t lookahead) const noexcept {
    const auto index = position_ + lookahead;
    return index < source_.size() ? source_[index] : '\0';
}
char PatternParser::consume() {
    if (at_end()) fail("unexpected end of pattern");
    return source_[position_++];
}
SourceLocation PatternParser::location() const noexcept {
    return SourceLocation{position_, 1, position_ + 1, flow_level_};
}
void PatternParser::fail(std::string_view message) const {
    throw ParseError(std::string(message), location());
}

std::string describe_pattern(const PatternNode& node) {
    return describe_impl(node);
}

} // namespace fred
