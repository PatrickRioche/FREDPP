#include "fred/ast/AbsoluteAddressNode.hpp"
#include "fred/ast/CommandNode.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cassert>
#include <memory>
#include <string_view>
#include <vector>

namespace {
class NullOutput final : public fred::Output {
public:
    void write(std::string_view) override {}
};
}

int main() {
    fred::BufferManager buffers;
    NullOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    // A on an empty buffer defaults to line 0 / $.
    fred::AppendCommandNode append_end(nullptr, fred::SourceLocation{});
    executor.execute_append(append_end, context, {"alpha", "beta"});
    assert(buffers.current().line_count() == 2);
    assert(buffers.current().line(1) == "alpha");
    assert(buffers.current().line(2) == "beta");
    assert(buffers.current().current_line() == 2);

    // 1A inserts after line 1 and selects the last inserted line.
    fred::AppendCommandNode append_after_one(
        std::make_unique<fred::AbsoluteAddressNode>(1, fred::SourceLocation{}),
        fred::SourceLocation{});
    executor.execute_append(append_after_one, context, {"middle-1", "middle-2"});
    assert(buffers.current().line_count() == 4);
    assert(buffers.current().line(1) == "alpha");
    assert(buffers.current().line(2) == "middle-1");
    assert(buffers.current().line(3) == "middle-2");
    assert(buffers.current().line(4) == "beta");
    assert(buffers.current().current_line() == 3);

    // 0A inserts before the first stored line.
    fred::AppendCommandNode append_after_zero(
        std::make_unique<fred::AbsoluteAddressNode>(0, fred::SourceLocation{}),
        fred::SourceLocation{});
    executor.execute_append(append_after_zero, context, {"first"});
    assert(buffers.current().line(1) == "first");
    assert(buffers.current().current_line() == 1);

    // No input changes only the current line to the addressed line.
    executor.execute_append(append_after_zero, context, {});
    assert(buffers.current().line_count() == 5);
    assert(buffers.current().current_line() == 0);

    // I inserts before the addressed line and selects the last inserted line.
    buffers.current().set_current_line(3);
    fred::InsertCommandNode insert_current(nullptr, fred::SourceLocation{});
    executor.execute_insert(insert_current, context, {"before-current"});
    assert(buffers.current().line(3) == "before-current");
    assert(buffers.current().line(4) == "middle-1");
    assert(buffers.current().current_line() == 3);

    // 1I inserts at the beginning of a non-empty buffer.
    fred::InsertCommandNode insert_before_one(
        std::make_unique<fred::AbsoluteAddressNode>(1, fred::SourceLocation{}),
        fred::SourceLocation{});
    executor.execute_insert(insert_before_one, context, {"new-first", "new-second"});
    assert(buffers.current().line(1) == "new-first");
    assert(buffers.current().line(2) == "new-second");
    assert(buffers.current().line(3) == "first");
    assert(buffers.current().current_line() == 2);

    // I on an empty buffer behaves like insertion at position zero.
    fred::BufferManager empty_buffers;
    fred::ExecutionContext empty_context(empty_buffers, output);
    fred::InsertCommandNode insert_empty(nullptr, fred::SourceLocation{});
    executor.execute_insert(insert_empty, empty_context, {"only"});
    assert(empty_buffers.current().line_count() == 1);
    assert(empty_buffers.current().line(1) == "only");
    assert(empty_buffers.current().current_line() == 1);
}
