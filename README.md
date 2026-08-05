# FREDPP

**FREDPP** is a modern C++20 reimplementation of the historical FRED text editor.

## Version

Run FREDPP and enter `?version` to display the version of the executable.

## Historical lineage

QED → Multics QED → FRED → FREDPP

Historical behavior is documented before implementation. The historical manuals remain the source of truth.

## Implemented foundation

The repository currently contains the migrated and validated Step 7.4A code:

- Buffer and BufferManager
- FlowEngine and nested input sources
- CharacterStream, Lexer and TokenStream
- AddressParser, PatternParser and CommandParser
- AST nodes and address evaluation
- ExecutionContext and CommandExecutor
- Executable `P`, `L`, `D`, `A`, `B`, `I`, `C`, `M`, `T`, `G` and `Z` commands
- Automated tests through CTest

## Requirements

- CMake 3.24 or newer
- A C++20 compiler
- Visual Studio Community with the Desktop development with C++ workload on Windows

## Windows quick start

From PowerShell at the repository root:

```powershell
.\scripts\build.bat
.\scripts\test.bat
```

For a clean rebuild followed by tests (recommended before committing):

```powershell
.\scripts\rebuild.bat
```

## Run the editor

After a Debug build:

```powershell
.\out\build\x64-Debug\Debug\fredpp.exe
```

## Architecture

The intended processing chain is:

FlowEngine → CharacterStream → Lexer → TokenStream → AddressParser / PatternParser / CommandParser → AST → AddressEvaluator → Runtime

The parser never accesses the Buffer. The Runtime never parses. AddressEvaluator connects the AST to the Buffer.

## Windows build reliability

The Windows scripts build serially and MSVC uses embedded debug information.
This prevents intermittent PDB creation failures that can occur when the repository
is located in a synchronized folder such as Google Drive.
