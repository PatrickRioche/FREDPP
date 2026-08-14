# Sprint 2.23 — Release v0.0.17 et packaging macOS

## Objectif
Préparer FREDPP v0.0.17 et étendre la chaîne de publication à macOS Intel
x86_64 et Apple Silicon arm64.

## Livrables
- `scripts/package-release-macos.sh` ;
- `packaging/LISEZMOI-MACOS.txt` ;
- jobs GitHub Actions macOS Intel et Apple Silicon ;
- archives `FREDPP-v0.0.17-macos-x64.tar.gz` et
  `FREDPP-v0.0.17-macos-arm64.tar.gz` ;
- intégration au manifeste SHA-256 et à la GitHub Release ;
- passage de la version CMake à 0.0.17.

## Validation
La publication attend la validation du tag, Windows x64, Debian amd64,
Debian arm64, macOS x64, macOS arm64 et VS Code. Les jobs macOS exécutent la
suite CTest complète et vérifient l'architecture Mach-O avant packaging.

## Sécurité
La première distribution macOS reste non signée et non notarisée.

## Documentation historique
Aucune modification n'est apportée aux fichiers sous
`docs/fr/reference/commandes`.
