#include "fred/core/Buffer.hpp"
#include "fred/core/BufferManager.hpp"

#include <cstdlib>
#include <iostream>

namespace {
void expect(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}
}

int main() {
    fred::Buffer buffer("demo");
    expect(buffer.empty(), "new buffer is empty");
    expect(buffer.current_line() == 0, "empty current line is zero");

    buffer.append("one");
    buffer.append("three");
    buffer.insert_before(2, "two");

    expect(buffer.line_count() == 3, "three lines");
    expect(buffer.line(2) == "two", "insert works");
    expect(buffer.current_line() == 2, "inserted line current");

    buffer.replace(2, "TWO");
    expect(buffer.line(2) == "TWO", "replace works");

    buffer.erase(2, 2);
    expect(buffer.line_count() == 2, "erase works");
    expect(buffer.line(2) == "three", "renumber after erase");

    fred::BufferManager manager;
    expect(manager.contains("0"), "default buffer exists");

    std::cout << "Buffer tests passed.\n";
    return EXIT_SUCCESS;
}
