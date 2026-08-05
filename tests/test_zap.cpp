#include "fred/ast/AbsoluteAddressNode.hpp"
#include "fred/ast/CommandNode.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <string_view>

namespace {

class StringOutput final : public fred::Output {
public:
    void write(std::string_view text) override { content_.append(text); }
    [[nodiscard]] const std::string& content() const noexcept { return content_; }
private:
    std::string content_;
};

} // namespace

int main() {
    fred::BufferManager buffers;
    buffers.current().append("one");
    buffers.current().append("two");
    buffers.current().append("three");

    StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    fred::ZapCommandNode command(
        std::make_unique<fred::AbsoluteAddressNode>(2, fred::SourceLocation{}),
        fred::SourceLocation{});
    executor.execute(command, context);

    assert(buffers.current().current_line() == 2);
    assert(output.content().empty());
}
