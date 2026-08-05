#include "fred/parser/PatternParser.hpp"
#include "fred/runtime/PatternMatcher.hpp"

#include <cassert>
#include <memory>
#include <string_view>

namespace {

bool matches(std::string_view pattern, std::string_view text) {
    fred::PatternParser parser(pattern);
    const auto node = parser.parse();
    return fred::PatternMatcher{}.search(*node, text);
}

} // namespace

int main() {
    assert(matches("/abc/", "xxabcxx"));
    assert(!matches("/abc/", "ab"));
    assert(matches("/^abc/", "abcdef"));
    assert(!matches("/^abc/", "xabc"));
    assert(matches("/abc$/", "xxabc"));
    assert(!matches("/abc$/", "abcx"));
    assert(matches("/ab*/", "a"));
    assert(matches("/ab+/", "abbb"));
    assert(!matches("/ab+/", "a"));
    assert(matches("/(ab|cd)+/", "xxabcdyy"));
    assert(matches("/[a-z0-9]+/", "---abc123---"));
    assert(matches("/[^0-9]+/", "123abc456"));
    assert(matches("/^$/", ""));
    assert(!matches("/^$/", "x"));
}
