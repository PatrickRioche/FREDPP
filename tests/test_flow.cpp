#include "fred/core/BufferManager.hpp"
#include "fred/flow/FlowEngine.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
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

    buffers.create_or_select("self").append("\\B(self)");
    expect_throws([&] {
        fred::FlowEngine flow(buffers, 8);
        (void)flow.expand_buffer("self");
    }, "recursive flow reaches depth protection");

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
