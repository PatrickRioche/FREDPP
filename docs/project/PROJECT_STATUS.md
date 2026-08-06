# État du projet

## Sprint courant

Sprint 2.11 — alias historique `*` pour la plage complète `1,$`.

## Version logicielle

La version exacte n'est pas recopiée dans ce document. Elle se consulte dans l'exécutable avec :

```text
?version
```

## Validation attendue

- Windows / MSVC : 23/23 tests ;
- Debian / GCC : 23/23 tests ;
- Debian / Clang : 23/23 tests.

## Fonctions ajoutées

- `*` reconnu comme alias de la plage `1,$` au début d'une commande ;
- `*` utilisé seul équivaut à `1,$P` ;
- formes composées comme `*P`, `*D` et `*G/<modèle>/P` ;
- aide embarquée `?*` sans modification des documentations de référence ;
- tests de parsing et d'exécution de l'alias.
