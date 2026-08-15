#pragma once

namespace fredpp {

// Efface le terminal interactif et replace le curseur en haut à gauche.
// Cette primitive est partagée par :cls et par le pager d'aide.
void clear_terminal();

} // namespace fredpp
