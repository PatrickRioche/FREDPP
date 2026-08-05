#include "fredpp/version.hpp"

#include <iostream>

int main() {
    if (fredpp::version() != "0.0.4") {
        std::cerr << "Unexpected version: " << fredpp::version() << '\n';
        return 1;
    }
    return 0;
}
