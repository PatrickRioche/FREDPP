#!/usr/bin/env python3
from __future__ import annotations

import json
import os
import stat
import subprocess
import sys
from pathlib import Path

ROOT = Path(sys.argv[1] if len(sys.argv) > 1 else '.').resolve()
EXPECTED_BASE = '0689525'

ORIGINAL: dict[str, str | None] = {}
VIRTUAL: dict[str, str] = {}
ERRORS: list[str] = []


def p(rel: str) -> Path:
    return ROOT / rel


def read_text(rel: str) -> str:
    q = p(rel)
    if not q.is_file():
        raise RuntimeError(f'fichier absent : {rel}')
    return q.read_text(encoding='utf-8').replace('\r\n', '\n')


def load(rel: str) -> None:
    text = read_text(rel)
    ORIGINAL[rel] = text
    VIRTUAL[rel] = text


def replace_once(rel: str, old: str, new: str, marker: str) -> None:
    text = VIRTUAL[rel]
    if marker in text:
        print(f'PRECHECK déjà OK : {rel} :: {marker}')
        return
    count = text.count(old)
    if count != 1:
        ERRORS.append(f'{rel}: contexte trouvé {count} fois au lieu de 1 :: marqueur {marker!r}')
        return
    VIRTUAL[rel] = text.replace(old, new, 1)
    print(f'PRECHECK OK      : {rel} :: {marker}')


def insert_before(rel: str, anchor: str, block: str, marker: str) -> None:
    text = VIRTUAL[rel]
    if marker in text:
        print(f'PRECHECK déjà OK : {rel} :: {marker}')
        return
    count = text.count(anchor)
    if count != 1:
        ERRORS.append(f'{rel}: ancre trouvée {count} fois au lieu de 1 :: marqueur {marker!r}')
        return
    VIRTUAL[rel] = text.replace(anchor, block + anchor, 1)
    print(f'PRECHECK OK      : {rel} :: {marker}')


def insert_after(rel: str, anchor: str, block: str, marker: str) -> None:
    text = VIRTUAL[rel]
    if marker in text:
        print(f'PRECHECK déjà OK : {rel} :: {marker}')
        return
    count = text.count(anchor)
    if count != 1:
        ERRORS.append(f'{rel}: ancre trouvée {count} fois au lieu de 1 :: marqueur {marker!r}')
        return
    VIRTUAL[rel] = text.replace(anchor, anchor + block, 1)
    print(f'PRECHECK OK      : {rel} :: {marker}')


def replace_full(rel: str, required_markers: list[str], content: str, marker: str) -> None:
    text = VIRTUAL[rel]
    if marker in text:
        print(f'PRECHECK déjà OK : {rel} :: {marker}')
        return
    missing = [m for m in required_markers if m not in text]
    if missing:
        ERRORS.append(f'{rel}: marqueurs préalables absents : {missing!r}')
        return
    VIRTUAL[rel] = content
    print(f'PRECHECK OK      : {rel} :: remplacement contrôlé complet')


def ensure_new(rel: str, content: str, marker: str) -> None:
    q = p(rel)
    if q.exists():
        current = q.read_text(encoding='utf-8').replace('\r\n', '\n')
        if marker in current:
            ORIGINAL[rel] = current
            VIRTUAL[rel] = current
            print(f'PRECHECK déjà OK : {rel} :: {marker}')
            return
        ERRORS.append(f'{rel}: existe déjà avec un contenu inattendu')
        return
    ORIGINAL[rel] = None
    VIRTUAL[rel] = content
    print(f'PRECHECK OK      : {rel} :: création')


