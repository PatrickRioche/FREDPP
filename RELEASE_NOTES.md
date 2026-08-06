# Notes de version FREDPP

La version exacte et le commit de construction se consultent avec `?version`.

## Sprint 2.13

- ajout de la commande historique `R` ;
- ajout de `W` et de la forme historique `WA` ;
- ajout de l'extension FREDPP `WU` pour UTF-8 sans BOM ;
- reconnaissance explicite de `WB`, non pris en charge car lié au GCOS/BCD ;
- détection ASCII ou UTF-8 et conservation de LF ou CRLF ;
- suivi des buffers propres ou modifiés et protection de `Q` ;
- intégration des aides `?r`, `?w` et `?wu` ;
- passage à 26 tests.

Consulter `ROADMAP.md`, `CHANGELOG.md` et `LIVRABLE_SPRINT_2.13.md` pour le détail du périmètre et des limites.
