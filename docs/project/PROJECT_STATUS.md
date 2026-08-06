# État du projet

## Sprint courant

Sprint 2.14 — kits de livraison Windows et Debian et publication GitHub Releases.

## Version logicielle

La version exacte n'est pas recopiée dans ce document. Elle se consulte dans l'exécutable avec :

```text
?version
```

## Validation attendue

- Windows / MSVC Release : 27/27 tests et ZIP portable ;
- Debian 13 / GCC Release : 27/27 tests, TAR.GZ et paquet DEB ;
- Debian / Clang : 27/27 tests dans la validation générale ;
- correspondance entre le tag `vX.Y.Z` et la version CMake ;
- publication de `SHA256SUMS.txt`.

## Fonctions ajoutées

- installation CMake et paquet Debian CPack ;
- scripts locaux de création des kits Windows et Debian ;
- workflow automatisé de publication ;
- contrôle de version et de propreté des sources ;
- statistiques de téléchargement des ressources ;
- documentation de release et test du manifeste.
