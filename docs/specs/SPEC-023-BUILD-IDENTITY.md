# SPEC-023 — Identité de compilation et aide spéciale

## Objectif

Associer chaque exécutable FREDPP à la version du logiciel et à l'état Git des sources utilisés pour le construire.

## Source de la version

La version normative est la valeur `VERSION` de la déclaration CMake :

```cmake
project(FREDPP VERSION X.Y.Z LANGUAGES CXX)
```

Aucun autre fichier utilisateur ne doit contenir une copie normative de cette valeur.

## Métadonnées Git

Lors de la configuration, CMake tente de récupérer :

- le hash court de `HEAD` ;
- l'état du répertoire de travail.

Valeurs de l'état :

- `propre` : aucune modification détectée ;
- `modifié` : fichiers modifiés, ajoutés ou non suivis détectés ;
- `inconnu` : Git ou les métadonnées du dépôt sont indisponibles.

Le hash et l'état décrivent le moment de la configuration CMake. Un nouveau commit nécessite donc une nouvelle configuration ou l'utilisation du script de reconstruction complète.

## Interface utilisateur

`?version` et `--version` affichent la même identité de compilation.

`?:` et `:help` affichent la documentation séparée des commandes spéciales de FREDPP.

## Contraintes documentaires

Les fichiers de référence sous `docs/fr/reference/commandes/` ne sont pas modifiés. La documentation propre à FREDPP est stockée sous `docs/fr/fredpp/`.
