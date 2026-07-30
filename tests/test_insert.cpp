#include "fred/ast/AbsoluteAddressNode.hpp"
#include "fred/ast/CommandNode.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cassert>
#include <memory>
#include <string_view>

namespace {
class NullOutput final : public fred::Output {
public:
    void write(std::string_view) override {}
};
}

int main() {
    NullOutput output;
    fred::CommandExecutor executor;

    // I on an empty buffer inserts at the beginning.
    fred::BufferManager empty_buffers;
    fred::ExecutionContext empty_context(empty_buffers, output);
    fred::InsertCommandNode insert_empty(nullptr, fred::SourceLocation{});
    executor.execute_insert(insert_empty, empty_context, {"only"});
    assert(empty_buffers.current().line_count() == 1);
    assert(empty_buffers.current().line(1) == "only");
    assert(empty_buffers.current().current_line() == 1);

    // Prepare a non-empty buffer: A, B, C, D.
    fred::BufferManager buffers;
    fred::ExecutionContext context(buffers, output);
    buffers.current().append("A");
    buffers.current().append("B");
    buffers.current().append("C");
    buffers.current().append("D");

    // I without an address inserts before the current line.
    buffers.current().set_current_line(3);
    fred::InsertCommandNode insert_current(nullptr, fred::SourceLocation{});
    executor.execute_insert(insert_current, context, {"X", "Y"});
    assert(buffers.current().line_count() == 6);
    assert(buffers.current().line(1) == "A");
    assert(buffers.current().line(2) == "B");
    assert(buffers.current().line(3) == "X");
    assert(buffers.current().line(4) == "Y");
    assert(buffers.current().line(5) == "C");
    assert(buffers.current().line(6) == "D");
    assert(buffers.current().current_line() == 4);

    // 1I inserts before the first line.
    fred::InsertCommandNode insert_before_one(
        std::make_unique<fred::AbsoluteAddressNode>(1, fred::SourceLocation{}),
        fred::SourceLocation{});
    executor.execute_insert(insert_before_one, context, {"FIRST"});
    assert(buffers.current().line(1) == "FIRST");
    assert(buffers.current().line(2) == "A");
    assert(buffers.current().current_line() == 1);

    // 0I is accepted as an explicit insertion at the beginning.
    fred::InsertCommandNode insert_zero(
        std::make_unique<fred::AbsoluteAddressNode>(0, fred::SourceLocation{}),
        fred::SourceLocation{});
    executor.execute_insert(insert_zero, context, {"ZERO"});
    assert(buffers.current().line(1) == "ZERO");
    assert(buffers.current().line(2) == "FIRST");
    assert(buffers.current().current_line() == 1);

    // An addressed insertion before the last line keeps the last line after
    // the inserted text.
    const auto last = buffers.current().line_count();
    fred::InsertCommandNode insert_before_last(
        std::make_unique<fred::AbsoluteAddressNode>(last, fred::SourceLocation{}),
        fred::SourceLocation{});
    executor.execute_insert(insert_before_last, context, {"BEFORE-LAST"});
    assert(buffers.current().line(buffers.current().line_count() - 1) == "BEFORE-LAST");
    assert(buffers.current().line(buffers.current().line_count()) == "D");
    assert(buffers.current().current_line() == buffers.current().line_count() - 1);

    // Empty text input does not change the buffer and selects the position
    // immediately before the addressed line.
    const auto count_before_empty_insert = buffers.current().line_count();
    executor.execute_insert(insert_before_one, context, {});
    assert(buffers.current().line_count() == count_before_empty_insert);
    assert(buffers.current().current_line() == 0);
}
