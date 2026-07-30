# FREDPP — Sprint 2.2 — Commande I (Insert)

## Objectif

Ajouter la commande historique `I`, insérant les lignes saisies avant la ligne adressée.

## Fichiers à recopier

Recopier le contenu de ce ZIP à la racine du dépôt FREDPP en conservant l'arborescence.

Fichiers modifiés :

- `include/fred/ast/AstNode.hpp`
- `include/fred/ast/CommandNode.hpp`
- `include/fred/runtime/CommandExecutor.hpp`
- `src/command/CommandRegistry.cpp`
- `src/runtime/CommandExecutor.cpp`
- `src/cli/main.cpp`
- `tests/test_command_parser.cpp`
- `tests/test_append.cpp`

## Comportement ajouté

- `I` : insertion avant la ligne courante.
- `nI` : insertion avant la ligne `n`.
- `0I` : insertion au début du buffer.
- `I` sur un buffer vide : création des premières lignes.
- Commande insensible à la casse (`i`).
- Lecture du texte jusqu'à une ligne contenant `\F`.
- Les plages d'adresses sont refusées pour `I`.

## Validation automatique effectuée

Sous Linux :

- configuration CMake : OK ;
- compilation : OK ;
- CTest : 13/13 tests réussis.

## Commandes de test sous Windows

```bat
scripts\rebuild.bat
```

ou :

```bat
scripts\build.bat
scripts\test.bat
```

## Test manuel conseillé

```text
A
alpha
beta
gamma
\F
2I
avant beta
\F
1,$P
```

Résultat attendu :

```text
alpha
avant beta
beta
gamma
```
