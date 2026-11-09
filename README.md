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
?:      FREDPP-specific development and diagnostic commands
?*      historical whole-buffer alias documentation
?q      historical Q command documentation
?r      historical R command documentation
?s      historical S command documentation
?n:     historical N: numeric-register assignment documentation
?w      historical W and WA documentation
?wu     FREDPP UTF-8 write extension
:help   same FREDPP-specific help as ?:
```

Only the French documentation of implemented FRED commands is embedded. The source files under `docs/fr/reference/commandes/` are not modified by the build.

For the actual syntax, supported behavior, examples and limitations of all
**30 commands in v0.0.30**, use the [French practical command guide](docs/fr/fredpp/guide-commandes-0.0.30.md).
The embedded `?commande` pages preserve the historical reference and may
describe forms that FREDPP has not implemented yet.

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
- executable `P`, `L`, `D`, `A`, `B`, `I`, `C`, `K`, `M`, `T`, `G`, `Z`, `S`, `Q`, `R` and `W` commands;
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
- portable Debian 13 amd64 TAR.GZ;
- Debian amd64 package;
- SHA-256 checksum manifest.

The release page is available at:

```text
https://github.com/PatrickRioche/FREDPP/releases/latest
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
