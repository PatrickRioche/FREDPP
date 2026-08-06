# FREDPP — Livrable Sprint 2.13

## Objectif

Rendre FREDPP utilisable comme éditeur de fichiers texte grâce aux commandes historiques `R` et `W`, à l'écriture ASCII `WA` et à l'extension moderne UTF-8 `WU`.

## Fonctions livrées

- `R fichier` : lecture d'un fichier ASCII ou UTF-8 ;
- `W [fichier]` : écriture avec conservation de l'encodage connu ;
- `WA [fichier]` : écriture ASCII stricte ;
- `WU [fichier]` : écriture UTF-8 sans BOM ;
- `WB [fichier]` : forme reconnue, mais refus explicite du GCOS/BCD ;
- chemins contenant des espaces, avec ou sans guillemets ;
- écritures complètes ou par plages ;
- conservation de LF ou CRLF ;
- suivi du fichier associé, de l'encodage et de l'état modifié ;
- refus de `Q` en présence de modifications non enregistrées ;
- sortie forcée avec `QQ` ;
- aides `?r`, `?w` et `?wu`.

## Validation automatisée

La suite contient désormais 26 tests. Le nouveau test `test_file_io` couvre notamment :

- lecture ASCII avec CRLF ;
- lecture UTF-8 avec BOM ;
- écriture UTF-8 sans BOM ;
- refus d'une écriture ASCII impossible ;
- écriture partielle conservant l'état modifié ;
- refus explicite de `WB` ;
- erreur de `W` sans fichier associé.

## Validation effectuée avant livraison

- GCC : 26/26 tests ;
- Clang : 26/26 tests, aucun avertissement.

La validation MSVC doit être réalisée sur le poste Windows de référence.

## Limites reportées

- `WB` GCOS/BCD ;
- `WX` ;
- listes avancées de fichiers avec `R` ;
- encodages autres que l'ASCII et l'UTF-8.
