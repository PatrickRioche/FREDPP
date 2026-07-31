# FREDPP — Sprint 2.5 — Move (`M`)

## Objet

Ajout de la commande `M` pour déplacer une ligne ou une plage après une ligne de destination.

## Syntaxe

```text
[address[,address]]Mdestination
```

Exemples :

```text
2M5
2,3M5
$M0
```

La destination `0` place le bloc au début du buffer. La destination est évaluée dans le buffer avant le déplacement.

## Règles

- Sans adresse source, `M` déplace la ligne courante.
- L'ordre des lignes déplacées est conservé.
- La ligne courante devient la dernière ligne du bloc déplacé.
- Une destination située dans la plage source est refusée.
- La destination doit être une adresse unique, pas une plage.

## Fichiers modifiés

- `include/fred/ast/AstNode.hpp`
- `include/fred/ast/CommandNode.hpp`
- `src/command/CommandRegistry.cpp`
- `src/lexer/Lexer.cpp`
- `src/parser/CommandParser.cpp`
- `src/runtime/CommandExecutor.cpp`
- `src/cli/main.cpp`
- `tests/test_command_parser.cpp`
- `tests/test_lexer.cpp`
- `tests/CMakeLists.txt`

## Fichier ajouté

- `tests/test_move.cpp`

## Validation attendue

```text
16/16 tests passed
```

## Recette fonctionnelle

```text
A
1
2
3
4
5
\F

2,3M5
1,$P
```

Résultat :

```text
1
4
5
2
3
```

Puis :

```text
5M0
1,$P
```

Résultat :

```text
3
1
4
5
2
```

Cas d'erreur :

```text
2,4M3
```

Résultat attendu :

```text
error: M destination lies inside the moved range
```

## Commit conseillé

```text
Sprint 2.5 - Implement Move command
```
