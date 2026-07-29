#include "fredpp/version.hpp"

#include <iostream>

int main() {
    if (fredpp::version() != "0.0.2") {
        std::cerr << "Unexpected version: " << fredpp::version() << '\n';
        return 1;
    }
    return 0;
}
