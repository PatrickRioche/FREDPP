# Compiler et tester FREDPP sous Debian

## Prérequis

```bash
apt update
apt install -y \
  git \
  cmake \
  build-essential \
  gcc \
  g++ \
  clang \
  ninja-build \
  gdb
```

## Cloner le dépôt

```bash
cd /home/debian
git clone https://github.com/patrickrioche/FREDPP.git
cd FREDPP
```

## Compilation avec GCC

```bash
cmake -S . -B out/build/linux-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build out/build/linux-debug -j"$(nproc)"
ctest --test-dir out/build/linux-debug --output-on-failure
```

Exécuter FREDPP :

```bash
./out/build/linux-debug/fredpp
```

## Compilation avec Clang

```bash
cmake -S . \
  -B out/build/linux-clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build out/build/linux-clang -j"$(nproc)"
ctest --test-dir out/build/linux-clang --output-on-failure
```

Exécuter FREDPP :

```bash
./out/build/linux-clang/fredpp
```

## Script automatisé

```bash
chmod +x scripts/rebuild.sh
./scripts/rebuild.sh gcc
./scripts/rebuild.sh clang
```

## Nettoyer les builds Linux

```bash
rm -rf out/build/linux-debug
rm -rf out/build/linux-clang
```
