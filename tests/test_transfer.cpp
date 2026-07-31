#include "fred/ast/AbsoluteAddressNode.hpp"
#include "fred/ast/CommandNode.hpp"
#include "fred/ast/RangeAddressNode.hpp"
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

std::unique_ptr<fred::AbsoluteAddressNode> line(std::size_t number) {
    return std::make_unique<fred::AbsoluteAddressNode>(
        number, fred::SourceLocation{});
}

void fill(fred::Buffer& buffer) {
    buffer.append("1");
    buffer.append("2");
    buffer.append("3");
    buffer.append("4");
    buffer.append("5");
}

} // namespace

int main() {
    fred::BufferManager buffers;
    NullOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    // Copy a range to the end. The source remains unchanged.
    fill(buffers.current());
    {
        auto source = std::make_unique<fred::RangeAddressNode>(
            line(2), line(3), fred::SourceLocation{});
        fred::TransferCommandNode command(
            std::move(source), line(5), fred::SourceLocation{});
        executor.execute(command, context);

        const auto& buffer = buffers.current();
        assert(buffer.line_count() == 7);
        assert(buffer.line(1) == "1");
        assert(buffer.line(2) == "2");
        assert(buffer.line(3) == "3");
        assert(buffer.line(4) == "4");
        assert(buffer.line(5) == "5");
        assert(buffer.line(6) == "2");
        assert(buffer.line(7) == "3");
        assert(buffer.current_line() == 7);
    }

    // Copy one line after line 1.
    {
        fred::TransferCommandNode command(
            line(4), line(1), fred::SourceLocation{});
        executor.execute(command, context);

        const auto& buffer = buffers.current();
        assert(buffer.line_count() == 8);
        assert(buffer.line(1) == "1");
        assert(buffer.line(2) == "4");
        assert(buffer.line(3) == "2");
        assert(buffer.current_line() == 2);
    }

    // Destination zero copies a line to the beginning.
    {
        fred::TransferCommandNode command(
            line(8), line(0), fred::SourceLocation{});
        executor.execute(command, context);

        const auto& buffer = buffers.current();
        assert(buffer.line_count() == 9);
        assert(buffer.line(1) == "3");
        assert(buffer.line(2) == "1");
        assert(buffer.current_line() == 1);
    }

    // A destination inside the source range is valid for a copy.
    {
        auto source = std::make_unique<fred::RangeAddressNode>(
            line(3), line(4), fred::SourceLocation{});
        fred::TransferCommandNode command(
            std::move(source), line(3), fred::SourceLocation{});
        executor.execute(command, context);

        const auto& buffer = buffers.current();
        assert(buffer.line_count() == 11);
        assert(buffer.line(3) == "4");
        assert(buffer.line(4) == "4");
        assert(buffer.line(5) == "2");
        assert(buffer.line(6) == "2");
        assert(buffer.current_line() == 5);
    }
}
