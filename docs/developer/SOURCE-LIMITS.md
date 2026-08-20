# Source limits and compatibility classifications

## 1. Purpose

This document consolidates limits discovered during the seven-lot source audit.

Every item is classified as one of:

```text
ARCHITECTURAL
HISTORICAL LANGUAGE
FREDPP EXTENSION
CURRENT IMPLEMENTATION LIMIT
CURRENTLY UNSUPPORTED / NON IMPLEMENTED
GENERATED / PLATFORM CONSTRAINT
```

The classification matters: a current implementation gap must not accidentally
become a permanent architectural prohibition, and a FREDPP extension must not be
misrepresented as historical FRED behavior.

## 2. Architectural constraints

### Central flow expansion

**ARCHITECTURAL**

When historical syntax permits flow/substitution in command input:

```text
expand before parse
```

The mechanism is transversal.

Do not add `\S`, `\L`, `\C`, `\O`, etc. separately to individual commands.

### Parser independence from Buffer

**ARCHITECTURAL**

Parser may create address/command/pattern AST but must not inspect or mutate
Buffer state.

### Runtime receives AST

**ARCHITECTURAL**

CommandExecutor executes parsed AST and must not reparse raw command source.

### AST is syntax/state ownership only

**ARCHITECTURAL**

AST nodes do not execute operations or query editor state.

### Runtime Output abstraction

**ARCHITECTURAL**

Normal Runtime-visible command text goes through `fred::Output`.

CLI terminal/debug UI is outside this restriction.

### Shared engine across front ends

**ARCHITECTURAL**

Desktop/Android front ends coordinate the same `fredpp_core` implementation
rather than fork command semantics.

## 3. Buffer-name length

Historical compatibility record:

```text
historical FRED  15 characters
FREDPP           64 characters
```

Classification:

```text
15 -> HISTORICAL LANGUAGE/COMPATIBILITY
64 -> FREDPP EXTENSION
```

Current limit constant:

```cpp
limits::max_buffer_name_length = 64
```

BufferManager and Flow name resolution enforce it.

The low-level `Buffer` constructor itself only rejects an empty name.

## 4. Procedure label length

```text
15 characters
```

**HISTORICAL LANGUAGE LIMIT**

ProcedureRunner validates label names used by:

```text
@(label)
J(label)
```

## 5. Numeric register-name length

```text
14 characters
```

**HISTORICAL LANGUAGE LIMIT**

ProcedureRunner validates `N(register)` syntax.

ExecutionContext storage itself does not enforce this limit.

## 6. J message length

```text
2000 characters
```

**HISTORICAL LANGUAGE LIMIT**

CommandParser and Runtime both enforce the maximum for implemented JM/JP forms.

## 7. Input/procedure expansion depth

Default:

```text
256
```

**CURRENT IMPLEMENTATION SAFETY LIMIT**

Used to prevent runaway recursive/nested input.

InputStack protects active source depth.

Command-input and procedure code also track recursive expansion depth.

This value is not documented as a historical FRED language limit.

## 8. Historical encoded control values

Historical implementations could associate `\C`/related flow behavior with
machine-specific control values.

FREDPP uses:

```text
ordinary text byte
+
CharacterInterpretation metadata
```

**FREDPP IMPLEMENTATION MODEL**

This is intentional modernization, not missing compatibility.

The semantics must be preserved; the historical byte encoding need not be
reproduced in source files.

## 9. Pattern-language coverage

**CURRENTLY UNSUPPORTED / NON IMPLEMENTED**

Current PatternParser does not silently become PCRE.

Historical forms recorded for later compatibility work include, among others:

- column assertions;
- tags;
- word boundaries;
- defined patterns;
- fence;
- additional FRED class/case modifiers.

These are implementation gaps, not architectural prohibitions.

## 10. Pattern source locations

PatternParser-generated SourceLocation currently uses positions relative to the
reconstructed pattern fragment.

**CURRENT IMPLEMENTATION LIMIT**

Original command-line columns are not retained through that local parse.

Flow level is preserved.

## 11. Byte-oriented lexical/pattern model

Lexer/StringCharacterStream/PatternMatcher operate primarily on bytes.

**CURRENT IMPLEMENTATION LIMIT**

