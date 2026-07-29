#include "fredpp/version.hpp"

#include <iostream>

int main() {
    std::cout << "FREDPP v" << fredpp::version() << '\n';
    std::cout << "Foundation repository initialized successfully.\n";
    return 0;
}
