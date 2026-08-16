#include "fred/flow/FlowCharacterStream.hpp"
#include "fred/lexer/Diagnostic.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenType.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

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

void expect_token(const fred::Token& token,
                  fred::TokenType type,
                  std::string_view lexeme,
                  std::size_t line,
                  std::size_t column,
                  std::size_t flow_level,
                  const std::string& message) {
    expect(token.type == type, message + ": wrong type");
    expect(token.lexeme == lexeme, message + ": wrong lexeme");
    expect(token.location.line == line, message + ": wrong line");
    expect(token.location.column == column, message + ": wrong column");
    expect(token.location.flow_level == flow_level,
           message + ": wrong flow level");
}

} // namespace

int main() {
    {
        fred::Lexer lexer("A");
        const auto tokens = lexer.tokenize();
        expect(tokens.size() == 2, "A token count");
        expect_token(tokens[0], fred::TokenType::Command, "A", 1, 1, 0,
                     "A command");
        expect_token(tokens[1], fred::TokenType::End, "", 1, 2, 0,
                     "A end");
    }

    {
        fred::Lexer lexer("123");
        const auto tokens = lexer.tokenize();
        expect_token(tokens[0], fred::TokenType::Number, "123", 1, 1, 0,
                     "number");
    }

    {
        fred::Lexer lexer("M5");
        const auto tokens = lexer.tokenize();
        expect(tokens.size() == 3, "compact command token count");
        expect_token(tokens[0], fred::TokenType::Command, "M",
                     1, 1, 0, "compact move command");
        expect_token(tokens[1], fred::TokenType::Number, "5",
                     1, 2, 0, "compact move destination");
        expect_token(tokens[2], fred::TokenType::End, "",
                     1, 3, 0, "compact move end");
    }

    {
        fred::Lexer lexer("abc_12");
        const auto tokens = lexer.tokenize();
        expect_token(tokens[0], fred::TokenType::Identifier, "abc_12",
                     1, 1, 0, "identifier");
    }

    {
        fred::Lexer lexer("1,5D");
        const auto tokens = lexer.tokenize();
        expect(tokens.size() == 5, "range token count");
        expect_token(tokens[0], fred::TokenType::Number, "1", 1, 1, 0,
                     "range first");
        expect_token(tokens[1], fred::TokenType::Comma, ",", 1, 2, 0,
                     "range comma");
        expect_token(tokens[2], fred::TokenType::Number, "5", 1, 3, 0,
                     "range second");
        expect_token(tokens[3], fred::TokenType::Command, "D", 1, 4, 0,
                     "range command");
        expect_token(tokens[4], fred::TokenType::End, "", 1, 5, 0,
                     "range end");
    }

    {
        fred::Lexer lexer("\\B(test)", 3);
        const auto tokens = lexer.tokenize();
        expect(tokens.size() == 6, "directive token count");
        expect_token(tokens[0], fred::TokenType::Backslash, "\\",
                     1, 1, 3, "directive slash");
        expect_token(tokens[1], fred::TokenType::Command, "B",
                     1, 2, 3, "directive command");
        expect_token(tokens[2], fred::TokenType::LeftParenthesis, "(",
                     1, 3, 3, "directive opening");
        expect_token(tokens[3], fred::TokenType::Identifier, "test",
                     1, 4, 3, "directive identifier");
        expect_token(tokens[4], fred::TokenType::RightParenthesis, ")",
                     1, 8, 3, "directive closing");
        expect_token(tokens[5], fred::TokenType::End, "",
                     1, 9, 3, "directive end");
    }

    {
        fred::Lexer lexer("A\n  12");
        const auto tokens = lexer.tokenize();
        expect_token(tokens[0], fred::TokenType::Command, "A",
                     1, 1, 0, "multiline command");
        expect_token(tokens[1], fred::TokenType::NewLine, "\n",
                     1, 2, 0, "newline");
        expect_token(tokens[2], fred::TokenType::Number, "12",
                     2, 3, 0, "multiline number");
    }

    {
        fred::FlowCharacterStream stream({
            {')', 0, fred::CharacterInterpretation::Literal}
        });
        fred::Lexer lexer(stream);
        const auto tokens = lexer.tokenize();
        expect(tokens.size() == 2,
               "literal closing parenthesis token count");
        expect_token(tokens[0], fred::TokenType::Symbol, ")",
                     1, 1, 0,
                     "literal closing parenthesis becomes symbol");
    }

    {
        fred::FlowCharacterStream stream({
            {'\\', 0, fred::CharacterInterpretation::Literal}
        });
        fred::Lexer lexer(stream);
        const auto tokens = lexer.tokenize();
        expect_token(tokens[0], fred::TokenType::Symbol, "\\",
                     1, 1, 0,
                     "literal backslash becomes symbol");
    }

    {
        fred::FlowCharacterStream stream({
            {' ', 0, fred::CharacterInterpretation::Literal},
            {'X', 0, fred::CharacterInterpretation::Normal}
        });
        fred::Lexer lexer(stream);
        const auto tokens = lexer.tokenize();
        expect(tokens.size() == 3,
               "literal space is not skipped");
        expect_token(tokens[0], fred::TokenType::Symbol, " ",
                     1, 1, 0,
                     "literal space becomes symbol");
        expect_token(tokens[1], fred::TokenType::Command, "X",
                     1, 2, 0,
                     "normal character after literal space");
    }

    {
        fred::FlowCharacterStream stream({
            {'\n', 0, fred::CharacterInterpretation::Literal},
            {'X', 0, fred::CharacterInterpretation::Normal}
        });
        fred::Lexer lexer(stream);
        const auto tokens = lexer.tokenize();
        expect(tokens.size() == 3,
               "literal newline does not terminate lexing");
        expect_token(tokens[0], fred::TokenType::Symbol, "\n",
                     1, 1, 0,
                     "literal newline becomes symbol");
        expect_token(tokens[1], fred::TokenType::Command, "X",
                     2, 1, 0,
                     "location advances after literal newline");
    }

    {
        fred::FlowCharacterStream stream({
            {'d', 1, fred::CharacterInterpretation::Literal},
            {'o', 1, fred::CharacterInterpretation::Literal},
            {'c', 1, fred::CharacterInterpretation::Literal},
            {'1', 1, fred::CharacterInterpretation::Literal}
        });
        fred::Lexer lexer(stream);
        const auto tokens = lexer.tokenize();
        expect(tokens.size() == 2,
               "literal identifier token count");
        expect_token(tokens[0], fred::TokenType::Identifier, "doc1",
                     1, 1, 1,
                     "literal S/L text still forms identifier");
    }

    {
        fred::FlowCharacterStream stream({
            {')', 0, fred::CharacterInterpretation::ForcedSpecial}
        });
        fred::Lexer lexer(stream);
        const auto tokens = lexer.tokenize();
        expect_token(tokens[0], fred::TokenType::RightParenthesis, ")",
                     1, 1, 0, "forced-special parenthesis");
        expect(tokens[0].interpretation == fred::CharacterInterpretation::ForcedSpecial,
               "forced-special token metadata");
    }
    {
        fred::FlowCharacterStream stream({
            {' ', 0, fred::CharacterInterpretation::ForcedSpecial}
        });
        fred::Lexer lexer(stream);
        const auto tokens = lexer.tokenize();
        expect_token(tokens[0], fred::TokenType::Symbol, " ",
                     1, 1, 0, "forced-special space is not skipped");
    }

    {
        fred::Lexer lexer(")");
        const auto tokens = lexer.tokenize();
        expect_token(tokens[0],
                     fred::TokenType::RightParenthesis, ")",
                     1, 1, 0,
                     "normal closing parenthesis remains structural");
    }

    {
        const fred::Diagnostic diagnostic{
            fred::DiagnosticSeverity::Error,
            "unknown command",
            fred::SourceLocation{2, 1, 3, 0},
            "AA?"
        };
        const auto formatted = fred::format_diagnostic(diagnostic);
        expect(formatted.find("error at 1:3") != std::string::npos,
               "diagnostic location");
        expect(formatted.find("^") != std::string::npos,
               "diagnostic caret");
    }

    std::cout << "Lexer tests passed.\n";
    return EXIT_SUCCESS;
}
