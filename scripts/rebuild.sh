#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
MODE="${1:-gcc}"

case "$MODE" in
  gcc)
    BUILD_DIR="$PROJECT_ROOT/out/build/linux-debug"
    CXX_COMPILER="g++"
    ;;
  clang)
    BUILD_DIR="$PROJECT_ROOT/out/build/linux-clang"
    CXX_COMPILER="clang++"
    ;;
  *)
    echo "Usage: $0 [gcc|clang]" >&2
    exit 2
    ;;
esac

echo "FREDPP Linux rebuild"
echo "Compiler : $CXX_COMPILER"
echo "Build dir: $BUILD_DIR"

rm -rf "$BUILD_DIR"

cmake \
  -S "$PROJECT_ROOT" \
  -B "$BUILD_DIR" \
  -DCMAKE_CXX_COMPILER="$CXX_COMPILER" \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build "$BUILD_DIR" -j"$(nproc)"
ctest --test-dir "$BUILD_DIR" --output-on-failure

echo
echo "FREDPP rebuild and tests completed successfully with $CXX_COMPILER."
