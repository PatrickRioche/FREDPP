#include "fred/ast/AbsoluteAddressNode.hpp"
#include "fred/ast/CommandNode.hpp"
#include "fred/ast/LastAddressNode.hpp"
#include "fred/ast/RangeAddressNode.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/runtime/CommandExecutionError.hpp"
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

void seed(fred::Buffer& buffer) {
    buffer.append("A");
    buffer.append("B");
    buffer.append("C");
    buffer.append("D");
}
}

int main() {
    NullOutput output;
    fred::CommandExecutor executor;

    // 2C replaces one addressed line by one line.
    {
        fred::BufferManager buffers;
        seed(buffers.current());
        fred::ExecutionContext context(buffers, output);
        fred::ChangeCommandNode command(
            std::make_unique<fred::AbsoluteAddressNode>(2, fred::SourceLocation{}),
            fred::SourceLocation{});

        executor.execute_change(command, context, {"BB"});
        assert(buffers.current().line_count() == 4);
        assert(buffers.current().line(1) == "A");
        assert(buffers.current().line(2) == "BB");
        assert(buffers.current().line(3) == "C");
        assert(buffers.current().line(4) == "D");
        assert(buffers.current().current_line() == 2);
    }

    // 2C may replace one line by several lines.
    {
        fred::BufferManager buffers;
        seed(buffers.current());
        fred::ExecutionContext context(buffers, output);
        fred::ChangeCommandNode command(
            std::make_unique<fred::AbsoluteAddressNode>(2, fred::SourceLocation{}),
            fred::SourceLocation{});

        executor.execute_change(command, context, {"B1", "B2", "B3"});
        assert(buffers.current().line_count() == 6);
        assert(buffers.current().line(1) == "A");
        assert(buffers.current().line(2) == "B1");
        assert(buffers.current().line(3) == "B2");
        assert(buffers.current().line(4) == "B3");
        assert(buffers.current().line(5) == "C");
        assert(buffers.current().line(6) == "D");
        assert(buffers.current().current_line() == 4);
    }

    // 2,3C replaces an addressed range at the original position.
    {
        fred::BufferManager buffers;
        seed(buffers.current());
        fred::ExecutionContext context(buffers, output);
        auto range = std::make_unique<fred::RangeAddressNode>(
            std::make_unique<fred::AbsoluteAddressNode>(2, fred::SourceLocation{}),
            std::make_unique<fred::AbsoluteAddressNode>(3, fred::SourceLocation{}),
            fred::SourceLocation{});
        fred::ChangeCommandNode command(std::move(range), fred::SourceLocation{});

        executor.execute_change(command, context, {"X", "Y"});
        assert(buffers.current().line_count() == 4);
        assert(buffers.current().line(1) == "A");
        assert(buffers.current().line(2) == "X");
        assert(buffers.current().line(3) == "Y");
        assert(buffers.current().line(4) == "D");
        assert(buffers.current().current_line() == 3);
    }

    // C without an address changes the current line.
    {
        fred::BufferManager buffers;
        seed(buffers.current());
        buffers.current().set_current_line(3);
        fred::ExecutionContext context(buffers, output);
        fred::ChangeCommandNode command(nullptr, fred::SourceLocation{});

        executor.execute_change(command, context, {"CURRENT"});
        assert(buffers.current().line(3) == "CURRENT");
        assert(buffers.current().current_line() == 3);
    }

    // 1,$C replaces the complete buffer.
    {
        fred::BufferManager buffers;
        seed(buffers.current());
        fred::ExecutionContext context(buffers, output);
        auto range = std::make_unique<fred::RangeAddressNode>(
            std::make_unique<fred::AbsoluteAddressNode>(1, fred::SourceLocation{}),
            std::make_unique<fred::LastAddressNode>(fred::SourceLocation{}),
            fred::SourceLocation{});
        fred::ChangeCommandNode command(std::move(range), fred::SourceLocation{});

        executor.execute_change(command, context, {"ONLY"});
        assert(buffers.current().line_count() == 1);
        assert(buffers.current().line(1) == "ONLY");
        assert(buffers.current().current_line() == 1);
    }

    // Immediate \\F is equivalent to deleting the addressed range.
    {
        fred::BufferManager buffers;
        seed(buffers.current());
        fred::ExecutionContext context(buffers, output);
        fred::ChangeCommandNode command(
            std::make_unique<fred::AbsoluteAddressNode>(2, fred::SourceLocation{}),
            fred::SourceLocation{});

        executor.execute_change(command, context, {});
        assert(buffers.current().line_count() == 3);
        assert(buffers.current().line(1) == "A");
        assert(buffers.current().line(2) == "C");
        assert(buffers.current().line(3) == "D");
        assert(buffers.current().current_line() == 2);
    }

    // C requires an existing line and must leave an empty buffer untouched.
    {
        fred::BufferManager buffers;
        fred::ExecutionContext context(buffers, output);
        fred::ChangeCommandNode command(nullptr, fred::SourceLocation{});
        bool failed = false;
        try {
            executor.execute_change(command, context, {"X"});
        } catch (const fred::CommandExecutionError&) {
            failed = true;
        }
        assert(failed);
        assert(buffers.current().empty());
    }
}
