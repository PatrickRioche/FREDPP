# Livrable Sprint 2.12 — Commandes S et Q

## Version

La version exacte, le commit Git et l'état des sources se consultent dans FREDPP avec `?version`.

## Contenu

- commande `S` sur la ligne courante, une adresse ou une plage ;
- remplacement de toutes les occurrences non chevauchantes ;
- prise en charge de `&` ;
- séparateurs `/`, `?`, `!`, `;` et autres séparateurs symboliques ;
- suffixe `P` pour afficher la dernière ligne modifiée ;
- commande `S` utilisable dans `G` ;
- commandes `Q` et `QQ` ;
- suppression complète de `:quit` dans le REPL et l'aide courante ;
- aides françaises `?s` et `?q` embarquées sans modification des sources de référence ;
- tests `test_substitute` et `test_quit` ;
- passage de 23 à 25 tests.

## Exemples

```fred
S/ancien/nouveau/
*S!ancien!nouveau!
1,5S/^/> /
G/ancien/S/ancien/nouveau/P
Q
QQ
```

## Limites connues

- `Q!<commande TSS>` n'est pas encore pris en charge ;
- la protection des buffers liés à des fichiers sera finalisée avec les futures commandes de lecture et d'écriture ;
- les suites générales de commandes ne sont pas encore disponibles.

## Validation

Le livrable doit être validé par 25/25 tests sous MSVC, GCC et Clang.
