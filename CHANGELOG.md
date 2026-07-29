# Changelog

All notable changes to FREDPP are documented here.

## [0.0.2-r1] - 2026-07-29

### Fixed
- Prevented intermittent MSVC `C1090` PDB failures by using embedded debug information (`/Z7`).
- Added MSVC `/FS` synchronization for compiler file writes.
- Changed Windows and CI builds to a single build job for deterministic operation in synchronized folders.

### Validation
- All 13 CTest tests are configured and buildable.

## [0.0.2] - 2026-07-29

### Added
- Migrated the validated Step 7.4A implementation into the Git repository.
- Buffer and BufferManager.
- FlowEngine and input-source stack.
- CharacterStream, Lexer, TokenStream and diagnostics.
- Address, pattern and command parsers with AST nodes.
- ExecutionContext, AddressEvaluator and CommandExecutor.
- Core P, L, D and A command execution.
- Historical implementation specifications.
- Twelve unit/integration executables plus a CLI smoke test.

### Changed
- Unified the historical code with the v0.0.1 CMake, scripts and CI foundation.
- Project version advanced to 0.0.2.

## [0.0.1] - 2026-07-29

### Added
- Initial repository foundation, CMake/CTest infrastructure, scripts and documentation.
