#include "fredpp/version.hpp"

#include <iostream>

int main() {
    std::cout << "FREDPP v" << fredpp::version() << '\n'
              << "Commit Git : " << fredpp::git_commit() << '\n'
              << "État des sources : " << fredpp::source_state() << '\n';
    return 0;
}
