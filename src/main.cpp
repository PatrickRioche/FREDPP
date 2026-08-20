#include "fredpp/version.hpp"

#include <iostream>

/**
 * @brief Legacy/minimal standalone version-reporting entry point.
 *
 * This source file is retained in the repository but is not the desktop
 * `fredpp` executable entry point selected by the current root CMakeLists.txt.
 * The active CLI target is built from `src/cli/main.cpp`.
 *
 * Keeping this distinction documented prevents future readers from mistaking
 * this small program for the production REPL/bootstrap front end.
 *
 * @return 0 after writing build-generated version/Git/source-state metadata.
 */
int main() {
    std::cout << "FREDPP v" << fredpp::version() << '\n'
              << "Commit Git : " << fredpp::git_commit() << '\n'
              << "État des sources : " << fredpp::source_state() << '\n';
    return 0;
}
