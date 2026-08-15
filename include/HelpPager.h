#pragma once

#include <string_view>

namespace fredpp {

// Affiche une rubrique d'aide dans un pager interactif lorsque stdin est
// un terminal. En contexte redirigé/non interactif, imprime tout le texte.
void show_paged_help(std::string_view text);

} // namespace fredpp
