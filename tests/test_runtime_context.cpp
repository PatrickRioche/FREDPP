#include "fred/core/BufferManager.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cassert>

int main() {
    fred::BufferManager buffers;
    buffers.current().append("alpha");

    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);

    assert(&context.buffers() == &buffers);
    assert(&context.current_buffer() == &buffers.current());
    assert(context.current_buffer().line_count() == 1);
    assert(context.current_buffer().line(1) == "alpha");

    context.output().write("one");
    context.output().write_line(" two");
    assert(output.content() == "one two\n");

    output.clear();
    assert(output.empty());

    context.buffers().create_or_select("work").append("beta");
    assert(context.current_buffer().name() == "work");
    assert(context.current_buffer().line(1) == "beta");

    context.set_counter(-7);
    assert(context.counter() == -7);

    assert(!context.has_numeric_register("np"));
    assert(context.numeric_register("np") == 0);

    context.set_numeric_register("np", 12);
    assert(context.has_numeric_register("np"));
    assert(context.numeric_register("np") == 12);

    context.set_numeric_register("np", -3);
    assert(context.numeric_register("np") == -3);
}
