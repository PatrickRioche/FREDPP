#pragma once

#include <cstddef>

namespace fredpp {

enum class PagerKey {
    PageUp,
    PageDown,
    Quit,
    Other,
};

// Efface le terminal interactif et replace le curseur en haut à gauche.
// Cette primitive est partagée par :cls et par le pager d'aide.
void clear_terminal();

// Indique si l'entrée standard est reliée à un terminal interactif.
[[nodiscard]] bool stdin_is_terminal() noexcept;

// Retourne la hauteur visible du terminal. Une valeur de repli est utilisée
// lorsque la taille ne peut pas être déterminée.
[[nodiscard]] std::size_t terminal_rows() noexcept;

// Lit une touche utile au pager sans exiger la touche Entrée.
[[nodiscard]] PagerKey read_pager_key();

} // namespace fredpp
