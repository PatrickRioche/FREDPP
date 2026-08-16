#include "fred/flow/FlowCharacterStream.hpp"
#include "fred/lexer/StringCharacterStream.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
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

void expect_throws(auto&& action, const std::string& message) {
    try {
        action();
    } catch (const std::out_of_range&) {
        return;
    }
    fail(message);
}

} // namespace

int main() {
    fred::StringCharacterStream stream("A\nbc", 4);

    expect(stream.size() == 4, "stream size");
    expect(stream.position() == 0, "initial position");
    expect(!stream.eof(), "not initially eof");

    const auto first = stream.peek();
    expect(first.has_value(), "first character exists");
    expect(first->value == 'A', "first character value");
    expect(first->location.offset == 0, "first offset");
    expect(first->location.line == 1, "first line");
    expect(first->location.column == 1, "first column");
    expect(first->location.flow_level == 4, "first flow level");
    expect(stream.position() == 0, "peek does not consume");

    const auto newline = stream.peek(1);
    expect(newline.has_value() && newline->value == '\n',
           "lookahead newline");
    expect(newline->location.line == 1, "newline line");
    expect(newline->location.column == 2, "newline column");

    const auto a = stream.consume();
    expect(a.has_value() && a->value == 'A', "consume A");
    expect(stream.position() == 1, "position after A");

    const auto nl = stream.consume();
    expect(nl.has_value() && nl->value == '\n', "consume newline");

    const auto b = stream.peek();
    expect(b.has_value() && b->value == 'b', "peek b");
    expect(b->location.line == 2, "b line");
    expect(b->location.column == 1, "b column");

    stream.rewind(0);
    expect(stream.position() == 0, "rewind to start");
    expect(stream.peek()->value == 'A', "A after rewind");

    stream.rewind(stream.size());
    expect(stream.eof(), "eof at size");
    expect(!stream.peek().has_value(), "peek empty at eof");
    expect(!stream.consume().has_value(), "consume empty at eof");

    const auto end = stream.end_location();
    expect(end.offset == 4, "end offset");
    expect(end.line == 2, "end line");
    expect(end.column == 3, "end column");
    expect(end.flow_level == 4, "end flow level");

    expect_throws([&] { stream.rewind(5); },
                  "invalid character rewind throws");

    {
        std::vector<fred::InputCharacter> input{
            {'A', 0, fred::CharacterInterpretation::Normal},
            {'\\', 2, fred::CharacterInterpretation::Literal},
            {'S', 2, fred::CharacterInterpretation::Literal}
        };

        fred::FlowCharacterStream flow_stream(std::move(input));

        const auto normal = flow_stream.consume();
        expect(normal.has_value(), "flow normal character exists");
        expect(
            normal->interpretation ==
                fred::CharacterInterpretation::Normal,
            "flow normal interpretation");
        expect(
            normal->location.flow_level == 0,
            "flow normal level");

        const auto literal = flow_stream.consume();
        expect(literal.has_value(), "flow literal character exists");
        expect(literal->value == '\\', "flow literal value");
        expect(
            literal->interpretation ==
                fred::CharacterInterpretation::Literal,
            "flow literal interpretation");
        expect(
            literal->location.flow_level == 2,
            "flow literal level");
    }

    std::cout << "CharacterStream tests passed.\n";
    return EXIT_SUCCESS;
}
