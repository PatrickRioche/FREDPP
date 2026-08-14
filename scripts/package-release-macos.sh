#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${1:-$PROJECT_ROOT/out/build/release-macos}"
OUTPUT_DIR="${2:-$PROJECT_ROOT/out/release}"

if [[ "$BUILD_DIR" != /* ]]; then BUILD_DIR="$PROJECT_ROOT/$BUILD_DIR"; fi
if [[ "$OUTPUT_DIR" != /* ]]; then OUTPUT_DIR="$PROJECT_ROOT/$OUTPUT_DIR"; fi

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

MACHINE="$(uname -m)"
case "$MACHINE" in
    x86_64) PACKAGE_ARCH="x64"; EXPECTED_FILE_ARCH="x86_64" ;;
    arm64)  PACKAGE_ARCH="arm64"; EXPECTED_FILE_ARCH="arm64" ;;
    *) echo "error: unsupported macOS architecture: $MACHINE" >&2; exit 1 ;;
esac

if ! file "$EXECUTABLE" | grep -Fq "$EXPECTED_FILE_ARCH"; then
    echo "error: executable architecture does not match $MACHINE" >&2
    file "$EXECUTABLE" >&2
    exit 1
fi

PACKAGE_NAME="FREDPP-v${VERSION}-macos-${PACKAGE_ARCH}"
STAGE_DIR="$OUTPUT_DIR/$PACKAGE_NAME"
ARCHIVE="$OUTPUT_DIR/${PACKAGE_NAME}.tar.gz"
rm -rf "$STAGE_DIR" "$ARCHIVE"
mkdir -p "$STAGE_DIR" "$OUTPUT_DIR"
install -m 0755 "$EXECUTABLE" "$STAGE_DIR/fredpp"
install -m 0644 "$PROJECT_ROOT/packaging/LISEZMOI-MACOS.txt" "$STAGE_DIR/LISEZMOI.txt"
for file in LICENSE NOTICE CHANGELOG.md ROADMAP.md RELEASE_NOTES.md; do
    install -m 0644 "$PROJECT_ROOT/$file" "$STAGE_DIR/$file"
done
tar -C "$OUTPUT_DIR" -czf "$ARCHIVE" "$PACKAGE_NAME"
echo "Created: $ARCHIVE"
file "$STAGE_DIR/fredpp"
rm -rf "$STAGE_DIR"
