# Android native front-end architecture

## 1. Scope

The Android application is a separate UI/front-end over the same FREDPP C++20
engine.

Native bridge:

```text
android/app/src/main/cpp/native-lib.cpp
```

JNI declaration/lifecycle wrapper:

```text
android/app/src/main/java/fr/fredpp/android/NativeBridge.kt
```

Compose UI:

```text
android/app/src/main/java/fr/fredpp/android/MainActivity.kt
```

Native build entry:

```text
android/app/src/main/cpp/CMakeLists.txt
```

The Android layer does not contain a second implementation of FRED semantics.

## 2. Native build relationship

Android native CMake computes the FREDPP repository root, then configures the
root project as a subdirectory with:

```text
FREDPP_BUILD_TESTS = OFF
FREDPP_BUILD_CLI   = OFF
```

It links:

```text
fredpp_android (shared library)
    ↓
fredpp_core
    + log
    + android
```

Therefore the JNI library directly reuses the same C++ Core/Flow/Lexer/Parser/
Runtime library as the desktop CLI.

## 3. `AndroidSession`

One Android session owns:

```text
BufferManager
StringOutput
ExecutionContext
CommandRegistry
CommandExecutor
ProcedureRunner
```

and Android-specific front-end state:

```text
pending A/I/C command
pending text lines
queued parsed commands
mutex
```

Construction order is significant.

`ExecutionContext` borrows `buffers` and `output`.

`ProcedureRunner` borrows:

```text
buffers
context
registry
executor
```

All those service objects are members of the same session and therefore outlive
the borrowing objects for the session lifetime.

## 4. Session handle model

JNI exposes the native `AndroidSession*` as an opaque `jlong`.

Conversion is performed through `std::intptr_t`.

A zero handle is rejected by `from_handle()` with:

```text
session Android FREDPP invalide
```

### Safety limit

A non-zero stale/foreign numeric handle cannot be validated by the native code.

Correct lifecycle therefore depends on the Kotlin `NativeSession` wrapper:

```text
createSession()
    ↓
use handle
    ↓
destroySession()
    ↓
handle = 0
```

This is a JNI/lifecycle constraint, not a FRED language concern.

## 5. Kotlin `NativeBridge`

The singleton loads:

```text
fredpp_android
```

with `System.loadLibrary`.

Native API:

```text
createSession()          -> Long
destroySession(Long)
executeLine(Long,String) -> String
prompt(Long)             -> String
version()                -> String
```

The JNI symbol names in `native-lib.cpp` match package/class:

```text
fr.fredpp.android.NativeBridge
```

## 6. Kotlin `NativeSession`

`NativeSession` is `AutoCloseable`.

It:

- creates one native session in its constructor;
- exposes `isAvailable`;
- forwards execute/prompt;
- destroys the handle in `close()`;
- resets by closing then creating a fresh session.

When session creation returns 0:

```text
executeLine -> "error: moteur FREDPP indisponible\n"
prompt      -> "?> "
```

## 7. Reset semantics

Runtime Q/QQ marks `ExecutionContext` as exited.

The native session deliberately remains allocated after Q/QQ, but further
execution returns:

```text
[session terminée par Q/QQ — utilisez Reset]
```

The UI Reset action destroys the old native session and constructs a new one.

This cleanly resets:

- buffers;
- options;
- condition/counter/registers;
- queued text/commands;
- exit state.

## 8. Per-session synchronization

`execute_line()` and `current_prompt()` lock:

```cpp
session.mutex
```

This serializes access to the mutable native session.

The mutex is per session; different sessions do not share a global runtime
lock.

## 9. Native output model

Android uses:

```cpp
fred::StringOutput
```

rather than ConsoleOutput.

At the start of each submitted UI line:

```cpp
session.output.clear()
```

so JNI returns only output produced while processing that submission.

Runtime commands still write through the normal `Output` abstraction.

## 10. Android command pipeline

For ordinary FRED input:

```text
UI String
   ↓
JNI from_jstring
   ↓
expand_command_input_with_metadata
   ↓
FlowCharacterStream
   ↓
Lexer
   ↓
TokenStream
   ↓
CommandParser::parse_one
   ↓
queued CommandNode AST
   ↓
CommandExecutor
   ↓
StringOutput
   ↓
JNI jstring
```

This is the same central expansion-before-parsing architecture as desktop and
procedure execution.

## 11. Flow-expansion bypass

Like the desktop CLI, Android starts with `make_command_input()` and applies
central expansion when the first non-horizontal-space byte is neither:

```text
:
"
```

Thus development/meta commands and comment lines bypass command-input
substitution.

No command-specific `\S` patch exists in Android.

## 12. Procedure-buffer directives

After central expansion Android calls:

```cpp
ProcedureRunner::execute_buffer_directive(command_input)
```

before ordinary CommandParser processing.

Therefore standalone procedure:

```text
\B(buffer)
```

uses the same ProcedureRunner as desktop.

## 13. Command chaining

The whole expanded input line is parsed through repeated:

```cpp
parser.parse_one()
```

Parsed nodes are pushed into:

```text
queued_commands
```

The queue allows Android to pause a command chain when one node requires
interactive text input and resume after that text command finishes.

