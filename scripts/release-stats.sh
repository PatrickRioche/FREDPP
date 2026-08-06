#!/usr/bin/env bash
set -euo pipefail

REPOSITORY="${1:-PatrickRioche/FREDPP}"

for command in curl jq; do
    if ! command -v "$command" >/dev/null 2>&1; then
        echo "error: $command is required" >&2
        exit 1
    fi
done

API_URL="https://api.github.com/repos/${REPOSITORY}/releases?per_page=100"
DATA="$(curl -fsSL \
    -H 'Accept: application/vnd.github+json' \
    -H 'X-GitHub-Api-Version: 2022-11-28' \
    "$API_URL")"

printf '%-12s  %-48s  %s\n' "VERSION" "FICHIER" "TÉLÉCHARGEMENTS"
printf '%-12s  %-48s  %s\n' "------------" "------------------------------------------------" "---------------"
printf '%s' "$DATA" | jq -r '.[] | .tag_name as $tag | .assets[] | [$tag, .name, (.download_count|tostring)] | @tsv' |
while IFS=$'\t' read -r version file count; do
    printf '%-12s  %-48s  %s\n' "$version" "$file" "$count"
done

echo
printf '%s' "$DATA" | jq -r '
  [.[].assets[]] as $assets |
  "Windows : \([ $assets[] | select(.name | test("windows-x64\\.zip$"; "i")) | .download_count ] | add // 0)",
  "Debian  : \([ $assets[] | select(.name | test("debian13-amd64\\.tar\\.gz$|_amd64\\.deb$"; "i")) | .download_count ] | add // 0)",
  "Total   : \([ $assets[].download_count ] | add // 0)"
'
