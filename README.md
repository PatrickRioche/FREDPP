# FREDPP

**FREDPP** is a modern C++20 reimplementation of the historical FRED text editor.

## Version and source identity

Run FREDPP and enter:

```text
?version
```

The command displays:

- the software version;
- the Git commit used to configure the build;
- whether the source tree was clean or modified;
- the matching section in `ROADMAP.md`.

No version number is duplicated manually in this README.

## Development history

- [Chronological development log](docs/project/DEVELOPMENT_LOG.md): dated work on `dev`, CI results, and local validation status.
- [Changelog](CHANGELOG.md): version-oriented summary of product changes.
- [Roadmap](ROADMAP.md): upcoming features and milestones.
- [Git history](https://github.com/PatrickRioche/FREDPP/commits/dev/): authoritative commit-level traceability.

## Help

Inside FREDPP:

```text
?       FRED command documentation currently embedded
?limites  Limites de securite FREDPP (documentation francaise)
?directives  Compatibilite des neuf directives de flot
?:      FREDPP-specific development and diagnostic commands
?*      historical whole-buffer alias documentation
?q      historical Q command documentation
?r      historical R command documentation
?s      historical S command documentation
?n:     historical N: numeric-register assignment documentation
?n+     numeric addition (also ?n-, ?n*, ?n/, ?n%, ?n=, ?n<, ?n>, ?n&, ?n|)
?n~     numeric bitwise complement (also ?n^, ?n{, ?n})
?na     current line into register (also ?nb, ?nd, ?nf, ?nl, ?np)
?w      historical W and WA documentation
?wu     FREDPP UTF-8 write extension
:help   same FREDPP-specific help as ?:
```

Only the French documentation of implemented FRED commands is embedded. The source files under `docs/fr/reference/commandes/` are not modified by the build.

For the actual syntax, supported behavior, examples and limitations of all
**103 historical entries recognized in development v0.0.103**, use the [French practical guide for lot 91–103](docs/fr/fredpp/guide-commandes-0.0.103.md), then the [lot 81–90 guide](docs/fr/fredpp/guide-commandes-0.0.90.md).
Coverage of an entry does **not** imply a complete GCOS emulation; see the
documented portable adaptations and limitations.
The embedded `?commande` pages preserve the historical reference and may
describe forms that FREDPP has not implemented yet.

## Limites de sécurité (FREDPP)

Sans réglage particulier, chaque procédure est limitée à **1 000 000 de lignes exécutées** et **60 secondes**. Les appels imbriqués `\\B` partagent ce budget. Les répétitions `U` disposent d'un budget distinct de **1 000 000 d'itérations cumulées** et **60 secondes**, partagé entre les `U` imbriqués.

Les valeurs effectives, y compris celles provenant de l'environnement, sont visibles sans lancer de procédure :

```powershell
fredpp --limits
$env:FREDPP_REPEAT_MAX_STEPS = '5'
fredpp --limits
Remove-Item Env:FREDPP_REPEAT_MAX_STEPS
```

Les variables configurables sont `FREDPP_PROCEDURE_MAX_STEPS`, `FREDPP_PROCEDURE_MAX_SECONDS`, `FREDPP_REPEAT_MAX_STEPS` et `FREDPP_REPEAT_MAX_SECONDS`. Les valeurs doivent être des entiers strictement positifs ; les durées sont plafonnées à 86 400 secondes. Les commandes internes bloquantes ne sont pas interrompues pendant leur exécution.

Voir [la documentation française des limites](docs/fr/fredpp/limites.md), également disponible par `?limites` dans FREDPP. Ces protections ne limitent pas encore la taille mémoire totale des buffers.

## Historical stream directives

The nine expected directives are `\B`, `\C`, `\E`, `\L`, `\N`, `\O`, `\R`, `\S`, and `\W`. Their current coverage and limitations are documented in the [French flow reference](docs/fr/fredpp/directives-flot.md), also available in the CLI with `?directives`. The `\W` directive injects the filename associated with the selected buffer; the `W` command writes a file.

## Historical lineage

QED → Multics QED → FRED → FREDPP

Historical behavior is documented before implementation. The historical manuals remain the source of truth.

## Implemented foundation

The repository currently contains:

- Buffer and BufferManager;
- FlowEngine and nested input sources;
- CharacterStream, Lexer and TokenStream;
- AddressParser, PatternParser and CommandParser;
- AST nodes and address evaluation;
- ExecutionContext and CommandExecutor;
- executable `P`, `L`, `D`, `A`, `B`, `I`, `C`, `K`, `M`, `T`, `G`, `Z`, `S`, `Q`, `R` and `W` commands, plus all 21 historical `N`-family commands (arithmetic, bitwise, shifts, line counters, formatting and printing);
- file reading with automatic ASCII or UTF-8 detection;
- file writing with `W`, forced ASCII with `WA`, and UTF-8 without BOM with the FREDPP extension `WU`;
- recognition of historical `WB` with an explicit unsupported GCOS/BCD diagnostic;
- per-buffer file association, encoding, line-ending and modified-state tracking;
- historical `Q` and `QQ` exit commands; the former FREDPP-specific `:quit` command has been removed;
- historical whole-buffer alias `*`, including bare `*` (`1,$P`) and forms such as `*D`;
- automated tests through CTest.

## Official releases

Ready-to-run packages are published from Git tags through GitHub Releases:

- portable Windows x64 ZIP;
- portable Debian 13 amd64 and arm64 TAR.GZ archives;
- Debian amd64 and arm64 packages;
- portable macOS Intel x64 and Apple Silicon arm64 TAR.GZ archives;
- VS Code language extension VSIX;
- SHA-256 checksum manifest covering the eight binary packages.

The `v0.1.0` release is a **pre-release tagged on `dev`** (103/103 historical
reference entries recognized, not full historical emulation). GitHub supplies
the source ZIP and TAR.GZ automatically, for **11 displayed assets** in total.
Stable releases are tagged on `main`; the `v0.1.0` pre-release does not modify it.
Android continues to use its separate CI workflow and is not part of this
release asset set.

The release page is available at:

```text
https://github.com/PatrickRioche/FREDPP/releases/latest
```

For the `dev` pre-release, use the specific tag page (pre-releases are not
selected by `/releases/latest`):

```text
https://github.com/PatrickRioche/FREDPP/releases/tag/v0.1.0
```

Download statistics can be displayed with:

```powershell
.\scripts\release-stats.ps1
```

```bash
./scripts/release-stats.sh
```

## Requirements

- CMake 3.24 or newer;
- a C++20 compiler;
- Visual Studio Community with the Desktop development with C++ workload on Windows.

## Windows quick start

From PowerShell at the repository root:

```powershell
.\scripts\build.bat
.\scripts\test.bat
.\scripts\test.ps1 -R "^(test_zap_copy|cli_zap_copy)$"
```

Use `test.ps1` for PowerShell regex filters with `|` alternation; invoking
`test.bat` with such a regex can be parsed incorrectly by `cmd.exe`.

Windows Debug artifacts are stored by default in
`%LOCALAPPDATA%\FREDPP\build\x64-Debug`, outside the source checkout and
`%TEMP%`. Set `$env:FREDPP_BUILD_DIR` before building/testing to select a
different permanent directory. VS 2022 users can set
`$env:FREDPP_CMAKE_GENERATOR = 'Visual Studio 17 2022'`.

For a clean rebuild followed by tests:

```powershell
.\scripts\rebuild.bat
```

## Linux quick start

```bash
./scripts/rebuild.sh gcc
./scripts/rebuild.sh clang
```

## Exit the editor

Inside FREDPP, use `Q` for a normal exit. `Q` refuses to leave while a buffer contains unsaved changes; use `W` to save or `QQ` for an immediate exit.

## Run the editor

Windows Debug build (after `scripts/build.bat`):

```powershell
& "$env:LOCALAPPDATA\FREDPP\build\x64-Debug\Debug\fredpp.exe"
```

With a custom `FREDPP_BUILD_DIR`, run `& "$env:FREDPP_BUILD_DIR\Debug\fredpp.exe"` instead.

Linux GCC build:

```bash
./out/build/linux-debug/fredpp
```

Linux Clang build:

```bash
./out/build/linux-clang/fredpp
```

## Architecture

The intended processing chain is:

FlowEngine → CharacterStream → Lexer → TokenStream → AddressParser / PatternParser / CommandParser → AST → AddressEvaluator → Runtime

The parser never accesses the Buffer. The Runtime never parses. AddressEvaluator connects the AST to the Buffer.

## Windows build reliability

The Windows scripts build serially and MSVC uses embedded debug information. This prevents intermittent PDB creation failures that can occur when the repository is located in a synchronized folder such as Google Drive.
