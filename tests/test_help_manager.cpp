#include "HelpManager.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

int main() {
    fredpp::HelpManager help;

    assert(help.exists("index"));
    assert(help.exists("b"));
    assert(help.exists("HELP"));
    assert(help.exists("  h  "));
    assert(!help.exists("rubrique-inconnue"));

    const auto index = help.load("");
    assert(index.find("# Aide FREDPP") != std::string::npos);

    const auto buffers = help.load("B");
    assert(buffers.find("# B — Positionnement sur un buffer") != std::string::npos);

    bool missing_rejected = false;
    try {
        (void)help.load("rubrique-inconnue");
    } catch (const std::runtime_error& error) {
        missing_rejected = std::string(error.what()).find("rubrique-inconnue") != std::string::npos;
    }
    assert(missing_rejected);

    const auto topics = help.topics();
    assert(std::is_sorted(topics.begin(), topics.end()));
    assert(std::find(topics.begin(), topics.end(), "index") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "b") != topics.end());

    std::cout << "help manager tests passed\n";
}
