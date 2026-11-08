#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/parser/ParseError.hpp"
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


    // Historical FRED R semantics are isolated in their own buffer so that
    // the pre-existing W/encoding tests remain unchanged.
    buffers.create_or_select("read_history");
    {
        const auto command = parse("R " + quoted(ascii_file));
        executor.execute(*command, context);
        assert(buffers.current().associated_file() &&
               *buffers.current().associated_file() == ascii_file.string());
        assert(!buffers.current().modified());
    }

    const auto second_file = root / "second input.txt";
    write_bytes(second_file, "delta\nomega\n");

    // Unaddressed R still refuses a non-empty current buffer.
    expect_execution_error(
        [&] {
            const auto command = parse("R " + quoted(second_file));
            executor.execute(*command, context);
        },
        "buffer not empty");

    // After *D the buffer is empty but remains associated and modified.
    // Historical R with a new filename must be allowed and replaces the
    // association; a successful complete read leaves the buffer clean.
    buffers.current().erase(1, buffers.current().line_count());
    assert(buffers.current().empty());
    assert(buffers.current().modified());
    assert(buffers.current().associated_file() &&
           *buffers.current().associated_file() == ascii_file.string());
    {
        const auto command = parse("R " + quoted(second_file));
        executor.execute(*command, context);
        assert(buffers.current().line_count() == 2);
        assert(buffers.current().line(1) == "delta");
        assert(buffers.current().line(2) == "omega");
        assert(buffers.current().associated_file() &&
               *buffers.current().associated_file() == second_file.string());
        assert(!buffers.current().modified());
    }

    // Official FRED '*d r': reload the already associated file.
    buffers.current().erase(1, buffers.current().line_count());
    assert(buffers.current().empty());
    assert(buffers.current().modified());
    {
        const auto command = parse("R");
        executor.execute(*command, context);
        assert(buffers.current().line_count() == 2);
        assert(buffers.current().line(1) == "delta");
        assert(buffers.current().line(2) == "omega");
        assert(buffers.current().associated_file() &&
               *buffers.current().associated_file() == second_file.string());
        assert(!buffers.current().modified());
    }

    // $R file appends after the final line, preserves the existing file
    // association, and marks the buffer modified.
    const auto append_file = root / "append input.txt";
    write_bytes(append_file, "tail-one\ntail-two\n");
    {
        const auto command = parse("$R " + quoted(append_file));
        executor.execute(*command, context);
        assert(buffers.current().line_count() == 4);
        assert(buffers.current().line(3) == "tail-one");
        assert(buffers.current().line(4) == "tail-two");
        assert(buffers.current().associated_file() &&
               *buffers.current().associated_file() == second_file.string());
        assert(buffers.current().modified());
        assert(context.counter() == 2);
        assert(context.condition());
    }

    // Addressed R without filename reads the associated file and inserts it
    // after the addressed line without changing the association.
    {
        const auto command = parse("1R");
        executor.execute(*command, context);
        assert(buffers.current().line_count() == 6);
        assert(buffers.current().line(2) == "delta");
        assert(buffers.current().line(3) == "omega");
        assert(buffers.current().associated_file() &&
               *buffers.current().associated_file() == second_file.string());
        assert(buffers.current().modified());
    }
    // $R on an empty buffer uses insertion position zero. This is the
    // historical behavior required by procedures that accumulate files with
    // B(proc) followed by $R file. Addressed R must not create an association.
    buffers.create_or_select("read_empty_dollar");
    assert(buffers.current().empty());
    assert(!buffers.current().associated_file());
    assert(!buffers.current().modified());
    {
        const auto command = parse("$R " + quoted(append_file));
        executor.execute(*command, context);
        assert(buffers.current().line_count() == 2);
        assert(buffers.current().line(1) == "tail-one");
        assert(buffers.current().line(2) == "tail-two");
        assert(!buffers.current().associated_file());
        assert(buffers.current().modified());
        assert(context.counter() == 2);
        assert(context.condition());
    }

    // Other line addresses on an empty buffer remain invalid.
    buffers.create_or_select("read_empty_line_one");
    expect_execution_error(
        [&] {
            const auto command = parse("1R " + quoted(append_file));
            executor.execute(*command, context);
        },
        "cannot address lines in an empty buffer");
    assert(buffers.current().empty());
    assert(!buffers.current().associated_file());
    assert(!buffers.current().modified());

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

    buffers.create_or_select("read_no_file");
    expect_execution_error(
        [&] {
            const auto command = parse("R");
            executor.execute(*command, context);
        },
        "R requires a filename when the buffer has no associated file");

    // R tries a historical whitespace-separated list from left to right.
    // A missing first candidate must not prevent reading the second file.
    const auto missing_first = root / "missing_first.txt";
    const auto available_second = root / "available_second.txt";
    write_bytes(available_second, "fallback-alpha\nfallback-beta\n");
    const auto alternatives =
        missing_first.string() + " " + available_second.string();
    buffers.create_or_select("read_fallback");
    {
        const auto command = parse("R " + alternatives);
        executor.execute(*command, context);
        const auto& buffer = buffers.current();
        assert(buffer.line_count() == 2);
        assert(buffer.line(1) == "fallback-alpha");
        assert(buffer.line(2) == "fallback-beta");
        assert(buffer.associated_file() &&
               *buffer.associated_file() == available_second.string());
        assert(!buffer.modified());
        assert(context.counter() == 2);
        assert(context.condition());
    }

    // Addressed R uses the same candidate selection but leaves the current
    // buffer's existing file association and modification state intact.
    buffers.create_or_select("read_fallback_addressed");
    buffers.current().append("original");
    {
        const auto command = parse("1R " + alternatives);
        executor.execute(*command, context);
        const auto& buffer = buffers.current();
        assert(buffer.line_count() == 3);
        assert(buffer.line(1) == "original");
        assert(buffer.line(2) == "fallback-alpha");
        assert(buffer.line(3) == "fallback-beta");
        assert(!buffer.associated_file());
        assert(buffer.modified());
        assert(context.counter() == 2);
    }

    // Failure of every candidate must leave an empty target unchanged.
    buffers.create_or_select("read_fallback_failure");
    const auto missing_second = root / "missing_second.txt";
    expect_execution_error(
        [&] {
            const auto command = parse(
                "R " + missing_first.string() + " " + missing_second.string());
            executor.execute(*command, context);
        },
        "R cannot read any file from list");
    assert(buffers.current().empty());
    assert(!buffers.current().associated_file());
    assert(!buffers.current().modified());

    // Each filename in an R list may now be quoted to preserve its spaces.
    const auto missing_quoted = root / "missing candidate.txt";
    const auto available_quoted = root / "available candidate.txt";
    write_bytes(available_quoted, "quoted-result\n");
    const auto quoted_alternatives =
        quoted(missing_quoted) + " " + quoted(available_quoted);
    buffers.create_or_select("read_quoted_fallback");
    {
        const auto command = parse("R " + quoted_alternatives);
        executor.execute(*command, context);
        const auto& buffer = buffers.current();
        assert(buffer.line_count() == 1);
        assert(buffer.line(1) == "quoted-result");
        assert(buffer.associated_file() &&
               *buffer.associated_file() == available_quoted.string());
        assert(!buffer.modified());
        assert(context.counter() == 1);
    }

    // Addressed R must preserve file association with quoted alternatives.
    buffers.create_or_select("read_quoted_addressed");
    buffers.current().append("before");
    {
        const auto command = parse("1R " + quoted_alternatives);
        executor.execute(*command, context);
        assert(buffers.current().line_count() == 2);
        assert(buffers.current().line(2) == "quoted-result");
        assert(!buffers.current().associated_file());
        assert(buffers.current().modified());
    }

    // A quoted first candidate wins over a later readable candidate.
    const auto first_quoted = root / "first candidate.txt";
    write_bytes(first_quoted, "first-result\n");
    buffers.create_or_select("read_quoted_first");
    {
        const auto command = parse(
            "R " + quoted(first_quoted) + " " + quoted(available_quoted));
        executor.execute(*command, context);
        assert(buffers.current().line(1) == "first-result");
        assert(buffers.current().associated_file() &&
               *buffers.current().associated_file() == first_quoted.string());
    }

    // Malformed quoted operands are syntax errors, not partial reads.
    try {
        (void)parse("R \"unterminated filename");
        assert(false && "expected ParseError");
    } catch (const fred::ParseError& error) {
        assert(std::string_view(error.what()).find("unterminated quoted") !=
               std::string_view::npos);
    }

    buffers.create_or_select("read_quoted_failure");
    expect_execution_error(
        [&] {
            const auto command = parse(
                "R " + quoted(missing_quoted) + " " +
                quoted(root / "missing second candidate.txt"));
            executor.execute(*command, context);
        },
        "R cannot read any file from list");
    assert(buffers.current().empty());
    assert(!buffers.current().associated_file());
    assert(!buffers.current().modified());

    std::filesystem::remove_all(root);
    return 0;
}
