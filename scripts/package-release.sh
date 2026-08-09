#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${1:-$PROJECT_ROOT/out/build/release-gcc}"
OUTPUT_DIR="${2:-$PROJECT_ROOT/out/release}"

if [[ "$BUILD_DIR" != /* ]]; then
    BUILD_DIR="$PROJECT_ROOT/$BUILD_DIR"
fi
if [[ "$OUTPUT_DIR" != /* ]]; then
    OUTPUT_DIR="$PROJECT_ROOT/$OUTPUT_DIR"
fi

VERSION="$(sed -nE 's/^[[:space:]]*project\(FREDPP VERSION ([0-9]+\.[0-9]+\.[0-9]+).*/\1/p' "$PROJECT_ROOT/CMakeLists.txt")"
if [[ -z "$VERSION" ]]; then
    echo "error: unable to read FREDPP version from CMakeLists.txt" >&2
    exit 1
fi

EXECUTABLE="$BUILD_DIR/fredpp"
if [[ ! -x "$EXECUTABLE" ]]; then
    echo "error: Release executable not found: $EXECUTABLE" >&2
    exit 1
fi

if ! "$EXECUTABLE" --version | grep -Fq "FREDPP v$VERSION"; then
    echo "error: executable version does not match v$VERSION" >&2
    exit 1
fi

ARCHITECTURE="$(dpkg --print-architecture)"
case "$ARCHITECTURE" in
    amd64|arm64)
        ;;
    *)
        echo "error: unsupported Debian release architecture: $ARCHITECTURE" >&2
        exit 1
        ;;
esac

PACKAGE_NAME="FREDPP-v${VERSION}-debian13-${ARCHITECTURE}"
STAGE_DIR="$OUTPUT_DIR/$PACKAGE_NAME"
ARCHIVE="$OUTPUT_DIR/${PACKAGE_NAME}.tar.gz"
DEB_EXPECTED="$OUTPUT_DIR/fredpp_${VERSION}_${ARCHITECTURE}.deb"

rm -rf "$STAGE_DIR" "$ARCHIVE" "$DEB_EXPECTED"
mkdir -p "$STAGE_DIR" "$OUTPUT_DIR"

install -m 0755 "$EXECUTABLE" "$STAGE_DIR/fredpp"
install -m 0644 "$PROJECT_ROOT/packaging/LISEZMOI-DEBIAN.txt" "$STAGE_DIR/LISEZMOI.txt"
for file in LICENSE NOTICE CHANGELOG.md ROADMAP.md RELEASE_NOTES.md; do
    install -m 0644 "$PROJECT_ROOT/$file" "$STAGE_DIR/$file"
done

tar -C "$OUTPUT_DIR" -czf "$ARCHIVE" "$PACKAGE_NAME"

cpack --config "$BUILD_DIR/CPackConfig.cmake" -G DEB -B "$OUTPUT_DIR"
if [[ ! -f "$DEB_EXPECTED" ]]; then
    echo "error: expected Debian package was not generated: $DEB_EXPECTED" >&2
    exit 1
fi

DEB_ARCHITECTURE="$(dpkg-deb --field "$DEB_EXPECTED" Architecture)"
if [[ "$DEB_ARCHITECTURE" != "$ARCHITECTURE" ]]; then
    echo "error: generated Debian architecture $DEB_ARCHITECTURE does not match $ARCHITECTURE" >&2
    exit 1
fi

echo "Created: $ARCHIVE"
rm -rf "$OUTPUT_DIR/_CPack_Packages"

echo "Created: $DEB_EXPECTED"
