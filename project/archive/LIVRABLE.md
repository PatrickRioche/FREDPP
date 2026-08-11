# FREDPP — Sprint 2.4 — Commande C (Change)

## Objectif

Ajouter la commande `C`, qui remplace la ligne ou la plage adressée par le bloc de texte saisi jusqu'à `\F`.

## Comportements couverts

- `C` : remplace la ligne courante.
- `nC` : remplace la ligne `n`.
- `n,mC` : remplace la plage `n` à `m`.
- `1,$C` : remplace le buffer complet.
- Plusieurs lignes de remplacement sont acceptées.
- `\F` immédiat supprime la ligne ou la plage sans insérer de texte.
- La dernière ligne insérée devient la ligne courante.

## Fichiers modifiés

- `include/fred/ast/AstNode.hpp`
- `include/fred/ast/CommandNode.hpp`
- `include/fred/runtime/CommandExecutor.hpp`
- `src/command/CommandRegistry.cpp`
- `src/runtime/CommandExecutor.cpp`
- `src/cli/main.cpp`
- `tests/CMakeLists.txt`
- `tests/test_command_parser.cpp`

## Nouveau fichier

- `tests/test_change.cpp`

## Validation locale

Compilation Linux réussie et `15/15` tests CTest réussis.

La validation Windows reste à effectuer avec :

```powershell
cd scripts
.\rebuild.bat
```
