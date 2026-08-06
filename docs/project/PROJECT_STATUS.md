# État du projet

## Sprint courant

Sprint 2.13 — lecture et écriture de fichiers avec `R`, `W`, `WA` et `WU`.

## Version logicielle

La version exacte n'est pas recopiée dans ce document. Elle se consulte dans l'exécutable avec :

```text
?version
```

## Validation attendue

- Windows / MSVC : 26/26 tests ;
- Debian / GCC : 26/26 tests ;
- Debian / Clang : 26/26 tests.

## Fonctions ajoutées

- lecture de fichiers ASCII ou UTF-8 avec `R` ;
- détection et suppression du BOM UTF-8 ;
- écriture avec `W`, `WA` et l'extension `WU` ;
- refus explicite de `WB` pour le format GCOS/BCD ;
- conservation de LF ou CRLF ;
- suivi des fichiers associés et des buffers modifiés ;
- protection de la sortie normale `Q` ;
- aides embarquées `?r`, `?w` et `?wu` ;
- test dédié aux entrées-sorties de fichiers.
