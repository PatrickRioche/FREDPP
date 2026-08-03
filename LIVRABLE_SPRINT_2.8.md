# FREDPP — Sprint 2.8

## Première validation multiplateforme

**Version validée :** v0.0.3  
**Date :** 2026-08-03  
**Statut :** validé

## Objectif

Démontrer que FREDPP est un projet C++20 réellement multiplateforme, compilable et testable à partir d'un clone propre du dépôt GitHub, sans modification du code source.

## Environnements validés

| Système | Compilateur | Version | Résultat |
|---|---|---:|:---:|
| Windows 11 | MSVC | 19.51 | ✅ |
| Debian 13 | GCC | 14.2.0 | ✅ |
| Debian 13 | Clang | 19.1.7 | ✅ |

### Environnement Linux

- Hyperviseur : Proxmox VE ;
- Machine virtuelle : Debian 13 ;
- Accès : SSH avec PuTTY ;
- Git : 2.47.3 ;
- CMake : 3.31.6.

## Procédure de validation

### Clone

```bash
git clone https://github.com/patrickrioche/FREDPP.git
cd FREDPP
```

### GCC

```bash
cmake -S . -B out/build/linux-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build out/build/linux-debug -j"$(nproc)"
ctest --test-dir out/build/linux-debug --output-on-failure
```

Résultat :

```text
100% tests passed, 0 tests failed out of 19
```

### Clang

```bash
cmake -S . \
  -B out/build/linux-clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build out/build/linux-clang -j"$(nproc)"
ctest --test-dir out/build/linux-clang --output-on-failure
```

Résultat :

```text
100% tests passed, 0 tests failed out of 19
```

### Windows

```powershell
cd .\scripts\
.\rebuild.bat
```

Résultat :

```text
100% tests passed out of 19
```

## Résultat consolidé

| Configuration | Tests réussis |
|---|---:|
| Windows / MSVC | 19/19 |
| Debian / GCC | 19/19 |
| Debian / Clang | 19/19 |
| **Total observé** | **57/57** |

Aucune modification du code source n'a été nécessaire entre les plateformes.

## Validation fonctionnelle Linux

L'exécutable produit sous Debian a été lancé avec succès.

Les éléments suivants ont été vérifiés :

- démarrage du REPL ;
- aide Markdown intégrée ;
- buffers ;
- commandes d'édition déjà disponibles ;
- `:buffers` ;
- `:print` ;
- moteur Flow avec `:flow <buffer>`.

## Observation Clang

Clang a signalé un avertissement dans `src/cli/main.cpp` :

```text
17 enumeration values not handled in switch
```

Cet avertissement n'empêche ni la compilation ni l'exécution des tests. Il doit être traité lors d'un prochain sprint qualité afin de rendre le `switch` explicitement exhaustif ou de documenter son comportement par défaut.

## Décision actée

FREDPP n'est pas développé pour Windows puis porté vers Linux.

FREDPP est développé comme un projet C++20 portable :

- Windows reste l'environnement principal de développement ;
- Debian devient la plateforme officielle de validation Linux ;
- MSVC, GCC et Clang constituent les compilateurs de référence.

## Politique de validation

Une release FREDPP ne doit pas être déclarée validée tant que les contrôles suivants ne sont pas satisfaits :

- compilation et tests sous Windows/MSVC ;
- compilation et tests sous Debian/GCC ;
- compilation et tests sous Debian/Clang.

## Conclusion

Ce sprint démontre que l'architecture de FREDPP est portable, reproductible et indépendante d'un compilateur particulier.

À partir d'un simple `git clone`, le même dépôt a été configuré, compilé et testé avec succès sur Windows et Debian, avec les trois principaux compilateurs C++ du projet.
