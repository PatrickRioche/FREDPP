#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutionError.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>

namespace {

std::unique_ptr<fred::CommandNode> parse(std::string_view source) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    return parser.parse();
}

std::string quoted(const std::filesystem::path& path) {
    return "\"" + path.string() + "\"";
}

void write_bytes(const std::filesystem::path& path, std::string_view bytes) {
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    assert(output);
    output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    assert(output);
}

std::string read_bytes(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    assert(input);
    return std::string(std::istreambuf_iterator<char>(input),
                       std::istreambuf_iterator<char>());
}

template <typename Action>
void expect_execution_error(Action&& action, std::string_view fragment) {
    try {
        action();
        assert(false && "expected CommandExecutionError");
    } catch (const fred::CommandExecutionError& error) {
        assert(std::string_view(error.what()).find(fragment) !=
               std::string_view::npos);
    }
}

} // namespace

int main() {
    const auto unique = std::to_string(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    const auto root = std::filesystem::temp_directory_path() /
                      ("fredpp_file_io_" + unique);
    std::filesystem::create_directories(root);

    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    const auto ascii_file = root / "ascii input.txt";
    write_bytes(ascii_file, "alpha\r\nbeta\r\n");
    {
        const auto command = parse("R " + quoted(ascii_file));
        executor.execute(*command, context);
        const auto& buffer = buffers.current();
        assert(buffer.line_count() == 2);
        assert(buffer.line(1) == "alpha");
        assert(buffer.line(2) == "beta");
        assert(buffer.encoding() == fred::TextEncoding::Ascii);
        assert(buffer.line_ending() == fred::LineEnding::CrLf);
        assert(buffer.final_newline());
        assert(buffer.associated_file() &&
               *buffer.associated_file() == ascii_file.string());
        assert(!buffer.modified());
    }

    buffers.current().replace(2, "gamma");
    assert(buffers.current().modified());
    {
        const auto command = parse("W");
        executor.execute(*command, context);
        assert(read_bytes(ascii_file) == "alpha\r\ngamma\r\n");
        assert(!buffers.current().modified());
    }

    const auto utf8_input = root / "utf8 input.txt";
    const auto utf8_output = root / "utf8 output.txt";
    write_bytes(utf8_input, std::string("\xEF\xBB\xBF") + "café\n");
    buffers.create_or_select("utf8");
    {
        const auto command = parse("R " + quoted(utf8_input));
        executor.execute(*command, context);
        assert(buffers.current().encoding() == fred::TextEncoding::Utf8);
        assert(buffers.current().line(1) == "café");
    }
    {
        const auto command = parse("WU " + quoted(utf8_output));
        executor.execute(*command, context);
        assert(read_bytes(utf8_output) == "café\n");
        assert(buffers.current().associated_file() &&
               *buffers.current().associated_file() == utf8_output.string());
        assert(!buffers.current().modified());
    }

    buffers.create_or_select("ascii_fail");
    buffers.current().append("café");
    const auto ascii_fail = root / "ascii fail.txt";
    expect_execution_error(
        [&] {
            const auto command = parse("WA " + quoted(ascii_fail));
            executor.execute(*command, context);
        },
        "cannot be represented in ASCII");
    assert(buffers.current().modified());

    buffers.create_or_select("partial");
    buffers.current().append("one");
    buffers.current().append("two");
    buffers.current().append("three");
    const auto partial = root / "partial.txt";
    {
        const auto command = parse("1,2WU " + quoted(partial));
        executor.execute(*command, context);
        assert(read_bytes(partial) == "one\ntwo\n");
        assert(buffers.current().modified());
        assert(!buffers.current().associated_file());
    }

    expect_execution_error(
        [&] {
            const auto command = parse("WB " + quoted(root / "legacy.bcd"));
            executor.execute(*command, context);
        },
        "GCOS/BCD");

    buffers.create_or_select("no_file");
    expect_execution_error(
        [&] {
            const auto command = parse("W");
            executor.execute(*command, context);
        },
        "requires a filename");

    std::filesystem::remove_all(root);
    return 0;
}
