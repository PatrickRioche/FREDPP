#include "fred/core/BufferManager.hpp"
#include "fred/flow/CommandInputExpansion.hpp"
#include "fred/flow/FlowEngine.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>

namespace {

void expect_equal(const std::string& actual,
                  const std::string& expected,
                  const char* message) {
    if (actual != expected) {
        std::cerr << "FAILED: " << message << "\n"
                  << "expected: [" << expected << "]\n"
                  << "actual:   [" << actual << "]\n";
        std::exit(EXIT_FAILURE);
    }
}

void expect_throws(auto&& action, const char* message) {
    try {
        action();
    } catch (const std::exception&) {
        return;
    }
    std::cerr << "FAILED: " << message << '\n';
    std::exit(EXIT_FAILURE);
}

} // namespace

int main() {
    fred::BufferManager buffers;

    // \R reads a single terminal line literally, without its newline.
    {
        fred::StringOutput terminal_output;
        fred::ExecutionContext terminal_context(buffers, terminal_output);
        std::istringstream input("input \\W/(literal)\r\n\\N(ignored)\n");
        terminal_context.set_terminal_input(input);
        fred::FlowEngine flow(terminal_context);

        expect_equal(flow.expand_input("left[\\R]right"),
                     "left[input \\W/(literal)]right",
                     "R reads CRLF-terminated terminal text literally");
        const auto characters =
            flow.expand_command_input_characters("JM!literal=\\r!");
        std::string expanded;
        for (const auto& character : characters) {
            expanded.push_back(character.value);
        }
        expect_equal(expanded, "JM!literal=\\N(ignored)!",
                     "R lowercase reads terminal without nested directives");
        const auto literal_start = expanded.find("\\N");
        if (literal_start == std::string::npos ||
            characters[literal_start].interpretation !=
                fred::CharacterInterpretation::Literal) {
            std::cerr << "FAILED: R terminal input must be literal\n";
            return EXIT_FAILURE;
        }
        expect_equal(flow.expand_command_input("\\C\\R"), "\\R",
                     "C protects R without another terminal read");
        expect_throws([&] { (void)flow.expand_input("\\R"); },
                      "R rejects terminal EOF");
        expect_equal(flow.expand_input("recovered"), "recovered",
                     "flow remains reusable after terminal EOF");
    }
    {
        fred::StringOutput terminal_output;
        fred::ExecutionContext terminal_context(buffers, terminal_output);
        std::istringstream input("terminal-buffer\n\n");
        terminal_context.set_terminal_input(input);
        buffers.create_or_select("terminal-buffer").append("from terminal");
        fred::FlowEngine flow(terminal_context);
        expect_equal(flow.expand_command_input("\\S(\\R)"),
                     "from terminal",
                     "R may compute the nested buffer name");
        expect_equal(flow.expand_input("left\\Rright"), "leftright",
                     "empty terminal line contributes no newline");
    }
    {
        fred::StringOutput output;
        fred::ExecutionContext no_terminal_context(buffers, output);
        fred::FlowEngine flow(no_terminal_context);
        expect_throws([&] { (void)flow.expand_command_input("\\R"); },
                      "R rejects an unconfigured terminal");
    }

    // Numeric registers participate in both general flow and command input.
    // The buffer-only API remains usable without an execution context.
    fred::StringOutput numeric_output;
    fred::ExecutionContext numeric_context(buffers, numeric_output);
    numeric_context.set_numeric_register("buf", 42);
    numeric_context.set_numeric_register("negative", -17);
    numeric_context.set_numeric_register("formatted", -35);
    numeric_context.set_numeric_base("formatted", 16);
    numeric_context.set_numeric_minimum_digits("formatted", 4);
    numeric_context.set_numeric_fill_character("formatted", '.');
    buffers.create_or_select("42").append("FROM-42");
    buffers.create_or_select("register-name").append("buf");
    buffers.create_or_select("numeric-parent")
        .append("before \\N(buf) after");
    {
        fred::FlowEngine flow(numeric_context);
        expect_equal(flow.expand_buffer("numeric-parent"),
                     "before 42 after\n",
                     "numeric substitution in buffer flow");
        expect_equal(flow.expand_input("[\\n(negative)]"), "[-17]",
                     "lowercase numeric substitution supports negatives");
        expect_equal(flow.expand_input("\\N(formatted)"), "-..23",
                     "N substitutes digits respecting NB/ND/NF");
        expect_equal(flow.expand_input("\\N(unset)"), "0",
                     "unset numeric register has historical zero value");
        expect_equal(flow.expand_input("\\S(\\N(buf))"), "FROM-42",
                     "numeric value may compute a nested buffer name");
        expect_equal(flow.expand_input("\\N(\\S(register-name))"), "42",
                     "buffer text may compute a numeric register name");
        expect_equal(flow.expand_input("\\C\\N(buf)"), "\\N(buf)",
                     "literal-character escape protects numeric directive");
        expect_equal(flow.expand_input("\\\\N(buf)"), "\\N(buf)",
                     "escaped backslash protects numeric directive");
        expect_throws([&] { (void)flow.expand_input("\\N()"); },
                      "empty numeric name is rejected");
        expect_throws([&] {
            (void)flow.expand_input("\\N(123456789012345)");
        }, "numeric name longer than 14 characters is rejected");
        expect_throws([&] { (void)flow.expand_input("\\N(buf"); },
                      "unterminated numeric substitution is rejected");
        expect_equal(flow.expand_input("\\N(buf)"), "42",
                     "flow is reusable after invalid numeric directive");
    }
    {
        const auto expanded = fred::expand_command_input_with_metadata(
            "JM/val=\\N(buf)/ B(\\N(buf))", numeric_context);
        expect_equal(expanded.text, "JM/val=42/ B(42)",
                     "numeric substitution in command arguments");
        const auto first_digit = expanded.text.find("42");
        if (first_digit == std::string::npos ||
            expanded.characters[first_digit].interpretation !=
                fred::CharacterInterpretation::Literal) {
            std::cerr << "FAILED: expanded numeric characters are literal\n";
            return EXIT_FAILURE;
        }
        fred::FlowEngine flow(numeric_context);
        expect_equal(flow.expand_command_input("\\N(formatted)"), "-..23",
                     "formatted N is expanded in command input");
        expect_equal(flow.expand_command_input("\\N(\\S(register-name))"),
                     "42", "nested numeric name in command input");
        expect_equal(flow.expand_command_input("\\C\\N(buf)"),
                     "\\N(buf)", "command literal escape protects numeric");
        expect_equal(flow.expand_command_input("\\\\N(buf)"),
                     "\\\\N(buf)",
                     "escaped command backslash protects numeric directive");
        buffers.create_or_select("literal-numeric").append("\\N(buf)");
        expect_equal(flow.expand_command_input("\\S(literal-numeric)"),
                     "\\N(buf)",
                     "literal buffer injection is not expanded again");
    }
    {
        fred::FlowEngine flow(numeric_context, 8);
        std::string nested = "\\N(buf)";
        for (int index = 0; index < 10; ++index) {
            nested = "\\N(" + nested + ")";
        }
        expect_throws([&] { (void)flow.expand_input(nested); },
                      "nested numeric reference is limited by flow depth");
    }
    {
        fred::FlowEngine flow(buffers);
        expect_equal(flow.expand_input("\\N(buf)"), "\\N(buf)",
                     "buffer-only flow keeps numeric directive unresolved");
    }


    // \W is a literal filename substitution; it is not the W command.
    buffers.create_or_select("w-current");
    {
        fred::FlowEngine flow(buffers);
        expect_throws([&] { (void)flow.expand_input("\\W"); },
                      "W needs a current filename");
        expect_throws([&] { (void)flow.expand_command_input("\\W"); },
                      "W needs a current filename in command input");
    }

    const std::string file_w = "C:\\data\\source.fredpp";
    buffers.current().associate_file(
        file_w, fred::TextEncoding::Utf8, fred::LineEnding::Lf, true);
    {
        fred::FlowEngine flow(buffers);
        expect_equal(flow.expand_input("File=\\W"),
                     "File=" + file_w, "full-flow filename");
        expect_equal(flow.expand_input("\\w"), file_w, "lowercase filename");
        expect_equal(flow.expand_command_input("JM!\\W!"),
                     "JM!" + file_w + "!", "command-input filename");
        expect_equal(flow.expand_command_input("JM!\\w!"),
                     "JM!" + file_w + "!", "lowercase command-input filename");
        expect_equal(flow.expand_input("\\C\\W"),
                     "\\W", "C protects filename flow");
        expect_equal(flow.expand_input("\\\\W"),
                     "\\W", "doubled slash protects filename flow");
        expect_equal(flow.expand_command_input("\\C\\W"),
                     "\\W", "C protects filename command input");
        expect_equal(flow.expand_command_input("\\\\W"),
                     "\\\\W", "doubled slash protects filename command input");
        const auto chars = flow.expand_command_input_characters("JM!\\W!");
        const auto offset = chars.size() - file_w.size() - 1;
        for (std::size_t i = 0; i < file_w.size(); ++i) {
            if (chars[offset + i].interpretation !=
                fred::CharacterInterpretation::Literal) {
                std::cerr << "FAILED: filename must be literal\n";
                return EXIT_FAILURE;
            }
        }
    }

    buffers.create_or_select("w-target").append("TARGET");
    buffers.create_or_select("w-current").associate_file(
        "w-target", fred::TextEncoding::Ascii, fred::LineEnding::Lf, true);
    {
        fred::FlowEngine flow(buffers);
        expect_equal(flow.expand_command_input("\\S(\\W)"),
                     "TARGET", "W computes a buffer name");
        expect_equal(flow.expand_input("\\S(\\W)"),
                     "TARGET", "W computes a full-flow buffer name");
    }

    buffers.create_or_select("plain").append("hello");
    {
        fred::FlowEngine flow(buffers);
        expect_equal(flow.expand_buffer("plain"), "hello\n",
                     "plain buffer emits line and newline");
    }

    buffers.create_or_select("child").append("CHILD");
    buffers.create_or_select("parent").append("before[\\B(child)]after");
    {
        fred::FlowEngine flow(buffers);
        expect_equal(flow.expand_buffer("parent"),
                     "before[CHILD\n]after\n",
                     "buffer injection resumes caller");
    }

    buffers.create_or_select("c").append("C");
    buffers.create_or_select("b").append("B<\\B(c)>");
    buffers.create_or_select("a").append("A<\\B(b)>");
    {
        fred::FlowEngine flow(buffers);
        expect_equal(flow.expand_buffer("a"),
                     "A<B<C\n>\n>\n",
                     "nested buffer injection");
    }

    buffers.create_or_select("s-child").append("EXPANDED");
    auto& s_literal = buffers.create_or_select("s-literal");
    s_literal.append("A\\B(s-child)");
    s_literal.append("C");
    buffers.create_or_select("s-parent").append("x[\\S(s-literal)]y");
    {
        fred::FlowEngine flow(buffers);
        expect_equal(flow.expand_buffer("s-parent"),
                     "x[A\\B(s-child)C]y\n",
                     "S injects literal buffer content without newlines");
    }

   buffers.create_or_select("s-empty").append("");
    buffers.create_or_select("s-empty-parent").append("a\\S(s-empty)b");
    {
        fred::FlowEngine flow(buffers);
        expect_equal(flow.expand_buffer("s-empty-parent"), "ab\n",
                     "S accepts an empty buffer");
    }

    buffers.create_or_select("s-missing").append("\\S(missing-s)");
    expect_throws([&] {
        fred::FlowEngine flow(buffers);
        (void)flow.expand_buffer("s-missing");
    }, "S missing buffer raises an error");

    buffers.create_or_select("slash").append("x\\\\y");
    {
        fred::FlowEngine flow(buffers);
        expect_equal(flow.expand_buffer("slash"), "x\\y\n",
                     "escaped slash");
    }

    buffers.create_or_select("bad").append("\\B(missing)");
    expect_throws([&] {
        fred::FlowEngine flow(buffers);
        (void)flow.expand_buffer("bad");
    }, "missing buffer raises an error");

    // A failed nested expansion must not strand its parent InputSource. Reuse
    // the same engine with both full-flow and command-input operations.
    {
        fred::FlowEngine flow(buffers);
        expect_throws([&] {
            (void)flow.expand_input("before\\B(missing)after");
        }, "missing nested buffer fails without poisoning the engine");
        expect_equal(flow.expand_buffer("plain"), "hello\n",
                     "full flow succeeds after a missing nested buffer");
        expect_equal(flow.expand_command_input("JM/ok/"), "JM/ok/",
                     "command input succeeds after full-flow failure");
        expect_throws([&] {
            (void)flow.expand_input("prefix\\C");
        }, "malformed directive fails without poisoning the engine");
        expect_equal(flow.expand_input("recovered"), "recovered",
                     "full flow succeeds after malformed directive");
    }

    buffers.create_or_select("self").append("\\B(self)");
    expect_throws([&] {
        fred::FlowEngine flow(buffers, 8);
        (void)flow.expand_buffer("self");
    }, "recursive flow reaches depth protection");
    {
        fred::FlowEngine flow(buffers, 8);
        expect_throws([&] { (void)flow.expand_buffer("self"); },
                      "recursive expansion fails at maximum depth");
        expect_equal(flow.expand_input("depth recovered"),
                     "depth recovered",
                     "flow engine reusable after recursion failure");
    }

    buffers.create_or_select("nombuf").append("doc1");
    buffers.create_or_select("doc1").append("BONJOUR");
    buffers.create_or_select("other").append("INTERDIT");
    buffers.create_or_select("literal-doc").append("\\S(other)");
    buffers.create_or_select("literal-name").append("literal-doc");

    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_command_input("B(\\S(nombuf))"),
            "B(doc1)",
            "command input expands S in B argument");
    }

    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_command_input(
                "R c:/fredpp/library/\\S(nombuf).fredpp"),
            "R c:/fredpp/library/doc1.fredpp",
            "command input expands S in R filename");
    }

    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_command_input(
                "W c:/fredpp/library/\\S(nombuf).fredpp"),
            "W c:/fredpp/library/doc1.fredpp",
            "command input expands S in W filename");
    }

    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_command_input("\\S(\\S(nombuf))"),
            "BONJOUR",
            "nested S resolves buffer name from another buffer");
    }

    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_input("\\S(\\S(nombuf))"),
            "BONJOUR",
            "full flow supports nested S buffer names");
    }

    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_command_input("\\S(\\S(literal-name))"),
            "\\S(other)",
            "S result remains literal and is not expanded twice");
    }

    {
        fred::FlowEngine flow(buffers);
        const auto characters =
            flow.expand_command_input_characters(
                "JM!\\S(literal-doc)!");

        bool saw_literal_slash = false;
        bool saw_normal_prefix = false;

        for (const auto& character : characters) {
            if (character.value == 'J' &&
                character.interpretation ==
                    fred::CharacterInterpretation::Normal) {
                saw_normal_prefix = true;
            }
            if (character.value == '\\' &&
                character.interpretation ==
                    fred::CharacterInterpretation::Literal) {
                saw_literal_slash = true;
            }
        }

        if (!saw_normal_prefix || !saw_literal_slash) {
            std::cerr
                << "FAILED: command flow character interpretation "
                   "was not preserved\n";
            return EXIT_FAILURE;
        }
    }

    auto& l_literal = buffers.create_or_select("l-literal");
    l_literal.append("A\\S(other)");
    l_literal.append("B(weird),$");

    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_input("x[\\L(l-literal)]y"),
            "x[A\\S(other)\nB(weird),$\n]y",
            "L injects literal buffer content with newlines");
    }

    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_command_input("\\L(l-literal)"),
            "A\\S(other)\nB(weird),$\n",
            "command input L preserves buffer newlines");
    }

    {
        fred::FlowEngine flow(buffers);
        const auto characters =
            flow.expand_command_input_characters(
                "JM!\\L(l-literal)!");

        bool saw_literal_slash = false;
        bool saw_literal_newline = false;

        for (const auto& character : characters) {
            if (character.value == '\\' &&
                character.interpretation ==
                    fred::CharacterInterpretation::Literal) {
                saw_literal_slash = true;
            }
            if (character.value == '\n' &&
                character.interpretation ==
                    fred::CharacterInterpretation::Literal) {
                saw_literal_newline = true;
            }
        }

        if (!saw_literal_slash || !saw_literal_newline) {
            std::cerr
                << "FAILED: L must preserve literal metadata and newlines\n";
            return EXIT_FAILURE;
        }
    }

    buffers.create_or_select("l-name").append("l-literal");
    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_command_input("\\L(\\S(l-name))"),
            "A\\S(other)\nB(weird),$\n",
            "L accepts nested S-computed buffer name");
    }

    buffers.create_or_select("l-empty");
    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_input("a\\L(l-empty)b"),
            "ab",
            "L accepts empty buffer");
    }

    expect_throws([&] {
        fred::FlowEngine flow(buffers);
        (void)flow.expand_input("\\L(missing-l)");
    }, "L missing buffer raises an error");

    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_input("A\\C$B"),
            "A$B",
            "full flow C makes the following character literal");
    }

    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_command_input("A\\C$B"),
            "A$B",
            "command input C removes the directive marker");
    }

    {
        fred::FlowEngine flow(buffers);
        const auto characters =
            flow.expand_command_input_characters("A\\C$B");

        bool saw_literal_dollar = false;
        for (const auto& character : characters) {
            if (character.value == '$' &&
                character.interpretation ==
                    fred::CharacterInterpretation::Literal) {
                saw_literal_dollar = true;
            }
        }

        if (!saw_literal_dollar) {
            std::cerr
                << "FAILED: C marks the following command-input "
                   "character literal\n";
            return EXIT_FAILURE;
        }
    }

    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_input("A\\C\\S(other)B"),
            "A\\S(other)B",
            "full flow C protects a following flow introducer");
    }

    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_command_input("A\\C\\S(other)B"),
            "A\\S(other)B",
            "command input C protects a following flow introducer");
    }

    {
        fred::FlowEngine flow(buffers);
        const auto characters =
            flow.expand_command_input_characters(
                "A\\C\\S(other)B");

        bool saw_literal_slash = false;
        for (const auto& character : characters) {
            if (character.value == '\\' &&
                character.interpretation ==
                    fred::CharacterInterpretation::Literal) {
                saw_literal_slash = true;
            }
        }

        if (!saw_literal_slash) {
            std::cerr
                << "FAILED: C must preserve literal metadata "
                   "for a protected backslash\n";
            return EXIT_FAILURE;
        }
    }

    buffers.create_or_select("c)paren").append("PAREN");
    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_command_input("\\S(c\\C)paren)"),
            "PAREN",
            "C protects a closing parenthesis inside a computed buffer name");
    }

    {
        fred::FlowEngine flow(buffers);
        expect_equal(
            flow.expand_input("\\S(c\\C)paren)"),
            "PAREN",
            "full flow C protects closing parenthesis in buffer name");
    }

    expect_throws([&] {
        fred::FlowEngine flow(buffers);
        (void)flow.expand_command_input("A\\C");
    }, "command input C requires a following character");

    expect_throws([&] {
        fred::FlowEngine flow(buffers);
        (void)flow.expand_input("A\\C");
    }, "full flow C requires a following character");

    {
        fred::FlowEngine flow(buffers);
        expect_equal(flow.expand_input("A\\O$B"), "A$B",
                     "full flow O consumes directive marker");
    }
    {
        fred::FlowEngine flow(buffers);
        expect_equal(flow.expand_command_input("A\\O$B"), "A$B",
                     "command input O consumes directive marker");
    }
    {
        fred::FlowEngine flow(buffers);
        const auto characters = flow.expand_command_input_characters("A\\O$B");
        bool saw_forced_special_dollar = false;
        for (const auto& character : characters) {
            if (character.value == '$' &&
                character.interpretation == fred::CharacterInterpretation::ForcedSpecial) {
                saw_forced_special_dollar = true;
            }
        }
        if (!saw_forced_special_dollar) {
            std::cerr << "FAILED: O must mark the following character ForcedSpecial\n";
            return EXIT_FAILURE;
        }
    }
    expect_throws([&] {
        fred::FlowEngine flow(buffers);
        (void)flow.expand_command_input("A\\O");
    }, "command input O requires a following character");
    expect_throws([&] {
        fred::FlowEngine flow(buffers);
        (void)flow.expand_input("A\\O");
    }, "full flow O requires a following character");

    {
        const std::string max_name(64, 'n');
        buffers.create_or_select(max_name);
    }

    expect_throws([&] {
        const std::string too_long_name(65, 'n');
        (void)buffers.create_or_select(too_long_name);
    }, "FREDPP rejects buffer names longer than 64 characters");

    std::cout << "Flow tests passed.\n";
    return EXIT_SUCCESS;
}