There is no general Unicode-code-point/grapheme lexer or matcher.

This does not prevent UTF-8 file I/O, which is validated separately.

## 12. G nested command support

Current G Runtime supports nested:

```text
P
D
Z
S
```

and rejects addressed nested commands.

**CURRENTLY UNSUPPORTED**

This must not be described as an architectural law of Global forever.

## 13. ZG nested command addressing

CommandParser currently rejects nested commands with their own explicit
address in ZG.

**CURRENTLY UNSUPPORTED**

## 14. L without filename

Runtime currently requires an explicit filename.

The form that would rely on current-file association reports that support is not
implemented.

**NON IMPLEMENTED**

## 15. WB

Parser/AST recognize historical WB.

Runtime reports historical GCOS/BCD output as unsupported and directs users to
WA/WU.

Classification:

```text
WB syntax/format -> HISTORICAL
execution in FREDPP -> CURRENTLY UNSUPPORTED
```

## 16. WU

WU is documented as a FREDPP UTF-8 write extension.

**FREDPP EXTENSION**

It must not be presented as historical FRED syntax unless historical evidence is
later found and the documentation is corrected.

## 17. Q external TSS form

CommandParser explicitly rejects Q! external TSS command behavior.

**NON IMPLEMENTED**

Normal Q/QQ are implemented.

## 18. Quit architecture

Historical Q/QQ drive Runtime exit state.

The special development `:quit` has been removed.

Do not reintroduce `:quit` as a duplicate normal exit path.

Classification:

```text
Q/QQ -> implemented historical command path
:quit -> deliberately absent FREDPP duplicate
```

## 19. Procedure numeric language

ProcedureRunner currently implements a minimal N syntax:

```text
:value
=value
<value
>value
```

and a limited numeric operand set:

```text
integer
$
.
#
```

**CURRENT IMPLEMENTATION LIMIT**

It must not be treated as the complete historical numeric language.

## 20. Numeric same-line tail

After current N operations, ProcedureRunner allows only a conditional/unconditional
J(label) tail.

**CURRENT IMPLEMENTATION LIMIT**

Diagnostics still containing development wording such as "Lot 4" / "this lot"
are stale milestone text, not language rules.

## 21. Relative forward address overflow

AddressEvaluator computes forward relative address using unsigned arithmetic
before ordinary bounds validation.

There is no explicit overflow guard.

**TECHNICAL IMPLEMENTATION LIMIT**

This matters only for extreme artificial values but should be addressed in a
future behavior-focused pass.

## 22. FlowEngine reuse after exception

Full-flow expansion shares an InputStack.

If an exception escapes while stack content remains, the engine does not
explicitly clear that stack before reuse.

A reused instance can then report:

```text
flow engine is already executing
```

**TECHNICAL IMPLEMENTATION LIMIT**

High-level command expansion normally avoids it by creating a fresh FlowEngine.

## 23. Whitespace reconstruction

Lexer discards Normal horizontal whitespace.

CommandParser reconstructs some textual operands from source-location column
gaps using ordinary spaces.

**CURRENT IMPLEMENTATION LIMIT**

Original tabs/CR bytes are not preserved byte-for-byte in those reconstructed
operands.

## 24. CommandDescriptor callback completeness

Several core descriptor callbacks construct default/placeholder nodes because
CommandParser intercepts operand-heavy commands first.

Directly invoking all registry callbacks as a standalone parsing API is unsafe.

**CURRENT IMPLEMENTATION/API LIMIT**

The correct source-input API remains CommandParser.

## 25. AST non-null child invariants

Several AST getters dereference owned `unique_ptr` children without runtime null
checks.

Parser-produced complete nodes preserve the invariant.

Some registry placeholder callbacks can deliberately construct incomplete nodes
that must not escape into ordinary execution.

**CURRENT IMPLEMENTATION INVARIANT**

## 26. Runtime L/G/WB distinctions

Use precise terminology:

```text
L current-file form -> NON IMPLEMENTED
G extra nested forms -> CURRENTLY UNSUPPORTED
WB BCD output        -> CURRENTLY UNSUPPORTED historical format
```

Do not label these "forbidden by architecture."

## 27. File I/O encoding

Current supported text representation:

```text
ASCII
UTF-8
```

