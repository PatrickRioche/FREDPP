# FREDPP

**FREDPP** is a modern C++20 reimplementation project for the historical FRED text editor.

This archive is the official **v0.0.1 repository foundation**. It provides a clean Git-ready structure, a compilable executable, CMake/CTest integration, Windows scripts, and continuous integration.

## Historical lineage

QED → Multics QED → FRED → FREDPP

Historical behavior must be documented before implementation. The manuals remain the source of truth.

## Requirements

- CMake 3.24 or newer
- A C++20 compiler
- Visual Studio Community with Desktop C++ workload on Windows

## Windows quick start

From PowerShell at the repository root:

```powershell
.\scripts\build.bat
.\scripts\test.bat
```

Or perform a clean rebuild and test:

```powershell
.\scripts\rebuild.bat
```

## Visual Studio

Open the repository folder directly in Visual Studio. Visual Studio can configure the root `CMakeLists.txt` as a CMake project.

## Git initialization

Extract the contents of the `FREDPP` folder into your existing local repository, then run:

```powershell
git add .
git commit -m "FREDPP v0.0.1 - Initial foundation"
git push
```

## Architecture rule

The intended processing chain is:

FlowEngine → CharacterStream → Lexer → TokenStream → AddressParser → PatternParser → CommandParser → AST → AddressEvaluator → Runtime

The parser never accesses the Buffer. The Runtime never parses. AddressEvaluator connects the AST to the Buffer.
