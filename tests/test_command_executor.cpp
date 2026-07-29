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
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>

namespace {

class StringOutput final : public fred::Output {
public:
    void write(std::string_view text) override { content_.append(text); }
    [[nodiscard]] const std::string& content() const noexcept { return content_; }
    void clear() noexcept { content_.clear(); }

private:
    std::string content_;
};

void append_sample(fred::Buffer& buffer) {
    buffer.append("alpha");
    buffer.append("beta");
    buffer.append("gamma");
    buffer.append("delta");
}

} // namespace

int main() {
    fred::BufferManager buffers;
    StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    append_sample(buffers.current());

    {
        fred::PrintCommandNode command(
            std::make_unique<fred::AbsoluteAddressNode>(1, fred::SourceLocation{}),
            fred::SourceLocation{});
        executor.execute(command, context);
        assert(output.content() == "alpha\n");
        assert(buffers.current().current_line() == 1);
        output.clear();
    }

    {
        auto range = std::make_unique<fred::RangeAddressNode>(
            std::make_unique<fred::AbsoluteAddressNode>(2, fred::SourceLocation{}),
            std::make_unique<fred::LastAddressNode>(fred::SourceLocation{}),
            fred::SourceLocation{});
        fred::PrintCommandNode command(std::move(range), fred::SourceLocation{});
        executor.execute(command, context);
        assert(output.content() == "beta\ngamma\ndelta\n");
        assert(buffers.current().current_line() == 4);
        output.clear();
    }

    {
        const auto path = std::filesystem::temp_directory_path() /
                          "fredpp_step73_list_test.txt";
        {
            std::ofstream file(path, std::ios::binary);
            file << "one\ntwo\n";
        }
        fred::ListCommandNode command(path.string(), fred::SourceLocation{});
        executor.execute(command, context);
        assert(output.content() == "one\ntwo\n");
        output.clear();
        std::filesystem::remove(path);
    }

    // D without an address deletes the current line.
    {
        buffers.current().set_current_line(2);
        fred::DeleteCommandNode command(nullptr, fred::SourceLocation{});
        executor.execute(command, context);
        assert(buffers.current().line_count() == 3);
        assert(buffers.current().line(1) == "alpha");
        assert(buffers.current().line(2) == "gamma");
        assert(buffers.current().line(3) == "delta");
        assert(buffers.current().current_line() == 2);
    }

    // An addressed D deletes exactly that line and selects its successor.
    {
        fred::DeleteCommandNode command(
            std::make_unique<fred::AbsoluteAddressNode>(1, fred::SourceLocation{}),
            fred::SourceLocation{});
        executor.execute(command, context);
        assert(buffers.current().line_count() == 2);
        assert(buffers.current().line(1) == "gamma");
        assert(buffers.current().line(2) == "delta");
        assert(buffers.current().current_line() == 1);
    }

    // A range can delete the complete buffer; current line becomes zero.
    {
        auto range = std::make_unique<fred::RangeAddressNode>(
            std::make_unique<fred::AbsoluteAddressNode>(1, fred::SourceLocation{}),
            std::make_unique<fred::LastAddressNode>(fred::SourceLocation{}),
            fred::SourceLocation{});
        fred::DeleteCommandNode command(std::move(range), fred::SourceLocation{});
        executor.execute(command, context);
        assert(buffers.current().empty());
        assert(buffers.current().current_line() == 0);
    }

    // D on an empty buffer is a runtime error and leaves the buffer unchanged.
    {
        fred::DeleteCommandNode command(nullptr, fred::SourceLocation{});
        try {
            executor.execute(command, context);
            assert(false && "D on an empty buffer should fail");
        } catch (const fred::CommandExecutionError& error) {
            assert(std::string_view(error.what()) == "current buffer is empty");
        }
        assert(buffers.current().empty());
    }
}
