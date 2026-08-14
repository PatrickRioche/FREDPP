# FREDPP v0.0.17 — Flot de procédures et packaging macOS

Cette release poursuit le rapprochement de FREDPP avec le comportement
historique FRED/UWTOOLS et ajoute pour la première fois des kits natifs macOS.

## Procédures et flot historique

Le moteur de procédures prend désormais en charge notamment :

- la visibilité correcte des buffers système du bootstrap dans `FB` ;
- la commande historique `M(buffer)` pour déplacer des lignes vers un buffer ;
- l'expansion `\S(buffer)` dans les commandes système `!` et `ZG` ;
- le retour en mode interactif après une erreur de procédure, avec conservation
  de l'état pour le diagnostic ;
- l'expansion `\S(buffer)` dans les modèles de commandes globales `G` ;
- les labels `@(label)` et les sauts `J(label)`, `J(label)T`, `J(label)F` ;
- un premier sous-ensemble des registres numériques historiques `N(...)` ;
- l'enchaînement de plusieurs commandes FRED sur une même ligne ;
- les délimiteurs historiques arbitraires de `JM` et `JP`.

## macOS

Deux kits portables sont produits et testés nativement par GitHub Actions :

- `FREDPP-v0.0.17-macos-x64.tar.gz` pour les Mac Intel ;
- `FREDPP-v0.0.17-macos-arm64.tar.gz` pour les Mac Apple Silicon.

Chaque build macOS est compilé, testé puis contrôlé sur une machine de
l'architecture correspondante avant publication.

Les exécutables macOS ne sont pas encore signés ni notarisés par Apple.

## Autres plateformes

La release continue de publier Windows x64, Debian 13 amd64, Debian 13 arm64
et l'extension Visual Studio Code FREDPP.

## Validation

```text
49/49 tests réussis sous Windows avant préparation de la release
```

Les jobs macOS exécutent ensuite la suite complète sur leurs runners natifs.

## Documentation

Les fichiers historiques sous `docs/fr/reference/commandes` restent inchangés.
La version effectivement exécutée se consulte avec `?version`.