Binary/NUL-containing input is rejected.

Non-ASCII input must validate as UTF-8.

**CURRENT IMPLEMENTATION SCOPE**

## 28. Terminal renderer width

HelpManager approximates display width by counting UTF-8 non-continuation bytes.

It does not implement wcwidth/grapheme-aware terminal width.

**PRESENTATION IMPLEMENTATION LIMIT**

French accented help text is the primary current target.

## 29. Help Markdown parser

Terminal help rendering supports a controlled subset of Markdown.

**PRESENTATION IMPLEMENTATION LIMIT**

It is not a general CommonMark parser.

## 30. HelpPager

Current limits include:

- no terminal-width wrapping model;
- pages count rendered physical lines, not visual wrapped rows;
- resize is measured once on pager entry;
- POSIX PageUp/PageDown recognizes specific escape sequences.

**PLATFORM/PRESENTATION LIMIT**

## 31. Version state freshness

Version/Git/source dirty state is generated at CMake configure time.

Changing source afterward without reconfiguration does not update embedded
source-state metadata.

**GENERATED BUILD-METADATA CONSTRAINT**

## 32. Desktop procedure library path

Simple procedure names additionally search:

```text
C:\fredpp\library
```

on Windows.

There is no equivalent fixed-library fallback on non-Windows in current CLI
code.

**CURRENT PLATFORM-SPECIFIC FRONT-END LIMIT**

## 33. Desktop no-argument bootstrap

Current desktop no-procedure interactive branch does not run the procedure-mode
historical bootstrap that creates d/t/u and processes user init.

**CURRENT FRONT-END BEHAVIOR**

This is documented rather than silently assumed to be universal startup
semantics.

## 34. Interactive text terminator differences

Desktop interactive A/I/C:

```text
exact \F
```

Android interactive A/I/C:

```text
exact \F
```

ProcedureRunner text blocks:

```text
trimmed \F or \f
```

**CURRENT FRONT-END DIFFERENCE**

These forms should not be unified without a deliberate compatibility decision.

## 35. Android JNI handles

Native session is represented as a raw pointer encoded into jlong.

Zero is detectable; arbitrary non-zero stale handles are not.

**JNI LIFECYCLE CONSTRAINT**

Kotlin NativeSession is responsible for correct create/destroy/reset lifecycle.

## 36. Android JNI strings

Bridge uses:

```text
GetStringUTFChars
NewStringUTF
```

**JNI PLATFORM CONSTRAINT**

These APIs use modified UTF-8 semantics.

The bridge is intended for command/output text, not arbitrary binary data.

## 37. Android bootstrap

AndroidSession does not execute desktop process bootstrap:

```text
d/t/u
argv B(0)
user .init
desktop procedure-path lookup
```

**CURRENT FRONT-END DIFFERENCE**

Shared Core/Runtime does not own these desktop policies.

## 38. Android version fallback

Compose startup currently contains a hard-coded fallback software version if
NativeBridge.version() fails.

**TECHNICAL FRONT-END DEBT**

This can become stale.

The authoritative source remains the generated native version.

## 39. `src/main.cpp`

A minimal standalone version reporter remains in the source tree but is not the
current desktop CLI target.

**UNUSED/LEGACY SOURCE IN CURRENT BUILD**

Root CMake builds:

```text
src/cli/main.cpp
```

for `fredpp`.

The retained stub is now explicitly documented to avoid ambiguity.

## 40. Generated source exclusions

Do not hand-edit:

```text
VersionInfo.hpp
EmbeddedHelp.hpp
EmbeddedHelp.cpp
```

**GENERATED FILES**

Their source-of-truth inputs are CMake configuration, Git metadata and Markdown
help sources.

## 41. Classification rule for future work

Before changing a limit, ask:

1. Is it required by historical FRED?
2. Is it a deliberate FREDPP extension?
3. Is it only an implementation shortcut/gap?
4. Is it an architectural separation that must be preserved?

Only category 4 should normally be described as "forbidden by architecture."

Use explicit phrases where useful:

```text
NON IMPLEMENTÉ
ACTUELLEMENT NON SUPPORTÉ
EXTENSION FREDPP
LIMITE HISTORIQUE FRED
INTERDIT PAR L'ARCHITECTURE
```
