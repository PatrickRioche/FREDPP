# FREDPP — Sprint 2.8

## Première validation multiplateforme

Ce livrable acte la validation de FREDPP v0.0.3 sur les environnements suivants :

- Windows 11 avec MSVC 19.51 ;
- Debian 13 avec GCC 14.2.0 ;
- Debian 13 avec Clang 19.1.7.

La suite complète de 19 tests a réussi dans les trois configurations :

- 19/19 sous MSVC ;
- 19/19 sous GCC ;
- 19/19 sous Clang.

Total observé : **57 tests réussis, aucun échec**.

## Contenu du livrable

```text
LIVRABLE_SPRINT_2.8.md
docs/project/PORTABILITY.md
docs/project/BUILD_DEBIAN.md
scripts/rebuild.sh
.github/workflows/build-and-test.yml
fragments/README_PORTABILITY.md
fragments/CHANGELOG_v0.0.3_PORTABILITY.md
```

## Intégration

Décompresser le contenu à la racine du dépôt FREDPP.

Sous Linux, rendre le script exécutable :

```bash
chmod +x scripts/rebuild.sh
```

Puis lancer :

```bash
./scripts/rebuild.sh gcc
./scripts/rebuild.sh clang
```

Le workflow GitHub Actions est ajouté comme première base d'intégration continue.