## 14. A/I/C text-input state machine

When a queued command is:

```text
A
I
C
```

the AST becomes:

```text
pending_text_command
```

and execution pauses.

Subsequent UI lines are appended literally to:

```text
pending_text_lines
```

until the submitted line is exactly:

```text
\F
```

Then the correct specialized executor method is called and queued commands
resume.

### Current Android behavior

The terminator is exact and case-sensitive.

The text lines are not command-input-expanded.

This mirrors the desktop interactive A/I/C approach more closely than the
ProcedureRunner's trimmed/case-insensitive procedure terminator.

## 15. Exit inside a command chain

`run_queued_commands()` and parsing loops test:

```cpp
context.exit_requested()
```

so Q/QQ prevents later queued commands from executing.

No Android-specific quit flag is introduced.

## 16. Error recovery

`execute_line()` catches `std::exception` and unknown native exceptions.

On command-processing failure it clears:

```text
queued_commands
pending_text_command
pending_text_lines
```

and writes a textual error to the session output.

The persistent Buffer/ExecutionContext state already modified before the
exception is not generally rolled back.

This matches ordinary command execution semantics: the JNI layer is not a
transaction around the whole submitted line.

## 17. JNI exception boundary

JNI exported functions prevent C++ exceptions from crossing into Kotlin.

`executeLine()` converts failures to:

```text
error: <message>
```

or:

```text
error: erreur native FREDPP inconnue
```

`prompt()` falls back to:

```text
?>
```

`createSession()` returns 0 on construction failure.

## 18. Java/native string conversion

Input uses:

```cpp
GetStringUTFChars
```

and output uses:

```cpp
NewStringUTF
```

These JNI functions use modified UTF-8 conventions.

The bridge is intended for textual FREDPP command/output data; it is not a
general arbitrary-byte transport interface.

A failed/null input conversion currently becomes an empty native string.

## 19. Android meta commands

Native Android handles:

```text
?version
?topic
:help
:print
```

### `?version`

Uses generated version/Git/source-state metadata.

### `?topic`

Uses HelpManager and terminal-rendered plain text, but does not invoke
HelpPager; the Compose transcript itself scrolls.

### `:help`

Android-specific alias that loads the `:` special-help topic.

The desktop special help is normally reached with:

```text
?:
```

and its current documented developer-command list does not advertise `:help`.

This is a **front-end compatibility difference**, not a Runtime command.

### `:print`

Writes the same debug-style buffer listing directly into StringOutput.

## 20. Unknown development command

After a leading `:` bypasses Flow/meta handling, an unrecognized colon command
produces:

```text
Unknown development command
```

It is not sent to CommandParser.

## 21. Prompt states

Native prompt:

```text
pending text command -> "text> "
exit requested       -> "[Q] "
normal               -> "<buffer-name>> "
```

The Compose UI renders that prompt beside its input field.

## 22. Compose UI lifecycle

`MainActivity` creates one `NativeSession` with Compose `remember`.

`DisposableEffect` closes it when the composition is disposed.

Reset calls `session.reset()`.

The transcript is UI state and does not live in the native engine.

## 23. Android `:cls`

`:cls` is implemented in Compose UI before JNI submission.

It clears the transcript only.

Therefore native-lib.cpp does not implement `:cls`, unlike desktop CLI where
`:cls` calls Terminal::clear_terminal().

This is an appropriate UI-layer difference.

## 24. Version-display anomaly recorded

The Compose initial transcript currently calls:

```text
NativeBridge.version()
```

but has a hard-coded fallback version string if that call fails.

A hard-coded current version can become stale independently of the generated
native version.

This documentation pass does **not** change it, but records it as a front-end
technical debt. The preferred source of truth remains NativeBridge.version() /
the CMake-generated FREDPP version.

## 25. Bootstrap difference from desktop procedure mode

`AndroidSession` construction creates the ordinary C++ BufferManager state only.

It does not execute the desktop CLI's process-specific bootstrap helpers for:

```text
B(d)
B(t)
B(u)
B(0) argv parameters
user .init.fredpp
procedure path lookup
```

Those helpers live in `src/cli/main.cpp`, not in reusable Core/Runtime.

This is a deliberate/current front-end difference unless a future requirement
moves bootstrap policy into shared services.

## 26. Ownership boundary

Kotlin owns the **lifetime decision** for the native session.

Native `AndroidSession` owns C++ services.

C++ Core/Runtime remains unaware of JNI/Compose.

Dependency direction:

```text
Compose UI
   ↓
NativeSession / NativeBridge
   ↓ JNI
AndroidSession
   ↓
shared fredpp_core
```

`fredpp_core` must not depend back on Android APIs.

## 27. Current Android-specific limits

- opaque non-zero JNI handles cannot be independently validated;
- modified-UTF-8 JNI conversion is used;
- no native transaction rollback for a whole submitted command chain;
- `:help` is Android-specific front-end behavior;
- desktop bootstrap/user-init environment is not shared with Android;
- Compose contains a hard-coded version fallback;
- Android native bridge itself has no separate unit-test target in the root
  50-test desktop CTest suite.

Because Lot 7 adds comments to `native-lib.cpp`, validation should include an
Android native/app build in addition to the normal CTest suite.
