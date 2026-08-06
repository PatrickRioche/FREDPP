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

## Help

Inside FREDPP:

```text
?       FRED command documentation currently embedded
?:      FREDPP-specific development and diagnostic commands
?*      historical whole-buffer alias documentation
?q      historical Q command documentation
?s      historical S command documentation
:help   same FREDPP-specific help as ?:
```

Only the French documentation of implemented FRED commands is embedded. The source files under `docs/fr/reference/commandes/` are not modified by the build.

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
- executable `P`, `L`, `D`, `A`, `B`, `I`, `C`, `M`, `T`, `G`, `Z`, `S` and `Q` commands;
- historical `Q` and `QQ` exit commands; the former FREDPP-specific `:quit` command has been removed;
- historical whole-buffer alias `*`, including bare `*` (`1,$P`) and forms such as `*D`;
- automated tests through CTest.

## Requirements

- CMake 3.24 or newer;
- a C++20 compiler;
- Visual Studio Community with the Desktop development with C++ workload on Windows.

## Windows quick start

From PowerShell at the repository root:

```powershell
.\scripts\build.bat
.\scripts\test.bat
```

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

Inside FREDPP, use `Q` for a normal exit or `QQ` for an immediate exit.

## Run the editor

Windows Debug build:

```powershell
.\out\build\x64-Debug\Debug\fredpp.exe
```

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
