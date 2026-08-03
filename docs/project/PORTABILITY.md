# Portabilité de FREDPP

## Statut actuel

FREDPP v0.0.3 a été compilé et testé avec succès sur :

| Plateforme | Compilateur | Version | Tests |
|---|---|---:|:---:|
| Windows 11 | MSVC | 19.51 | 19/19 |
| Debian 13 | GCC | 14.2.0 | 19/19 |
| Debian 13 | Clang | 19.1.7 | 19/19 |

## Principe

Le cœur `fredpp_core` doit rester indépendant du système d'exploitation.

La portabilité repose sur :

- C++20 ;
- la bibliothèque standard C++ ;
- CMake ;
- CTest.

Les dépendances propres à une plateforme doivent rester confinées aux couches d'interface ou aux scripts de construction.

## Plateformes de référence

### Développement principal

- Windows 11 ;
- Visual Studio ;
- MSVC.

### Validation Linux

- Debian 13 sur machine virtuelle Proxmox VE ;
- accès SSH ;
- GCC et Clang.

## Critère de release

Une release n'est considérée comme validée que lorsque :

1. le projet est configurable ;
2. la compilation réussit ;
3. tous les tests CTest réussissent ;

dans chacune des configurations de référence.

## Avertissements

La compilation Clang 19.1.7 signale actuellement un `switch` non exhaustif dans `src/cli/main.cpp`. Cet avertissement est connu et n'a pas d'incidence fonctionnelle observée.
