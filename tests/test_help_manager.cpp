#include "HelpManager.h"

#include <algorithm>
#include <cassert>
#include <initializer_list>
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
    assert(help.exists("f"));
    assert(help.exists("fb"));
    assert(help.exists("ff"));
    assert(help.exists("fn"));
    assert(help.exists("n:"));
    assert(help.exists("ncolon"));
    for (const auto* topic : {
             "n+", "n-", "n*", "n/", "n%", "n=", "n<", "n>", "n&", "n|",
             "n~", "n^", "n{", "n}", "na", "nb", "nd", "nf", "nl", "np"}) {
        assert(help.exists(topic));
        assert(!help.load(topic).empty());
    }
    assert(help.exists("fq"));
    assert(help.exists("f?"));
    assert(help.exists("="));
    assert(help.exists("fo"));
    for (const auto* topic : {"fv", "je", "zi", "zo", "zr", "zw",
                              "oq", "os&", "osd", "ot"}) {
        assert(help.exists(topic));
        assert(!help.load(topic).empty());
    }
    for (const auto* topic : {"#", "@", "window", "wait", "y",
                              "oia", "oic", "oir", "oil", "oiu"}) {
        assert(help.exists(topic));
        assert(!help.load(topic).empty());
    }
    for (const auto* topic : {"oc", "oi\\f", "oie", "oif", "ool",
                              "ooo", "oou", "op", "os/", "jo"}) {
        assert(help.exists(topic));
        assert(!help.load(topic).empty());
    }
    for (const auto* topic : {"e", "fe", "j", "jb", "u", "v",
                              "ob", "oo", "or", "os", "zv"}) {
        assert(help.exists(topic));
        assert(!help.load(topic).empty());
    }
    assert(help.exists("jm"));
    assert(help.exists("jp"));
    assert(help.exists("g"));
    assert(help.exists("q"));
    assert(help.exists("r"));
    assert(help.exists("s"));
    assert(help.exists("w"));
    assert(help.exists("wu"));
    assert(help.exists("oi("));
    assert(help.exists("\""));
    assert(help.exists("om"));
    assert(help.exists("procedure"));
    assert(help.exists("limites"));
    assert(help.exists("directives"));
    assert(help.load("directives").find("W") != std::string::npos);
    const auto limits_page = help.load("limites");
    assert(limits_page.find("FREDPP_PROCEDURE_MAX_STEPS") != std::string::npos);
    assert(limits_page.find("FREDPP_REPEAT_MAX_SECONDS") != std::string::npos);
    assert(help.exists("!"));
    assert(help.exists("zg"));
    assert(help.exists("z"));
    for (const auto* topic : {"za", "zb", "zc", "zd", "zl", "zm",
                              "zp", "zs", "zt", "zu"}) {
        assert(help.exists(topic));
        assert(!help.load(topic).empty());
    }
    assert(help.exists("HELP"));
    assert(help.exists("  h  "));
    assert(!help.exists("rubrique-inconnue"));

    const auto index = help.load_for_terminal("");
    assert(index.find("AIDE FRED — COMMANDES DISPONIBLES") != std::string::npos);
    assert(index.find("    ?limites") != std::string::npos);
    assert(index.find("    ?directives") != std::string::npos);
    assert(index.find("    ?fb") != std::string::npos);
    assert(index.find("    ?f ") != std::string::npos);
    assert(index.find("    ?ff") != std::string::npos);
    assert(index.find("    ?fn") != std::string::npos);
    assert(index.find("    ?n:") != std::string::npos);
    assert(index.find("    ?ncolon") == std::string::npos);
    assert(index.find("    ?n+") != std::string::npos);
    assert(index.find("    ?n|") != std::string::npos);
    for (const auto* topic : {
             "    ?n~", "    ?n^", "    ?n{", "    ?n}", "    ?na",
             "    ?nb", "    ?nd", "    ?nf", "    ?nl", "    ?np"}) {
        assert(index.find(topic) != std::string::npos);
    }
    assert(index.find("    ?f?") != std::string::npos);
    assert(index.find("    ?fq") == std::string::npos);
    assert(index.find("    ?=") != std::string::npos);
    assert(index.find("    ?fo") != std::string::npos);
    assert(index.find("    ?jm") != std::string::npos);
    assert(index.find("    ?jp") != std::string::npos);
    assert(index.find("    ?g") != std::string::npos);
    assert(index.find("    ?q") != std::string::npos);
    assert(index.find("    ?r") != std::string::npos);
    assert(index.find("    ?s") != std::string::npos);
    assert(index.find("    ?w") != std::string::npos);
    assert(index.find("    ?wu") == std::string::npos);
    assert(index.find("    ?oi(") != std::string::npos);
    assert(index.find("    ?om") != std::string::npos);
    for (const auto* topic : {"    ?fv", "    ?je", "    ?zi", "    ?zo",
                              "    ?zr", "    ?zw", "    ?oq", "    ?os&",
                              "    ?osd", "    ?ot"}) {
        assert(index.find(topic) != std::string::npos);
    }
    for (const auto* topic : {"    ?#", "    ?@", "    ?window", "    ?wait",
                              "    ?y", "    ?oia", "    ?oic", "    ?oir",
                              "    ?oil", "    ?oiu"}) {
        assert(index.find(topic) != std::string::npos);
    }
    for (const auto* topic : {"    ?oc", "    ?oi\\f", "    ?oie", "    ?oif",
                              "    ?ool", "    ?ooo", "    ?oou", "    ?op",
                              "    ?os/", "    ?jo"}) {
        assert(index.find(topic) != std::string::npos);
    }
    assert(index.find("    ?procedure") == std::string::npos);
    assert(index.find("    ?z") != std::string::npos);
    assert(index.find("    ?\"") != std::string::npos);
    assert(index.find("    ?*") != std::string::npos);
    assert(index.find("    ?index") == std::string::npos);
    assert(index.find("    ?:") == std::string::npos);
    assert(index.find("    ?version") == std::string::npos);
    assert(index.find("- `?g`") == std::string::npos);

    const auto special = help.load_for_terminal(":");
    assert(special.find("COMMANDES FREDPP") != std::string::npos);
    assert(special.find("    :quit") == std::string::npos);
    assert(special.find("    Q                        Quitter normalement") == std::string::npos);
    assert(special.find("    QQ                       Quitter immédiatement") == std::string::npos);
    assert(special.find("    :buffers") == std::string::npos);
    assert(special.find("    :help") == std::string::npos);
    assert(special.find("    :new") == std::string::npos);
    assert(special.find("    :use") == std::string::npos);
    assert(special.find("    :append") == std::string::npos);
    assert(special.find("    :insert") == std::string::npos);
    assert(special.find("    :delete") == std::string::npos);
    assert(special.find("    :print") != std::string::npos);
    assert(special.find("    :flow <buffer>") != std::string::npos);
    assert(special.find("    :pattern <modèle>") != std::string::npos);
    assert(special.find("    :cls") != std::string::npos);
    assert(special.find("    Page Up") != std::string::npos);
    assert(special.find("    Page Down") != std::string::npos);
    assert(special.find("Quitter l\'aide") != std::string::npos);
    assert(special.find("    ?:") != std::string::npos);
    assert(special.find("    ?version") != std::string::npos);
    assert(special.find("    ?wu") != std::string::npos);
    assert(special.find("    ?procedure") != std::string::npos);
    assert(special.find("Pour l'aide des commandes FRED") != std::string::npos);
    assert(special.find("| Commande |") == std::string::npos);
    assert(special.find("## GESTION") == std::string::npos);

    const auto equals = help.load_for_terminal("=");
    assert(equals.find("IMPRESSION DU NUMÉRO DE LIGNE") != std::string::npos);

    const auto changed_buffers = help.load_for_terminal("F?");
    assert(changed_buffers.find("LISTE DES BUFFERS MODIFIÉS") !=
           std::string::npos);
    assert(help.load_for_terminal("fq") == changed_buffers);

    const auto current_buffer_help = help.load_for_terminal("f");
    assert(current_buffer_help.find("INFORMATIONS SUR LE BUFFER") !=
           std::string::npos);
    assert(help.load("f").find("F <nom de fichier>") !=
           std::string::npos);

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

    const auto facts_buffers = help.load_for_terminal("fb");
    assert(facts_buffers.find("FB") != std::string::npos);

    const auto facts_options = help.load_for_terminal("fo");
    assert(facts_options.find("FO") != std::string::npos);

    const auto facts_registers = help.load_for_terminal("fn");
    assert(facts_registers.find("FN") != std::string::npos);
    assert(facts_registers.find("REGISTRES") != std::string::npos);

    const auto numeric_assignment = help.load_for_terminal("N:");
    assert(numeric_assignment.find("AFFECTATION DE VALEUR") !=
           std::string::npos);
    assert(help.load_for_terminal("ncolon") == numeric_assignment);

    const auto option_parenthesis = help.load_for_terminal("oi(");
    assert(option_parenthesis.find("OI(") != std::string::npos);

    const auto monitor = help.load_for_terminal("om");
    assert(monitor.find("OPTION MONITOR") != std::string::npos);

    const auto procedure = help.load_for_terminal("procedure");
    assert(procedure.find("\\B(buffer)") != std::string::npos);

    const auto comment = help.load_for_terminal("\"");
    assert(comment.find("COMMENTAIRE") != std::string::npos);

    const auto jump_message = help.load_for_terminal("jm");
    assert(jump_message.find("JUMP MESSAGE") != std::string::npos);

    const auto jump_prompt = help.load_for_terminal("jp");
    assert(jump_prompt.find("JUMP PROMPT") != std::string::npos);

    const auto global = help.load_for_terminal("g");
    assert(global.find("G — GLOBAL COMMANDE") != std::string::npos);

    const auto read = help.load_for_terminal("r");
    assert(read.find("R — LECTURE") != std::string::npos);

    const auto substitute = help.load_for_terminal("s");
    assert(substitute.find("S — ( SUBSTITUTION )") != std::string::npos);

    const auto quit = help.load_for_terminal("q");
    assert(quit.find("Q — ARRÊT") != std::string::npos);

    const auto write = help.load_for_terminal("w");
    assert(write.find("W — ECRITURE") != std::string::npos);

    const auto wu = help.load_for_terminal("wu");
    assert(wu.find("WU — ÉCRITURE UTF-8") != std::string::npos);

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
    assert(std::find(topics.begin(), topics.end(), "fb") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "fn") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "ncolon") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "fo") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "jm") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "jp") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "g") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "q") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "r") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "s") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "w") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "wu") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "oi(") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "\"") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "om") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "procedure") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "!") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "zg") != topics.end());
    assert(std::find(topics.begin(), topics.end(), "z") != topics.end());

    std::cout << "help manager tests passed\n";
}
