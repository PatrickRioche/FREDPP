# C++ source-file inventory and documentation coverage

## 1. Scope

This inventory closes the seven-lot source-documentation pass for the maintained
FREDPP C++ product tree.

Included extensions:

```text
.hpp
.h
.cpp
```

Included source roots:

```text
include/
src/
android/app/src/main/cpp/
```

The audited tree contains:

```text
47 headers
26 implementation/source .cpp files
73 C++ source files total
```

Every one of these 73 files is assigned to a documentation lot below.

## 2. Explicit exclusions

The following are **not** counted as maintained C++ source files in the 73-file
inventory:

- build-generated `VersionInfo.hpp`;
- build-generated `EmbeddedHelp.hpp` / `EmbeddedHelp.cpp`;
- CMake/Gradle build descriptions;
- tests;
- scripts;
- Markdown/documentation source;
- Android Kotlin/Compose source;
- Android resources/manifests.

Generated C++ is documented through the subsystem that generates/consumes it,
rather than edited directly.

Android Kotlin is nevertheless mapped in `ANDROID.md` because it owns the JNI
session lifecycle.

## 3. Coverage meaning

A file is considered covered when at least one of these applies:

1. its public interface/source carries Doxygen/implementation comments added by
   the source-documentation pass;
2. a large orchestration `.cpp` is intentionally left unchanged but reviewed and
   documented exhaustively in the corresponding developer module document;
3. a generated/peripheral relationship is explicitly documented with its
   callers, ownership and architectural boundary.

No source file is silently marked covered merely because it compiles.

## 4. Lot summary

| Lot | Area | C++ files assigned |
|---:|---|---:|
| 1 | Lexer + character/token infrastructure | 14 |
| 2 | AST + parsers | 16 |
| 3 | Command descriptor/registry | 3 |
| 4 | Core + Flow | 15 |
| 5 | Runtime | 14 |
| 6 | CLI / Terminal / Help / version | 9 |
| 7 | Remaining/peripheral C++ | 2 |
| **Total** | | **73** |

## 5. Complete inventory

### Lot 1

- `include/fred/core/CharacterInterpretation.hpp`
- `include/fred/lexer/Character.hpp`
- `include/fred/lexer/CharacterStream.hpp`
- `include/fred/lexer/Diagnostic.hpp`
- `include/fred/lexer/Lexer.hpp`
- `include/fred/lexer/SourceLocation.hpp`
- `include/fred/lexer/StringCharacterStream.hpp`
- `include/fred/lexer/Token.hpp`
- `include/fred/lexer/TokenStream.hpp`
- `include/fred/lexer/TokenType.hpp`
- `src/lexer/Diagnostic.cpp`
- `src/lexer/Lexer.cpp`
- `src/lexer/StringCharacterStream.cpp`
- `src/lexer/TokenStream.cpp`

### Lot 2

- `include/fred/ast/AbsoluteAddressNode.hpp`
- `include/fred/ast/AddressNode.hpp`
- `include/fred/ast/AstNode.hpp`
- `include/fred/ast/CommandNode.hpp`
- `include/fred/ast/CurrentAddressNode.hpp`
- `include/fred/ast/LastAddressNode.hpp`
- `include/fred/ast/PatternNodes.hpp`
- `include/fred/ast/RangeAddressNode.hpp`
- `include/fred/ast/RelativeAddressNode.hpp`
- `include/fred/parser/AddressParser.hpp`
- `include/fred/parser/CommandParser.hpp`
- `include/fred/parser/ParseError.hpp`
- `include/fred/parser/PatternParser.hpp`
- `src/parser/AddressParser.cpp`
- `src/parser/CommandParser.cpp`
- `src/parser/PatternParser.cpp`

### Lot 3

- `include/fred/command/CommandDescriptor.hpp`
- `include/fred/command/CommandRegistry.hpp`
- `src/command/CommandRegistry.cpp`

### Lot 4

- `include/fred/core/Buffer.hpp`
- `include/fred/core/BufferManager.hpp`
- `include/fred/core/Limits.hpp`
- `include/fred/flow/BufferInputSource.hpp`
- `include/fred/flow/CommandInputExpansion.hpp`
- `include/fred/flow/FlowCharacterStream.hpp`
- `include/fred/flow/FlowEngine.hpp`
- `include/fred/flow/InputCharacter.hpp`
- `include/fred/flow/InputSource.hpp`
- `include/fred/flow/InputStack.hpp`
- `src/core/Buffer.cpp`
- `src/core/BufferManager.cpp`
- `src/flow/BufferInputSource.cpp`
- `src/flow/FlowEngine.cpp`
- `src/flow/InputStack.cpp`

### Lot 5

- `include/fred/runtime/AddressEvaluator.hpp`
- `include/fred/runtime/CommandExecutionError.hpp`
- `include/fred/runtime/CommandExecutor.hpp`
- `include/fred/runtime/ConsoleOutput.hpp`
- `include/fred/runtime/ExecutionContext.hpp`
- `include/fred/runtime/Output.hpp`
- `include/fred/runtime/PatternMatcher.hpp`
- `include/fred/runtime/ProcedureRunner.hpp`
- `src/runtime/AddressEvaluator.cpp`
- `src/runtime/CommandExecutor.cpp`
- `src/runtime/ConsoleOutput.cpp`
- `src/runtime/ExecutionContext.cpp`
- `src/runtime/PatternMatcher.cpp`
- `src/runtime/ProcedureRunner.cpp`

### Lot 6

- `include/HelpManager.h`
- `include/HelpPager.h`
- `include/Terminal.h`
- `include/fredpp/version.hpp`
- `src/HelpManager.cpp`
- `src/HelpPager.cpp`
- `src/Terminal.cpp`
- `src/version.cpp`
- `src/cli/main.cpp`

### Lot 7

- `src/main.cpp`
- `android/app/src/main/cpp/native-lib.cpp`

## 6. Large implementation files intentionally not replaced

Several large `.cpp` files were reviewed in full but deliberately not rewritten
only to add comments:

```text
src/parser/CommandParser.cpp
src/runtime/CommandExecutor.cpp
src/runtime/ProcedureRunner.cpp
src/cli/main.cpp
src/HelpManager.cpp
src/HelpPager.cpp
src/Terminal.cpp
src/version.cpp
```

Their current behavior is documented respectively in:

```text
PARSER.md
RUNTIME.md
CLI.md
HELP.md
TERMINAL.md
```

This policy reduced risk during a documentation-only pass.

## 7. Last uncovered C++ resolved by Lot 7

Before Lot 7, two product C++ files were outside the previous module lots:

### `src/main.cpp`

A minimal version-reporting `main()` retained in the repository.

The active desktop executable does **not** use it: root CMake currently builds
`fredpp` from `src/cli/main.cpp`.

Lot 7 adds an explicit source comment so future readers do not mistake the stub
for the production CLI.

### `android/app/src/main/cpp/native-lib.cpp`

The Android JNI front end.

It owns Android session state but reuses the standard FREDPP C++ Core/Flow/
Lexer/Parser/Runtime stack.

Lot 7 adds source comments to the session, command pipeline and JNI lifecycle,
with `ANDROID.md` documenting the Kotlin/JNI relationship.

## 8. Coverage conclusion

For the audited `main` tree at the time of Lot 7:

```text
C++ source files discovered: 73
assigned to documentation lots: 73
unassigned C++ source files: 0
```

Therefore this documentation pass has no known uncovered `.hpp`, `.h` or `.cpp`
file in the maintained product source roots listed above.

Future source files must be added to this inventory (or generated-file exclusion
list) when introduced.
