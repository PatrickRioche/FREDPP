# SPEC-027 — Kits de livraison et Releases GitHub

## Périmètre

Cette spécification définit la première chaîne officielle de livraison binaire de FREDPP.

## Artefacts obligatoires

- ZIP portable Windows x64 ;
- archive portable Debian 13 amd64 ;
- paquet Debian amd64 ;
- manifeste SHA-256.

## Règles de traçabilité

- le tag doit être `vX.Y.Z` ;
- `X.Y.Z` doit correspondre à `project(FREDPP VERSION X.Y.Z)` ;
- les builds de release doivent provenir d'un dépôt propre ;
- chaque binaire conserve le commit Git dans `?version` ;
- les tests doivent réussir avant toute publication.

## Nommage

```text
FREDPP-vX.Y.Z-windows-x64.zip
FREDPP-vX.Y.Z-debian13-amd64.tar.gz
fredpp_X.Y.Z_amd64.deb
SHA256SUMS.txt
```

## Plateformes

- Windows x64 avec MSVC en configuration Release et runtime statique ;
- Debian 13 amd64 avec GCC en configuration Release ;
- GCC et Clang restent validés par le workflow général de compilation.

## Publication

La publication est déclenchée exclusivement par l'envoi d'un tag `v*`. Le workflow bloque la création de la Release si le tag et la version CMake divergent.

## Statistiques

Les scripts de statistiques interrogent l'API GitHub Releases et lisent le champ `download_count` de chaque ressource publiée.
