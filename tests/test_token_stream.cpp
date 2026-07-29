#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/lexer/TokenType.hpp"

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
    fred::Lexer lexer("1,5D");
    fred::TokenStream stream(lexer);

    expect(stream.position() == 0, "initial token position");
    expect(!stream.eof(), "not initially eof");

    const auto& first = stream.peek();
    expect(first.type == fred::TokenType::Number, "first type");
    expect(first.lexeme == "1", "first lexeme");
    expect(stream.position() == 0, "peek does not consume");

    const auto& command = stream.peek(3);
    expect(command.type == fred::TokenType::Command, "lookahead command");
    expect(command.lexeme == "D", "lookahead command lexeme");
    expect(stream.position() == 0, "lookahead does not consume");

    const auto mark = stream.position();

    expect(stream.consume().lexeme == "1", "consume first");
    expect(stream.position() == 1, "position after first");
    expect(stream.consume().type == fred::TokenType::Comma,
           "consume comma");
    expect(stream.position() == 2, "position after comma");

    stream.rewind(mark);
    expect(stream.position() == 0, "rewind token stream");
    expect(stream.consume().lexeme == "1", "consume first after rewind");

    expect_throws([&] { stream.rewind(2); },
                  "cannot rewind forward");

    (void)stream.consume(); // comma
    (void)stream.consume(); // 5
    (void)stream.consume(); // D

    expect(stream.eof(), "eof after command");
    const auto eof_position = stream.position();

    const auto end1 = stream.consume();
    const auto end2 = stream.consume();

    expect(end1.type == fred::TokenType::End, "first End");
    expect(end2.type == fred::TokenType::End, "stable End");
    expect(stream.position() == eof_position,
           "consuming End does not advance");

    const auto& far_lookahead = stream.peek(100);
    expect(far_lookahead.type == fred::TokenType::End,
           "lookahead beyond end returns End");

    std::cout << "TokenStream tests passed.\n";
    return EXIT_SUCCESS;
}
