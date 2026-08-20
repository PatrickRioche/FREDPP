# FREDPP source architecture

## 1. Architectural objective

FREDPP separates language interpretation from editor state and front-end
presentation.

The core direction is:

```text
front end
   ↓
Flow expansion
   ↓
lexical analysis
   ↓
parsing / AST
   ↓
runtime evaluation/execution
   ↓
Buffer state / Output
```

This separation is one of the main compatibility and maintainability
constraints of the project.

## 2. Main layers

### Front ends

Current front ends:

```text
Desktop CLI      src/cli/main.cpp
Android JNI/UI   android/app/src/main/cpp/native-lib.cpp + Kotlin
Procedure        ProcedureRunner
```

They coordinate input/output lifecycle but do not own normal FRED command
semantics.

### Core

```text
Buffer
BufferManager
Limits
CharacterInterpretation
```

Core owns editable state and basic metadata.

It does not parse or execute FRED source.

### Flow

```text
InputSource
InputStack
BufferInputSource
InputCharacter
FlowEngine
CommandInputExpansion
FlowCharacterStream
```

Flow implements language-level input expansion and preserves interpretation
metadata.

### Lexer

```text
CharacterStream
StringCharacterStream
Lexer
Token
TokenType
TokenStream
Diagnostic
SourceLocation
```

Lexer converts characters into syntax-neutral tokens.

### AST

```text
AstNode
AddressNode hierarchy
PatternNode hierarchy
CommandNode hierarchy
```

AST owns parsed syntax and operands.

It does not query Buffer state.

### Parser

```text
AddressParser
PatternParser
CommandParser
ParseError
```

Parser builds AST only.

### Command registry

```text
CommandDescriptor
CommandRegistry
```

The registry maps known mnemonics to metadata/construction callbacks.

It is not the complete operand parser for every command.

### Runtime

```text
AddressEvaluator
PatternMatcher
ExecutionContext
CommandExecutor
ProcedureRunner
Output
ConsoleOutput
```

Runtime resolves AST against state and performs command effects.

### Help / terminal presentation

```text
HelpManager
HelpPager
Terminal
generated EmbeddedHelp
```

This is a presentation subsystem parallel to the language/runtime pipeline.

## 3. Ownership map

### Buffer ownership

```text
BufferManager
    owns unique_ptr<Buffer>
```

Other components borrow Buffers/BufferManager.

### AST ownership

Composite AST nodes own child AST nodes through `std::unique_ptr`.

Parser returns `std::unique_ptr<...>` to callers.

No shared AST ownership is required.

### Flow ownership

`InputStack` owns nested `InputSource` objects.

`BufferInputSource` borrows Buffer.

`FlowCharacterStream` owns expanded Character values.

### Runtime service ownership

Desktop:

```text
main() stack objects
    BufferManager
    Output
    ExecutionContext (borrows)
    Registry
    Executor
    ProcedureRunner (borrows)
```

Android:

```text
AndroidSession owns all native service objects
```

### Output ownership

ExecutionContext borrows Output.

`ZG` temporarily exchanges Output but never takes ownership.

## 4. Central command-input expansion rule

This is a permanent architectural rule.

When historical FRED syntax allows flow directives in command input:

```text
raw input
  ↓
expand_command_input_with_metadata()
  ↓
FlowCharacterStream
  ↓
Lexer
  ↓
CommandParser
```

Expansion is **not** implemented separately inside each command.

Example:

```text
B(nombuf) = doc1

B(\S(nombuf))
    ↓ expansion
B(doc1)
    ↓ parser
BufferCommandNode("doc1")
```

This rule applies to existing and future commands wherever the historical
language permits the syntax.

## 5. Historical control semantics in modern text

FREDPP does not need to encode historical GCOS/TSS control effects as special
machine character values.

Instead ordinary source bytes carry:

```cpp
CharacterInterpretation
```

with:

```text
Normal
Literal
ForcedSpecial
```

This preserves language behavior while allowing normal modern text/procedure
files.

## 6. Parser/state boundary

Parsers must not:

- inspect current Buffer;
- test whether a line exists;
- perform file I/O;
- mutate editor state;
- execute commands.

Address syntax can therefore preserve forms such as absolute zero even when a
later command/runtime layer rejects or specially interprets it.

## 7. Address evaluation boundary

`AddressEvaluator` converts AddressNode syntax into existing Buffer line ranges.

Command-specific insertion exceptions such as A/I/R/T address-zero behavior are
handled by CommandExecutor rather than weakening the generic evaluator.

## 8. Command execution boundary

`CommandExecutor` receives already-parsed AST.

It may:

- evaluate addresses;
- mutate Buffer state;
- perform file/system I/O;
- update ExecutionContext;
- write through Output.

It must not re-tokenize/reparse the original command source.

## 9. Procedure orchestration

ProcedureRunner is allowed to parse procedure-control syntax that is not an
ordinary CommandNode form:

```text
\B(buffer)
@(label)
J(label)[T|F]
N(register)...
\F text termination
```

Ordinary FRED command lines inside procedures still use:

```text
Flow -> Lexer -> CommandParser -> CommandExecutor
```

## 10. Front-end separation

Desktop-specific policy includes:

- process arguments;
- environment variables;
- .init lookup;
- terminal pager;
- fixed Windows procedure-library fallback.

Android-specific policy includes:

- JNI session handle;
- Compose transcript/prompt;
- Reset;
- mutex/queued command state.

These policies should not leak into shared `fredpp_core` unless intentionally
promoted to cross-platform behavior.

## 11. Help generation boundary

Help Markdown is source documentation.

CMake embeds selected pages into generated C++.

Generated `EmbeddedHelp.*` is build output and should not be hand-edited.

HelpManager renders/looks up help; it does not derive command semantics from the
Runtime dynamically.

## 12. Version boundary

Software version source of truth:

```text
CMake project(... VERSION ...)
```

Generated `VersionInfo.hpp` provides:

```text
version
Git commit
source-tree state
dirty flag
```

Runtime/front ends read those values.

Current user-facing documentation should not duplicate a hard-coded live
version number.

## 13. Platform reuse

The root target:

```text
fredpp_core
```

contains the reusable engine/help infrastructure.

Desktop links it into `fredpp`.

Android native CMake adds the root project with CLI/tests disabled and links the
same `fredpp_core` into `fredpp_android`.

This makes compatibility bugs in Core/Flow/Parser/Runtime shared rather than
forked per platform.

## 14. Architectural prohibitions summary

### Flow

Must not implement command execution.

### Lexer

Must not know Buffer semantics or command registry semantics.

### Parser

Must not access Buffer or execute commands.

### AST

Must not perform runtime work.

### Runtime

Must not reparse source commands.

### Command-specific implementations

Must not each invent independent flow substitution.

### Front ends

Must not duplicate ordinary command semantics already in Runtime.

### Shared core

Must not depend on desktop terminal or Android JNI/Compose APIs.

## 15. Documentation map

Detailed module documents:

```text
LEXER.md
AST.md
PARSER.md
COMMAND.md
CORE.md
FLOW.md
RUNTIME.md
CLI.md
HELP.md
TERMINAL.md
ANDROID.md
SOURCE-FLOW.md
SOURCE-LIMITS.md
files/SOURCE-FILES.md
```

Together these documents describe the current source architecture rather than
only the historical/specification target.
