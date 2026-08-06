# FREDPP — Sprint 2.14

## Kits de livraison et Releases GitHub

### Objectif

Fournir une chaîne reproductible de construction, de test et de publication de FREDPP pour Windows x64 et Debian 13 amd64.

### Version

La version exacte se consulte dans FREDPP avec `?version`.

### Artefacts produits

```text
FREDPP-vX.Y.Z-windows-x64.zip
FREDPP-vX.Y.Z-debian13-amd64.tar.gz
fredpp_X.Y.Z_amd64.deb
SHA256SUMS.txt
```

### Fonctions ajoutées

- configuration d'installation CMake ;
- paquet Debian généré avec CPack ;
- script de kit portable Windows ;
- script d'archive portable Debian ;
- workflow GitHub Actions déclenché par un tag `v*` ;
- contrôle automatique de cohérence entre tag et version ;
- calcul et publication des empreintes SHA-256 ;
- scripts PowerShell et Bash de suivi des téléchargements ;
- documentation française de publication ;
- test CTest du manifeste de release.

### Validation locale Linux

```bash
cmake -S . -B out/build/release-gcc -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DFREDPP_BUILD_TESTS=ON
cmake --build out/build/release-gcc --parallel
ctest --test-dir out/build/release-gcc --output-on-failure
./scripts/package-release.sh out/build/release-gcc out/release
```

### Publication

```bash
git tag -a vX.Y.Z -m "FREDPP vX.Y.Z"
git push origin vX.Y.Z
```

Le tag déclenche la construction et la création de la Release GitHub.
