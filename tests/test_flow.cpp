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

    std::cout << "Flow tests passed.\n";
    return EXIT_SUCCESS;
}
