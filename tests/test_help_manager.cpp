#include "HelpManager.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

int main() {
    fredpp::HelpManager help;

    assert(help.exists("index"));
    assert(help.exists(":"));
    assert(help.exists("*"));
    assert(help.exists("a"));
    assert(help.exists("b"));
    assert(help.exists("g"));
    assert(help.exists("z"));
    assert(help.exists("HELP"));
    assert(help.exists("  h  "));
    assert(!help.exists("s"));
    assert(!help.exists("rubrique-inconnue"));

    const auto index = help.load_for_terminal("");
    assert(index.find("AIDE FREDPP — COMMANDES FRED DISPONIBLES") != std::string::npos);
    assert(index.find("    ?g") != std::string::npos);
    assert(index.find("    ?z") != std::string::npos);
    assert(index.find("    ?*") != std::string::npos);
    assert(index.find("    ?:") != std::string::npos);
    assert(index.find("    ?s") == std::string::npos);
    assert(index.find("- `?g`") == std::string::npos);

    const auto special = help.load_for_terminal(":");
    assert(special.find("COMMANDES SPÉCIALES DE FREDPP") != std::string::npos);
    assert(special.find("    :quit") != std::string::npos);
    assert(special.find("    :buffers") != std::string::npos);
    assert(special.find("    :print") != std::string::npos);
    assert(special.find("    :flow <buffer>") != std::string::npos);
    assert(special.find("    :pattern <modèle>") != std::string::npos);
    assert(special.find("| Commande |") == std::string::npos);
    assert(special.find("## GESTION") == std::string::npos);

    const auto star = help.load_for_terminal("*");
    assert(star.find("ALIAS DE LA TOTALITÉ DU BUFFER") != std::string::npos);
    assert(star.find("1,$P") != std::string::npos);
    assert(star.find("*D") != std::string::npos);

    // The embedded Markdown remains unchanged and can still be consumed raw.
    const auto raw_append = help.load("a");
    assert(raw_append.find("# A — Insertion") != std::string::npos);
    assert(raw_append.find("| Élément | Description |") != std::string::npos);
    assert(raw_append.find("```fred") != std::string::npos);

    // Terminal rendering improves headings, parameter tables, examples and
    // compatibility without changing the Markdown source files.
    const auto append = help.load_for_terminal("a");
    assert(append.find("============================================================") != std::string::npos);
    assert(append.find("PARAMÈTRES") != std::string::npos);
    assert(append.find("Élément") != std::string::npos);
    assert(append.find("Description") != std::string::npos);
    assert(append.find("(.)") != std::string::npos);
    assert(append.find("[FRED]") != std::string::npos);
    assert(append.find("COMPATIBILITÉ") != std::string::npos);
    assert(append.find("FRED historique") != std::string::npos);
    assert(append.find("| Élément |") == std::string::npos);
    assert(append.find("```fred") == std::string::npos);
    assert(append.find("## Paramètres") == std::string::npos);

    const auto buffers = help.load_for_terminal("B");
    assert(buffers.find("B — POSITIONNEMENT SUR UN BUFFER") != std::string::npos);

    const auto global = help.load_for_terminal("g");
    assert(global.find("G — GLOBAL COMMANDE") != std::string::npos);

    const auto zap = help.load_for_terminal("Z");
    assert(zap.find("Z — COMMANDES DIVERSES") != std::string::npos);

    bool missing_rejected = false;
    try {
        (void)help.load("rubrique-inconnue");
    } catch (const std::runtime_error& error) {
        missing_rejected =
            std::string(error.what()).find("rubrique-inconnue") != std::string::npos;
    }
    assert(missing_rejected);

    const auto topics = help.topics();
    assert(std::is_sorted(topics.begin(), topics.end()));
    assert(std::find(topics.begin(), topics.end(), "index") != topics.end());
    assert(std::find(topics.begin(), topics.end(), ":") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "*") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "b") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "g") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "z") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "s") == topics.end());

    std::cout << "help manager tests passed\n";
}
