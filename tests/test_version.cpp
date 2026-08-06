#include "fredpp/version.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string_view>

namespace {

bool is_git_hash(std::string_view value) {
    return value.size() >= 7 &&
           std::all_of(value.begin(), value.end(), [](unsigned char ch) {
               return std::isxdigit(ch) != 0;
           });
}

} // namespace

int main() {
    if (fredpp::version() != "0.0.7") {
        std::cerr << "Unexpected version: " << fredpp::version() << '\n';
        return 1;
    }

    if (fredpp::git_commit() != "inconnu" && !is_git_hash(fredpp::git_commit())) {
        std::cerr << "Unexpected Git commit: " << fredpp::git_commit() << '\n';
        return 1;
    }

    const auto state = fredpp::source_state();
    if (state != "propre" && state != "modifié" && state != "inconnu") {
        std::cerr << "Unexpected source state: " << state << '\n';
        return 1;
    }

    if (fredpp::source_is_dirty() != (state == "modifié")) {
        std::cerr << "Dirty flag and source state disagree\n";
        return 1;
    }

    return 0;
}
