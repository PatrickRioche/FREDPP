# État du projet

## Sprint courant

Sprint 2.12 — commandes historiques `S` et `Q`.

## Version logicielle

La version exacte n'est pas recopiée dans ce document. Elle se consulte dans l'exécutable avec :

```text
?version
```

## Validation attendue

- Windows / MSVC : 25/25 tests ;
- Debian / GCC : 25/25 tests ;
- Debian / Clang : 25/25 tests.

## Fonctions ajoutées

- substitutions sur une ligne ou une plage avec `S` ;
- `&` pour réinsérer le texte trouvé ;
- séparateurs symboliques et suffixe optionnel `P` ;
- utilisation de `S` dans `G` ;
- sortie avec `Q` ou `QQ` ;
- suppression de `:quit` ;
- aides embarquées `?s` et `?q` ;
- tests dédiés à S et Q.
