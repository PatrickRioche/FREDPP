# FREDPP source documentation

This directory is the developer-oriented companion to the normative
specifications under `docs/specs/`.

The goal is to document the implementation as it exists in the source tree:
class responsibilities, inputs and outputs, ownership, side effects, invariants,
limits, and the relationships between layers.

## Source pipeline

```text
Desktop CLI / ProcedureRunner / Android front end
          ↓
FlowEngine / CommandInputExpansion
          ↓
CharacterStream
          ↓
Lexer
          ↓
TokenStream
          ↓
AddressParser / PatternParser / CommandParser
          ↓
AST
          ↓
AddressEvaluator / PatternMatcher
          ↓
CommandExecutor
          ↓
ExecutionContext
   ├── BufferManager / Buffer
   └── Output
```

Help/terminal presentation is a parallel front-end concern:

```text
docs/fr Markdown
      ↓ CMake embedding
HelpManager
      ↓
HelpPager / Android transcript
      ↓
Terminal / Compose UI
```

For command text, flow/substitution that is valid in historical FRED syntax is
a **pre-parsing language stage**. It must remain centralized rather than being
implemented independently by each command.

## Architecture and audit documents

- [Overall source architecture](SOURCE-ARCHITECTURE.md)
- [End-to-end source flows](SOURCE-FLOW.md)
- [Limits and compatibility classifications](SOURCE-LIMITS.md)
- [Complete C++ source inventory and coverage](files/SOURCE-FILES.md)
- [C++ implementation documentation completeness](CPP-DOCUMENTATION.md)

The completed seven-lot audit assigns every maintained product `.hpp`, `.h` and
`.cpp` file under `include/`, `src/` and the Android native C++ source directory
to a documentation lot.

## Windows-focused development lane

Windows Debug scripts (`scripts/build.bat`, `test.bat`, and `rebuild.bat`,
with PowerShell wrappers) use an external persistent build directory:
`%LOCALAPPDATA%\FREDPP\build\x64-Debug` by default, or
`FREDPP_BUILD_DIR` when configured. This avoids writing MSVC objects into
Google Drive synchronized sources, and a rebuild cleans only selected build
targets instead of deleting whole output trees. VS 2022 users can override
`FREDPP_CMAKE_GENERATOR`.

The `test.bat -R` selector is quoted internally when forwarded to CTest,
so regex alternation such as `^(test_zap_copy|cli_zap_copy)$` remains safe
when the batch file is invoked from PowerShell.

The development version `0.0.N` follows the number `N` of implemented
historical commands in `docs/project/COMMAND_STATUS.md`; CMake rejects
mismatches during configuration. This never authorizes an automatic release.

Every push to the `dev` branch with product code, test or CI changes triggers
the dedicated Windows/MSVC build in `.github/workflows/windows-development.yml`.
This lane compiles all C++ targets and runs a small, explicit set of engineering
regressions covering Flow, Runtime, parsing, historical T implementation, M, G
and S. It does not replace the deferred historical compatibility suite or user
validation on Windows 11.

- [Visible T condition/jump integration test on Windows](TESTING-T-CONDITION.md)
- [Historical K copy integration test on Windows](TESTING-K-COPY.md)
- [Historical ZK insertion-copy test on Windows](TESTING-ZK-COPY.md)
- [Dated development log and verification status](../project/DEVELOPMENT_LOG.md)

## Module documentation

- [Lexer and character/token infrastructure](LEXER.md)
- [AST nodes and ownership](AST.md)
- [Address, pattern and command parsers](PARSER.md)
- [Command descriptors and registry](COMMAND.md)
- [Core buffers, manager and limits](CORE.md)
- [Flow engine and command-input expansion](FLOW.md)
- [Runtime evaluation, execution and procedures](RUNTIME.md)
- [Desktop CLI orchestration and bootstrap](CLI.md)
- [Embedded help generation and rendering](HELP.md)
- [Terminal primitives and interactive help pager](TERMINAL.md)
- [Android JNI/Compose front-end integration](ANDROID.md)

## Documentation rules

1. Public C++ interfaces use Doxygen-compatible comments in `.hpp`/`.h`.
2. Implementation comments in `.cpp` explain non-obvious behavior, invariants
   and historical compatibility rules; they do not restate obvious code.
3. Documentation distinguishes:
   - architectural constraints;
   - current implementation limits;
   - historical FRED language rules;
   - FREDPP extensions;
   - functionality not yet implemented.
4. Documentation-only lots must not change runtime behavior.
5. Statements about behavior should be cross-checked against implementation and
   tests before being written as a contract.
6. Older milestone specifications are not assumed to describe every feature of
   the current implementation; later code/tests take precedence when the
   developer documentation describes the current source tree.
7. Flow/substitution in command arguments is a transversal language capability:
   when historical syntax permits it, expansion occurs before command parsing,
   never as command-by-command patches.
8. Runtime receives parsed AST and must not reparse source command text.
9. Front ends may coordinate services and presentation, but FRED command
   semantics remain in parser/runtime layers.
10. The current software version is generated by the build and should be exposed
    through `?version`/`--version` rather than duplicated as a hard-coded value
    in user documentation.
11. Generated C++ (`VersionInfo.hpp`, `EmbeddedHelp.*`) is documented through its
    generation/consumption path and must not be manually maintained.
12. New product `.hpp`, `.h` or `.cpp` files should be added to
    `files/SOURCE-FILES.md` when introduced so coverage does not silently drift.