def git(*args: str) -> str:
    result = subprocess.run(['git', *args], cwd=ROOT, text=True,
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if result.returncode != 0:
        raise RuntimeError('git ' + ' '.join(args) + ' a échoué : ' + result.stderr.strip())
    return result.stdout.strip()


# ---------------------------------------------------------------------------
# PRECHECK GLOBAL
# ---------------------------------------------------------------------------

top = Path(git('rev-parse', '--show-toplevel')).resolve()
if top != ROOT:
    raise RuntimeError(f'racine Git inattendue : {top} ; demandé : {ROOT}')

head = git('rev-parse', '--short=7', 'HEAD')
if head != EXPECTED_BASE:
    raise RuntimeError(f'base Git inattendue : {head} ; attendu : {EXPECTED_BASE}')

if subprocess.run(['git', 'diff', '--quiet'], cwd=ROOT).returncode != 0:
    raise RuntimeError('des fichiers suivis sont déjà modifiés')
if subprocess.run(['git', 'diff', '--cached', '--quiet'], cwd=ROOT).returncode != 0:
    raise RuntimeError('des changements sont déjà indexés')

required = [
    'CMakeLists.txt',
    '.gitignore',
    'scripts/package-release.sh',
    'packaging/LISEZMOI-DEBIAN.txt',
    'packaging/LISEZMOI-WINDOWS.txt',
    '.github/workflows/release.yml',
    'tests/test_release_manifest.cmake',
    'CHANGELOG.md',
    'RELEASE_NOTES.md',
    'ROADMAP.md',
]
for rel in required:
    load(rel)

for rel, marker in [
    ('CMakeLists.txt', 'project(FREDPP VERSION 0.0.14 LANGUAGES CXX)'),
    ('.github/workflows/release.yml', 'workflow_dispatch:'),
    ('.github/workflows/release.yml', 'Windows x64 portable ZIP'),
    ('.github/workflows/release.yml', 'Debian 13 amd64 packages'),
    ('.github/workflows/release.yml', 'Create SHA-256 manifest'),
    ('scripts/package-release.sh', 'debian13-amd64'),
    ('tests/test_release_manifest.cmake', 'scripts/package-release.sh'),
]:
    if marker not in VIRTUAL[rel]:
        ERRORS.append(f'{rel}: état de base inattendu, marqueur absent {marker!r}')

# ---------------------------------------------------------------------------
# CMAKE / GITIGNORE / DEBIAN PACKAGING
# ---------------------------------------------------------------------------

replace_once(
    'CMakeLists.txt',
    'project(FREDPP VERSION 0.0.14 LANGUAGES CXX)\n',
    'project(FREDPP VERSION 0.0.15 LANGUAGES CXX)\n',
    'project(FREDPP VERSION 0.0.15'
)

replace_once(
    'CMakeLists.txt',
    'set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)\nset(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")\ninclude(CPack)\n',
    'set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)\n'
    '# Let CPack derive the native Debian architecture through\n'
    '# dpkg --print-architecture. This keeps the same manifest valid for\n'
    '# both amd64 and arm64 release runners.\n'
    'include(CPack)\n',
    'dpkg --print-architecture'
)

replace_once(
    '.gitignore',
    '*.deb\n*.tar.gz\nSHA256SUMS.txt\n',
    '*.deb\n*.tar.gz\n*.vsix\nSHA256SUMS.txt\n',
    '*.vsix'
)

replace_once(
    'scripts/package-release.sh',
    'PACKAGE_NAME="FREDPP-v${VERSION}-debian13-amd64"\n'
    'STAGE_DIR="$OUTPUT_DIR/$PACKAGE_NAME"\n'
    'ARCHIVE="$OUTPUT_DIR/${PACKAGE_NAME}.tar.gz"\n'
    'DEB_EXPECTED="$OUTPUT_DIR/fredpp_${VERSION}_amd64.deb"\n',
    'ARCHITECTURE="$(dpkg --print-architecture)"\n'
    'case "$ARCHITECTURE" in\n'
    '    amd64|arm64)\n'
    '        ;;\n'
    '    *)\n'
    '        echo "error: unsupported Debian release architecture: $ARCHITECTURE" >&2\n'
    '        exit 1\n'
    '        ;;\n'
    'esac\n\n'
    'PACKAGE_NAME="FREDPP-v${VERSION}-debian13-${ARCHITECTURE}"\n'
    'STAGE_DIR="$OUTPUT_DIR/$PACKAGE_NAME"\n'
    'ARCHIVE="$OUTPUT_DIR/${PACKAGE_NAME}.tar.gz"\n'
    'DEB_EXPECTED="$OUTPUT_DIR/fredpp_${VERSION}_${ARCHITECTURE}.deb"\n',
    'ARCHITECTURE="$(dpkg --print-architecture)"'
)

replace_once(
    'scripts/package-release.sh',
    'if [[ ! -f "$DEB_EXPECTED" ]]; then\n'
    '    echo "error: expected Debian package was not generated: $DEB_EXPECTED" >&2\n'
    '    exit 1\n'
    'fi\n\n'
    'echo "Created: $ARCHIVE"\n',
    'if [[ ! -f "$DEB_EXPECTED" ]]; then\n'
    '    echo "error: expected Debian package was not generated: $DEB_EXPECTED" >&2\n'
    '    exit 1\n'
    'fi\n\n'
    'DEB_ARCHITECTURE="$(dpkg-deb --field "$DEB_EXPECTED" Architecture)"\n'
    'if [[ "$DEB_ARCHITECTURE" != "$ARCHITECTURE" ]]; then\n'
    '    echo "error: generated Debian architecture $DEB_ARCHITECTURE does not match $ARCHITECTURE" >&2\n'
    '    exit 1\n'
    'fi\n\n'
    'echo "Created: $ARCHIVE"\n',
    'DEB_ARCHITECTURE="$(dpkg-deb --field'
)

# ---------------------------------------------------------------------------
# PACKAGING DOCUMENTATION
# ---------------------------------------------------------------------------

debian_readme = '''## Archive portable

Les kits Debian 13 sont publiés pour `amd64` et `arm64`.

Remplacez `<arch>` par l'architecture de votre système (`amd64` ou `arm64`) :

```text
tar -xzf FREDPP-vX.Y.Z-debian13-<arch>.tar.gz
cd FREDPP-vX.Y.Z-debian13-<arch>
./fredpp
```

## Paquet Debian

```text
sudo apt install ./fredpp_X.Y.Z_<arch>.deb
fredpp
```

Pour connaître l'architecture Debian de la machine :

```text
dpkg --print-architecture
```

Le paquet `arm64` est destiné aux systèmes Debian 13 ARM64 et aux systèmes
ARM64 compatibles, notamment les Raspberry Pi utilisant un système 64 bits
compatible.

Dans FREDPP, utilisez `?` pour l'aide, `?version` pour identifier le binaire et
`Q` pour quitter.

Projet : https://github.com/PatrickRioche/FREDPP
Licence : voir le fichier LICENSE inclus dans ce kit.
'''
replace_full(
    'packaging/LISEZMOI-DEBIAN.txt',
    ['FREDPP-vX.Y.Z-debian13-amd64.tar.gz', 'fredpp_X.Y.Z_amd64.deb',
     'Projet : https://github.com/PatrickRioche/FREDPP'],
    debian_readme,
    'Les kits Debian 13 sont publiés pour `amd64` et `arm64`.'
)

insert_before(
    'packaging/LISEZMOI-WINDOWS.txt',
    'Projet : https://github.com/PatrickRioche/FREDPP\n',
    '''## Windows 11 - exécutable non signé

`fredpp.exe` n'est pas encore signé numériquement. Selon la configuration de
Windows 11, Microsoft Defender SmartScreen ou Smart App Control peut donc
afficher un avertissement ou bloquer son exécution.

Avant d'exécuter FREDPP :

1. téléchargez le kit uniquement depuis les Releases GitHub officielles du
   projet FREDPP ;
2. vérifiez, lorsque cela est possible, l'empreinte SHA-256 du kit avec le
   fichier `SHA256SUMS.txt` fourni dans la même Release ;
3. si Windows bloque l'application, consultez **Sécurité Windows** puis
   **Contrôle des applications et du navigateur** pour identifier la protection
   ayant déclenché le blocage.

Ne désactivez pas globalement les protections de Windows uniquement pour
exécuter FREDPP.

''',
    '## Windows 11 - exécutable non signé'
)

# ---------------------------------------------------------------------------
# VS CODE EXTENSION
# ---------------------------------------------------------------------------

package_json = {
    'name': 'fredpp-language',
    'displayName': 'FREDPP Language Support',
    'description': 'Visual Studio Code language support for FREDPP procedure files.',
    'version': '0.1.0',
    'publisher': 'fredpp',
    'engines': {'vscode': '^1.90.0'},
    'categories': ['Programming Languages'],
    'keywords': ['FREDPP', 'FRED', 'text editor', 'procedure'],
    'repository': {'type': 'git', 'url': 'https://github.com/PatrickRioche/FREDPP.git'},
    'contributes': {
        'languages': [{
            'id': 'fredpp',
            'aliases': ['FREDPP', 'fredpp'],
            'extensions': ['.fredpp'],
            'configuration': './language-configuration.json',
        }],
        'grammars': [{
            'language': 'fredpp',
            'scopeName': 'source.fredpp',
            'path': './syntaxes/fredpp.tmLanguage.json',
        }],
        'snippets': [{
            'language': 'fredpp',
            'path': './snippets/fredpp.json',
        }],
    },
}

language_config = {
    'comments': {'lineComment': '"'},
    'brackets': [['(', ')']],
    'autoClosingPairs': [{'open': '(', 'close': ')'}],
    'surroundingPairs': [['(', ')']],
}

grammar = {
    '$schema': 'https://raw.githubusercontent.com/martinring/tmlanguage/master/tmlanguage.json',
    'name': 'FREDPP',
    'scopeName': 'source.fredpp',
    'patterns': [
        {'include': '#comments'}, {'include': '#system'}, {'include': '#flow'},
        {'include': '#patterns'}, {'include': '#addresses'}, {'include': '#options'},
        {'include': '#commands'}, {'include': '#star'}, {'include': '#numbers'},
    ],
    'repository': {
        'comments': {'patterns': [{
            'name': 'comment.line.double-quote.fredpp',
            'match': '(?m)^\\s*".*$',
        }]},
        'system': {'patterns': [{
            'begin': '(?m)^\\s*(!)',
            'beginCaptures': {'1': {'name': 'keyword.control.command.fredpp'}},
            'end': '$',
            'patterns': [{'name': 'string.unquoted.shell.fredpp', 'match': '.+'}],
        }]},
        'flow': {'patterns': [{
            'name': 'keyword.control.flow.fredpp',
            'match': '\\\\(?:B|L|S)\\([^)]*\\)|\\\\F',
        }]},
        'patterns': {'patterns': [{
            'name': 'string.regexp.fredpp',
            'match': '/(?:\\\\.|[^/])*/',
        }]},
        'addresses': {'patterns': [{
            'name': 'variable.language.address.fredpp',
            'match': '(?i)(?<![A-Za-z0-9_])(?:\\.|\\$|[0-9]+)(?:\\s*,\\s*(?:\\.|\\$|[0-9]+))?(?![A-Za-z0-9_])',
        }]},
        'options': {'patterns': [{
            'name': 'keyword.control.option.fredpp',
            'match': '(?i)(?<![A-Za-z])O[+-](?:M|I\\()(?![A-Za-z])',
        }]},
        'commands': {'patterns': [{
            'name': 'keyword.control.command.fredpp',
            'match': '(?i)(?<![A-Za-z])(?:FB|FO|JM|JP|ZG|QQ|WA|WU|A|B|C|D|G|I|L|M|P|Q|R|S|T|W|Z)(?![A-Za-z])',
        }]},
        'star': {'patterns': [{
            'name': 'keyword.control.command.fredpp',
            'match': '(?m)^\\s*\\*\\s*$',
        }]},
        'numbers': {'patterns': [{
            'name': 'constant.numeric.fredpp',
            'match': '(?<![A-Za-z0-9_])[0-9]+(?![A-Za-z0-9_])',
        }]},
    },
}

snippets = {
    'Print line': {'prefix': 'p', 'body': ['${1:1}P'], 'description': 'FREDPP print command'},
    'List file': {'prefix': 'l', 'body': ['L ${1:fichier.txt}'], 'description': 'FREDPP list command'},
    'Delete line': {'prefix': 'd', 'body': ['${1:1}D'], 'description': 'FREDPP delete command'},
    'Append block': {'prefix': 'a', 'body': ['${1:$}A', '${2:texte}', '\\F'], 'description': 'FREDPP append block'},
    'Print all': {'prefix': 'pa', 'body': ['1,$P'], 'description': 'Print the complete buffer'},
    'System command': {'prefix': 'bang', 'body': ['!${1:dir}'], 'description': 'Execute a host system command'},
    'ZG system capture': {'prefix': 'zg', 'body': ['ZG(${1:buffer})!${2:dir}'], 'description': 'Capture system command output in a FREDPP buffer'},
    'Message': {'prefix': 'jm', 'body': ['JM/${1:message}/'], 'description': 'Display a FREDPP message'},
}

vscode_readme = '''# FREDPP Language Support for Visual Studio Code

Support du langage FREDPP pour les fichiers de procédures `.fredpp`.

## Fonctionnalités

- association automatique de l'extension `.fredpp` au langage FREDPP ;
- coloration des commandes FREDPP actuellement prises en charge ;
- coloration des adresses de lignes (`1`, `.`, `$`, `1,$`) ;
- coloration des motifs `/.../` ;
- coloration des directives de flux actuellement prises en charge (`\\B`,
  `\\L`, `\\S`, `\\F`) ;
- prise en charge de la commande système `!` et de `ZG` ;
- snippets de base pour les procédures FREDPP ;
- commentaire de ligne FRED avec `"`.

## Installation d'un VSIX

Dans Visual Studio Code :

1. ouvrez la vue **Extensions** ;
2. ouvrez le menu `...` ;
3. choisissez **Install from VSIX...** ;
4. sélectionnez le fichier `FREDPP-Language-vX.Y.Z.vsix`.

Le VSIX est déclaratif et ne contient aucun binaire natif FREDPP.

## Développement

Ouvrez le dossier `editors/vscode` dans Visual Studio Code puis utilisez le
mode de développement des extensions de VS Code.

## Exemple

```fredpp
" Exemple FREDPP
JM/Bonjour depuis FREDPP/
!echo FREDPP
ZG(capture)!echo sortie capturée
B(capture)
*
```

La liste des règles de coloration doit être mise à jour au fur et à mesure des
commandes effectivement implémentées dans FREDPP.
'''

example_fredpp = '''" Exemple FREDPP pour Visual Studio Code
JM/Bonjour depuis FREDPP/
!echo FREDPP
ZG(capture)!echo sortie capturee
B(capture)
*
'''

ensure_new('editors/vscode/package.json', json.dumps(package_json, ensure_ascii=False, indent=2) + '\n', '"name": "fredpp-language"')
ensure_new('editors/vscode/language-configuration.json', json.dumps(language_config, ensure_ascii=False, indent=2) + '\n', '"lineComment": "\\\""')
ensure_new('editors/vscode/syntaxes/fredpp.tmLanguage.json', json.dumps(grammar, ensure_ascii=False, indent=2) + '\n', '"scopeName": "source.fredpp"')
ensure_new('editors/vscode/snippets/fredpp.json', json.dumps(snippets, ensure_ascii=False, indent=2) + '\n', '"ZG system capture"')
ensure_new('editors/vscode/README.md', vscode_readme, '# FREDPP Language Support for Visual Studio Code')
ensure_new('editors/vscode/example.fredpp', example_fredpp, 'ZG(capture)!echo')

# ---------------------------------------------------------------------------
# RELEASE MANIFEST TEST
# ---------------------------------------------------------------------------

replace_once(
    'tests/test_release_manifest.cmake',
    '    scripts/package-release.sh\n    scripts/release-stats.ps1\n',
    '    scripts/package-release.sh\n'
    '    editors/vscode/package.json\n'
    '    editors/vscode/language-configuration.json\n'
    '    editors/vscode/syntaxes/fredpp.tmLanguage.json\n'
    '    editors/vscode/snippets/fredpp.json\n'
    '    editors/vscode/README.md\n'
    '    scripts/release-stats.ps1\n',
    'editors/vscode/package.json'
)

# ---------------------------------------------------------------------------
# RELEASE WORKFLOW
# ---------------------------------------------------------------------------

replace_once(
    '.github/workflows/release.yml',
    '    outputs:\n      version: ${{ steps.version.outputs.version }}\n      tag: ${{ steps.version.outputs.tag }}\n',
    '    outputs:\n'
    '      version: ${{ steps.version.outputs.version }}\n'
    '      tag: ${{ steps.version.outputs.tag }}\n'
    '      vscode_version: ${{ steps.version.outputs.vscode_version }}\n',
    'vscode_version: ${{ steps.version.outputs.vscode_version }}'
)

replace_once(
    '.github/workflows/release.yml',
    '          version="$(sed -nE \'s/^[[:space:]]*project\\(FREDPP VERSION ([0-9]+\\.[0-9]+\\.[0-9]+).*/\\1/p\' CMakeLists.txt)"\n\n'
    '          if [ -z "${version}" ]; then\n'
    '            echo "Unable to read project version from CMakeLists.txt" >&2\n'
    '            exit 1\n'
    '          fi\n',
    '          version="$(sed -nE \'s/^[[:space:]]*project\\(FREDPP VERSION ([0-9]+\\.[0-9]+\\.[0-9]+).*/\\1/p\' CMakeLists.txt)"\n'
    '          vscode_version="$(sed -nE \'s/^[[:space:]]*"version"[[:space:]]*:[[:space:]]*"([0-9]+\\.[0-9]+\\.[0-9]+)".*/\\1/p\' editors/vscode/package.json | head -n 1)"\n\n'
    '          if [ -z "${version}" ]; then\n'
    '            echo "Unable to read project version from CMakeLists.txt" >&2\n'
    '            exit 1\n'
    '          fi\n'
    '          if [ -z "${vscode_version}" ]; then\n'
    '            echo "Unable to read VS Code extension version" >&2\n'
    '            exit 1\n'
    '          fi\n',
    'Unable to read VS Code extension version'
)

replace_once(
    '.github/workflows/release.yml',
    '          echo "version=${version}" >> "${GITHUB_OUTPUT}"\n'
    '          echo "tag=${release_tag}" >> "${GITHUB_OUTPUT}"\n',
    '          echo "version=${version}" >> "${GITHUB_OUTPUT}"\n'
    '          echo "tag=${release_tag}" >> "${GITHUB_OUTPUT}"\n'
    '          echo "vscode_version=${vscode_version}" >> "${GITHUB_OUTPUT}"\n',
    'echo "vscode_version=${vscode_version}"'
)

amd64_install = '''      - name: Install build dependencies
        shell: bash
        run: |
          apt-get update
          apt-get install -y --no-install-recommends \\
            ca-certificates \\
            cmake \\
            dpkg-dev \\
            file \\
            g++ \\
            git \\
            gzip \\
            ninja-build \\
            tar
'''
insert_after(
    '.github/workflows/release.yml',
    amd64_install,
    '''
      - name: Verify amd64 environment
        shell: bash
        run: test "$(dpkg --print-architecture)" = "amd64"
''',
    'Verify amd64 environment'
)

insert_before(
    '.github/workflows/release.yml',
    '  publish:\n    name: Publish GitHub Release\n',
    '''  debian_arm64:
    name: Debian 13 arm64 packages
    needs: validate
    runs-on: ubuntu-24.04-arm
    container: debian:13

    steps:
      - name: Install build dependencies
        shell: bash
        run: |
          apt-get update
          apt-get install -y --no-install-recommends \\
            ca-certificates \\
            cmake \\
            dpkg-dev \\
            file \\
            g++ \\
            git \\
            gzip \\
            ninja-build \\
            tar

      - name: Verify arm64 environment
        shell: bash
        run: test "$(dpkg --print-architecture)" = "arm64"

      - name: Checkout release tag
        uses: actions/checkout@v4
        with:
          ref: ${{ needs.validate.outputs.tag }}
          fetch-depth: 0

      - name: Configure Release
        shell: bash
        run: cmake -S . -B out/build/release-gcc-arm64 -G Ninja -DCMAKE_BUILD_TYPE=Release -DFREDPP_BUILD_TESTS=ON

      - name: Build Release
        shell: bash
        run: cmake --build out/build/release-gcc-arm64 --parallel

      - name: Test Release
        shell: bash
        run: ctest --test-dir out/build/release-gcc-arm64 --output-on-failure

      - name: Verify executable architecture
        shell: bash
        run: file out/build/release-gcc-arm64/fredpp

      - name: Create Debian packages
        shell: bash
        run: bash scripts/package-release.sh out/build/release-gcc-arm64 out/release

      - name: Upload Debian ARM64 packages
        uses: actions/upload-artifact@v4
        with:
          name: release-debian-arm64
          path: |
            out/release/FREDPP-v${{ needs.validate.outputs.version }}-debian13-arm64.tar.gz
            out/release/fredpp_${{ needs.validate.outputs.version }}_arm64.deb
          if-no-files-found: error

  vscode:
    name: VS Code universal VSIX
    needs: validate
    runs-on: ubuntu-latest

    steps:
      - name: Checkout release tag
        uses: actions/checkout@v4
        with:
          ref: ${{ needs.validate.outputs.tag }}
          fetch-depth: 0

      - name: Setup Node.js
        uses: actions/setup-node@v4
        with:
          node-version: "22"

      - name: Validate VS Code extension JSON
        shell: bash
        run: |
          node -e '
            const fs = require("fs");
            for (const file of [
              "editors/vscode/package.json",
              "editors/vscode/language-configuration.json",
              "editors/vscode/syntaxes/fredpp.tmLanguage.json",
              "editors/vscode/snippets/fredpp.json"
            ]) JSON.parse(fs.readFileSync(file, "utf8"));
          '

      - name: Create VSIX
        shell: bash
        run: |
          mkdir -p out/release
          cd editors/vscode
          npx --yes @vscode/vsce@3.9.2 package \\
            --out "../../out/release/FREDPP-Language-v${{ needs.validate.outputs.vscode_version }}.vsix"

      - name: Upload VSIX
        uses: actions/upload-artifact@v4
        with:
          name: release-vscode
          path: out/release/FREDPP-Language-v${{ needs.validate.outputs.vscode_version }}.vsix
          if-no-files-found: error

''',
    '  debian_arm64:'
)

replace_once(
    '.github/workflows/release.yml',
    '    needs: [validate, windows, debian]\n',
    '    needs: [validate, windows, debian, debian_arm64, vscode]\n',
    'needs: [validate, windows, debian, debian_arm64, vscode]'
)

replace_once(
    '.github/workflows/release.yml',
    '          sha256sum \\\n'
    '            "FREDPP-v${{ needs.validate.outputs.version }}-windows-x64.zip" \\\n'
    '            "FREDPP-v${{ needs.validate.outputs.version }}-debian13-amd64.tar.gz" \\\n'
    '            "fredpp_${{ needs.validate.outputs.version }}_amd64.deb" \\\n'
    '            > SHA256SUMS.txt\n',
    '          sha256sum \\\n'
    '            "FREDPP-v${{ needs.validate.outputs.version }}-windows-x64.zip" \\\n'
    '            "FREDPP-v${{ needs.validate.outputs.version }}-debian13-amd64.tar.gz" \\\n'
    '            "fredpp_${{ needs.validate.outputs.version }}_amd64.deb" \\\n'
    '            "FREDPP-v${{ needs.validate.outputs.version }}-debian13-arm64.tar.gz" \\\n'
    '            "fredpp_${{ needs.validate.outputs.version }}_arm64.deb" \\\n'
    '            "FREDPP-Language-v${{ needs.validate.outputs.vscode_version }}.vsix" \\\n'
    '            > SHA256SUMS.txt\n',
    '            "FREDPP-Language-v${{ needs.validate.outputs.vscode_version }}.vsix" \\\n'
    '            > SHA256SUMS.txt\n'
)

replace_once(
    '.github/workflows/release.yml',
    '          assets=(\n'
    '            "out/release/FREDPP-v${RELEASE_VERSION}-windows-x64.zip"\n'
    '            "out/release/FREDPP-v${RELEASE_VERSION}-debian13-amd64.tar.gz"\n'
    '            "out/release/fredpp_${RELEASE_VERSION}_amd64.deb"\n'
    '            "out/release/SHA256SUMS.txt"\n'
    '          )\n',
    '          assets=(\n'
    '            "out/release/FREDPP-v${RELEASE_VERSION}-windows-x64.zip"\n'
    '            "out/release/FREDPP-v${RELEASE_VERSION}-debian13-amd64.tar.gz"\n'
    '            "out/release/fredpp_${RELEASE_VERSION}_amd64.deb"\n'
    '            "out/release/FREDPP-v${RELEASE_VERSION}-debian13-arm64.tar.gz"\n'
    '            "out/release/fredpp_${RELEASE_VERSION}_arm64.deb"\n'
    '            "out/release/FREDPP-Language-v${{ needs.validate.outputs.vscode_version }}.vsix"\n'
    '            "out/release/SHA256SUMS.txt"\n'
    '          )\n',
    '            "out/release/fredpp_${RELEASE_VERSION}_arm64.deb"\n'
)

# ---------------------------------------------------------------------------
# PROJECT DOCUMENTATION
# ---------------------------------------------------------------------------

insert_before(
    'CHANGELOG.md',
    '## v0.0.14\n',
    '''## v0.0.15

### Added

- Packaging Debian 13 ARM64 natif avec compilation et tests sur runner ARM64.
- Extension Visual Studio Code FREDPP version 0.1.0 sous `editors/vscode`.
- Publication du VSIX universel dans les Releases GitHub.
- Artefacts Debian ARM64 ajoutés au manifeste SHA-256.

### Changed

- Le packaging Debian détecte désormais l'architecture avec
  `dpkg --print-architecture` au lieu de forcer `amd64`.
- Le fichier `LISEZMOI-DEBIAN.txt` documente `amd64` et `arm64`.
- Le kit Windows documente le statut non signé de `fredpp.exe` et les
  protections Windows 11 susceptibles de le bloquer.

''',
    '## v0.0.15'
)

insert_before(
    'RELEASE_NOTES.md',
    '## Sprint 2.19',
    '''## Sprint 2.20 — Packaging multiplateforme et VS Code

- Windows x64 : archive portable ZIP conservée.
- Debian 13 amd64 : archive portable et paquet `.deb` conservés.
- Debian 13 arm64 : compilation native, tests et génération des deux paquets.
- Raspberry Pi : mise à disposition du paquet ARM64 pour les systèmes 64 bits
  compatibles.
- Visual Studio Code : ajout d'un VSIX universel FREDPP, sans composant natif.
- GitHub Release : les artefacts Windows, Debian amd64, Debian arm64 et VSIX
  sont couverts par `SHA256SUMS.txt`.
- Windows 11 : le kit rappelle que `fredpp.exe` n'est pas encore signé
  numériquement et peut être bloqué par SmartScreen ou Smart App Control.

''',
    '## Sprint 2.20 — Packaging multiplateforme et VS Code'
)

insert_before(
    'ROADMAP.md',
    '## Version v0.0.14 — Sprint 2.19\n',
    '''## Version v0.0.15 — Sprint 2.20

### Packaging et outillage

- ZIP portable Windows x64 ;
- paquets Debian 13 amd64 et arm64 ;
- compilation et tests natifs ARM64 avant publication ;
- support Raspberry Pi OS 64 bits compatible via le paquet arm64 ;
- support Visual Studio Code versionné sous `editors/vscode` ;
- VSIX universel publié avec les autres artefacts ;
- manifeste SHA-256 couvrant tous les artefacts ;
- avertissement Windows 11 pour le binaire non signé.

''',
    '## Version v0.0.15 — Sprint 2.20'
)

# ---------------------------------------------------------------------------
# VIRTUAL FINAL VALIDATION
# ---------------------------------------------------------------------------

must_have = [
    ('CMakeLists.txt', 'project(FREDPP VERSION 0.0.15'),
    ('CMakeLists.txt', 'dpkg --print-architecture'),
    ('.gitignore', '*.vsix'),
    ('scripts/package-release.sh', 'ARCHITECTURE="$(dpkg --print-architecture)"'),
    ('scripts/package-release.sh', 'fredpp_${VERSION}_${ARCHITECTURE}.deb'),
    ('packaging/LISEZMOI-WINDOWS.txt', '## Windows 11 - exécutable non signé'),
    ('packaging/LISEZMOI-DEBIAN.txt', '`amd64` et `arm64`'),
    ('.github/workflows/release.yml', 'runs-on: ubuntu-24.04-arm'),
    ('.github/workflows/release.yml', 'Verify arm64 environment'),
    ('.github/workflows/release.yml', '@vscode/vsce@3.9.2'),
    ('.github/workflows/release.yml', 'release-debian-arm64'),
    ('.github/workflows/release.yml', 'release-vscode'),
    ('.github/workflows/release.yml', 'needs: [validate, windows, debian, debian_arm64, vscode]'),
    ('tests/test_release_manifest.cmake', 'editors/vscode/package.json'),
    ('CHANGELOG.md', '## v0.0.15'),
    ('RELEASE_NOTES.md', '## Sprint 2.20 — Packaging multiplateforme et VS Code'),
    ('ROADMAP.md', '## Version v0.0.15 — Sprint 2.20'),
    ('editors/vscode/package.json', '"version": "0.1.0"'),
    ('editors/vscode/syntaxes/fredpp.tmLanguage.json', 'ZG'),
    ('editors/vscode/snippets/fredpp.json', 'ZG system capture'),
]
for rel, marker in must_have:
    if marker not in VIRTUAL.get(rel, ''):
        ERRORS.append(f'contrôle final : {marker!r} absent de {rel}')

if 'set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")' in VIRTUAL['CMakeLists.txt']:
    ERRORS.append('CMakeLists.txt contient encore l\'architecture Debian amd64 forcée')
if 'PACKAGE_NAME="FREDPP-v${VERSION}-debian13-amd64"' in VIRTUAL['scripts/package-release.sh']:
    ERRORS.append('package-release.sh contient encore un nom de paquet amd64 forcé')

for rel in [
    'editors/vscode/package.json',
    'editors/vscode/language-configuration.json',
    'editors/vscode/syntaxes/fredpp.tmLanguage.json',
    'editors/vscode/snippets/fredpp.json',
]:
    try:
        json.loads(VIRTUAL[rel])
    except Exception as exc:
        ERRORS.append(f'JSON invalide dans {rel} : {exc}')

workflow = VIRTUAL['.github/workflows/release.yml']

# Validate release artefacts in their actual workflow sections rather than
# relying on backslash-sensitive full-line markers.
sha_start = workflow.find('          sha256sum ')
sha_end = workflow.find('            > SHA256SUMS.txt', sha_start)
if sha_start == -1 or sha_end == -1 or sha_end <= sha_start:
    ERRORS.append('workflow release : bloc SHA256SUMS introuvable ou invalide')
    sha_block = ''
else:
    sha_block = workflow[sha_start:sha_end]

for expected in [
    'FREDPP-v${{ needs.validate.outputs.version }}-windows-x64.zip',
    'FREDPP-v${{ needs.validate.outputs.version }}-debian13-amd64.tar.gz',
    'fredpp_${{ needs.validate.outputs.version }}_amd64.deb',
    'FREDPP-v${{ needs.validate.outputs.version }}-debian13-arm64.tar.gz',
    'fredpp_${{ needs.validate.outputs.version }}_arm64.deb',
    'FREDPP-Language-v${{ needs.validate.outputs.vscode_version }}.vsix',
]:
    if expected not in sha_block:
        ERRORS.append(
            f'workflow release : artefact absent du bloc SHA256 : {expected!r}'
        )

assets_start = workflow.find('          assets=(')
assets_end = workflow.find('          )', assets_start)
if assets_start == -1 or assets_end == -1 or assets_end <= assets_start:
    ERRORS.append('workflow release : bloc assets=(...) introuvable ou invalide')
    assets_block = ''
else:
    assets_block = workflow[assets_start:assets_end]

for expected in [
    'out/release/FREDPP-v${RELEASE_VERSION}-windows-x64.zip',
    'out/release/FREDPP-v${RELEASE_VERSION}-debian13-amd64.tar.gz',
    'out/release/fredpp_${RELEASE_VERSION}_amd64.deb',
    'out/release/FREDPP-v${RELEASE_VERSION}-debian13-arm64.tar.gz',
    'out/release/fredpp_${RELEASE_VERSION}_arm64.deb',
    'out/release/FREDPP-Language-v${{ needs.validate.outputs.vscode_version }}.vsix',
    'out/release/SHA256SUMS.txt',
]:
    if expected not in assets_block:
        ERRORS.append(
            f'workflow release : artefact absent du bloc publish assets : {expected!r}'
        )

# Validate producer/upload paths separately.
for expected in [
    'out/release/FREDPP-v${{ needs.validate.outputs.version }}-debian13-arm64.tar.gz',
    'out/release/fredpp_${{ needs.validate.outputs.version }}_arm64.deb',
    'out/release/FREDPP-Language-v${{ needs.validate.outputs.vscode_version }}.vsix',
]:
    if expected not in workflow:
        ERRORS.append(
            f'workflow release : artefact de job producteur absent : {expected!r}'
        )

# Also verify that the publish job depends on every producer job.
if 'needs: [validate, windows, debian, debian_arm64, vscode]' not in workflow:
    ERRORS.append('workflow release : dépendances publish incomplètes')

print()
if ERRORS:
    print('=== PRECHECK ÉCHOUÉ : AUCUN FICHIER N\'A ÉTÉ MODIFIÉ ===')
    for i, error in enumerate(ERRORS, 1):
        print(f'{i:02d}. {error}')
    print(f'{len(ERRORS)} incompatibilité(s) détectée(s).')
    raise SystemExit(2)

changes = [rel for rel, content in VIRTUAL.items() if ORIGINAL.get(rel) != content]
print('=== PRECHECK COMPLET : OK ===')
print(f'{len(changes)} fichier(s) seront créés ou modifiés.')
for rel in changes:
    print(f'  - {rel}')
print('Application atomique du Sprint 2.20 v4...')

prepared: list[tuple[str, Path, Path, bytes | None, int | None]] = []
applied: list[tuple[str, Path, bytes | None, int | None]] = []

try:
    for rel in changes:
        q = p(rel)
        q.parent.mkdir(parents=True, exist_ok=True)
        previous_bytes = q.read_bytes() if q.exists() else None
        previous_mode = stat.S_IMODE(q.stat().st_mode) if q.exists() else None
        tmp = q.with_name(q.name + '.sprint220v4.tmp')
        tmp.write_text(VIRTUAL[rel], encoding='utf-8', newline='\n')
        if previous_mode is not None:
            os.chmod(tmp, previous_mode)
        prepared.append((rel, q, tmp, previous_bytes, previous_mode))

    for rel, q, tmp, previous_bytes, previous_mode in prepared:
        os.replace(tmp, q)
        applied.append((rel, q, previous_bytes, previous_mode))
except Exception:
    print('ERREUR pendant l\'application : rollback...')
    for rel, q, previous_bytes, previous_mode in reversed(applied):
        if previous_bytes is None:
            try:
                q.unlink()
            except FileNotFoundError:
                pass
        else:
            rollback = q.with_name(q.name + '.sprint220v4.rollback.tmp')
            rollback.write_bytes(previous_bytes)
            if previous_mode is not None:
                os.chmod(rollback, previous_mode)
            os.replace(rollback, q)
    for _, _, tmp, _, _ in prepared:
        try:
            tmp.unlink()
        except FileNotFoundError:
            pass
    raise

print()
print('Sprint 2.20 v4 appliqué avec succès.')
print('FREDPP : 0.0.15')
print('VS Code extension : 0.1.0')
print('Packaging : Windows x64 + Debian amd64 + Debian arm64 + VSIX')
print('Aucun fichier sous docs/fr/reference/commandes n\'a été modifié.')
print('Étape suivante : git diff --check puis rebuild local.')
