#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"
#include "fredpp/version.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>

#if defined(FREDPP_TEST_STUB_VERSION)
namespace fredpp {
std::string_view version() noexcept { return "0.0.70"; }
std::string_view git_commit() noexcept { return "test"; }
std::string_view source_state() noexcept { return "test"; }
bool source_is_dirty() noexcept { return false; }
}
#endif

namespace {
void check(bool passed, std::string_view text) {
    if (!passed) {
        std::cerr << "FAIL: " << text << '\n';
        std::exit(EXIT_FAILURE);
    }
}
void run(std::string_view text, fred::CommandExecutor& executor,
         fred::ExecutionContext& context) {
    fred::Lexer lexer(text);
    fred::TokenStream tokens(lexer);
    const auto commands = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, commands);
    executor.execute(*parser.parse(), context);
}
bool fails(std::string_view text, fred::CommandExecutor& executor,
           fred::ExecutionContext& context) {
    try { run(text, executor, context); }
    catch (const std::exception&) { return true; }
    return false;
}
std::string contents(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}
}

int main() {
    fred::CommandExecutor executor;
    {
        fred::BufferManager buffers;
        fred::StringOutput output;
        fred::ExecutionContext context(buffers, output);
        run("FV", executor, context);
        check(output.content().find("FREDPP v0.0.70") != std::string::npos,
              "FV prints current compiled version");
        output.clear();
        run("JE/Erreur numero 5/", executor, context);
        check(output.content() == "Erreur numero 5\n" &&
              !context.exit_requested(), "JE prints error without exiting by default");
        run("O+Q", executor, context);
        run("JE/Fatal/", executor, context);
        check(context.quit_on_error() && context.exit_requested(),
              "O+Q terminates after JE");
    }
    {
        fred::BufferManager buffers;
        fred::StringOutput output;
        fred::ExecutionContext context(buffers, output);
        auto& buffer = buffers.current();
        buffer.append("abc");
        run("O-S&", executor, context);
        run("S/abc/& &/", executor, context);
        check(buffer.line(1) == "& &", "O-S& leaves ampersands literal");
        run("O+S&", executor, context);
        run("S/&/(&)/", executor, context);
        check(buffer.line(1) == "(&) (&)", "O+S& expands matched text");
        run("O+SD", executor, context);
        buffer.append("AbC");
        run("2T/abc/", executor, context);
        check(context.condition(), "OSD enables case-insensitive search");
        run("O-SD", executor, context);
        run("2T/abc/", executor, context);
        check(!context.condition(), "OSD disables case-insensitive search");
        check(fails("O+T20,10", executor, context),
              "OT rejects unordered tab positions");
        check(fails("O+T0", executor, context),
              "OT rejects invalid tab positions");
        run("O+T10,20", executor, context);
        check(context.tab_stops().size() == 2 &&
              context.tab_stops()[0] == 10, "OT changes tab stops");
        buffer.append("abc      def");
        run("3ZI", executor, context);
        check(buffer.line(3) == "abc\tdef" && context.counter() == 1,
              "ZI compresses aligned spaces into tabs");
        run("3ZO", executor, context);
        check(buffer.line(3) == "abc      def" && context.counter() == 1,
              "ZO expands tabs with active tab stops");
        run("O-T", executor, context);
        check(!context.tabs_enabled(), "O-T disables tab display option");
    }
    {
        fred::BufferManager buffers;
        fred::StringOutput output;
        fred::ExecutionContext context(buffers, output);
        const auto root = std::filesystem::temp_directory_path() /
            ("fredpp_lot_61_70_" + std::to_string(
                std::chrono::steady_clock::now().time_since_epoch().count()));
        std::filesystem::create_directory(root);
        const auto filename = root / "fredpp_lot_61_70_input.txt";
        const auto append_file = root / "fredpp_lot_61_70_append.txt";
        {
            std::ofstream file(filename, std::ios::binary | std::ios::trunc);
            file << "Alpha\nBravo\n";
        }
        {
            std::ofstream file(append_file, std::ios::binary | std::ios::trunc);
            file << "First";
        }
        run("ZR " + filename.string(), executor, context);
        check(buffers.current().name() == "a" &&
              buffers.current().line_count() == 2 &&
              !buffers.current().modified() &&
              context.counter() == 2, "ZR allocates and loads a file buffer");
        buffers.current().append("Edited");
        run("ZR " + filename.string(), executor, context);
        check(buffers.current().name() == "a" &&
              buffers.current().line_count() == 3,
              "ZR reselects an existing associated buffer without reloading");
        run("1,2ZW " + append_file.string(), executor, context);
        check(contents(append_file) == "First\nAlpha\nBravo\n" &&
              buffers.current().modified(),
              "ZW appends lines without clearing the dirty flag");
        check(fails("1ZR " + filename.string(), executor, context) &&
              fails("ZWU /missing-directory/fredpp_lot61.txt", executor, context),
              "ZR invalid address and ZWU invalid path rejected");
        std::filesystem::remove(filename);
        std::filesystem::remove(append_file);
        std::filesystem::remove(root);
    }
    std::cout << "FREDPP lot 61-70 OK\n";
}
