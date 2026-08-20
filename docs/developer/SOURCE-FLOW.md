# End-to-end source flows

## 1. Interactive desktop command

```text
stdin line
  ↓
CLI help/dev/comment classification
  ↓
expand_command_input_with_metadata()
  ↓
ExpandedCommandInput
  ↓
FlowCharacterStream
  ↓
Lexer
  ↓
TokenStream
  ↓
CommandParser::parse_one()
  ↓
CommandNode
  ↓
CommandExecutor
  ↓
ExecutionContext
  ├── BufferManager
  └── ConsoleOutput
```

For A/I/C, the CLI pauses after parsing, collects text through `\F`, then calls
the specialized executor method.

## 2. Command chaining

One expanded input line can contain several commands.

```text
TokenStream
  ↓ parse_one
Command 1 -> execute
  ↓ parse_one
Command 2 -> execute
  ...
```

The same ExecutionContext is shared.

Q/QQ stops the remaining chain through `exit_requested()`.

## 3. Procedure command

```text
procedure Buffer snapshot
  ↓ physical line
comment/control classification
  ↓
central Flow expansion
  ↓
FlowCharacterStream
  ↓
Lexer
  ↓
CommandParser
  ↓
CommandExecutor
```

Procedure-only jump/numeric/label forms are handled by ProcedureRunner around
this ordinary-command pipeline.

## 4. Nested procedure Buffer

```text
procedure line: \B(child)
  ↓
ProcedureRunner
  ↓
execute_buffer_impl(child, depth+1)
  ↓
child Buffer snapshot
```

Maximum depth protects recursion.

## 5. Full-flow active Buffer injection

```text
FlowEngine input
  ↓ sees \B(name)
InputStack.push(BufferInputSource)
  ↓
child source consumes first
  ↓ EOF
InputStack pops child
  ↓
parent resumes
```

`\B` injected characters are Normal and can execute additional directives.

`\S` / `\L` injected characters are Literal.

## 6. Command-input nested buffer-name calculation

Example:

```text
B(nombuf) = doc1
B(doc1)   = BONJOUR

\S(\S(nombuf))
```

Flow:

```text
outer \S name parser
   ↓ nested \S(nombuf)
doc1
   ↓ outer lookup
BONJOUR
```

This happens before CommandParser.

## 7. G/S pattern flow

When command input contains a flow substitution inside G/S syntax:

```text
Flow expansion
   ↓
InputCharacter interpretation metadata
   ↓
FlowCharacterStream
   ↓
Lexer tokens retain interpretation
   ↓
CommandParser reconstructs pattern fragment + metadata
   ↓
PatternParser
   ↓
PatternNode
   ↓
PatternMatcher at Runtime
```

Literal metadata prevents injected pattern metacharacters from becoming
structural when they are supposed to remain literal.

## 8. Address flow

```text
Lexer / TokenStream
  ↓
AddressParser
  ↓
AddressNode
  ↓
CommandNode owns optional address
  ↓
CommandExecutor
  ↓
AddressEvaluator
  ↓
Buffer LineRange
```

Parser never asks Buffer whether the address exists.

## 9. R file read

```text
ReadCommandNode
  ↓
CommandExecutor
  ↓
read bytes
  ↓
reject NUL/binary
  ↓
BOM / UTF-8 validation
  ↓
line-ending/final-newline detection
  ↓
Buffer::load_file OR insert_after
  ↓
counter / condition update
```

## 10. W file write

```text
WriteCommandNode
  ↓
resolve range / filename / encoding
  ↓
validate selected text
  ↓
write bytes/newlines
  ↓
counter / condition
  ↓
full write only:
    associate file
    update encoding metadata
    mark clean
```

## 11. S substitution

```text
SubstituteCommandNode
  ↓
AddressEvaluator
  ↓ each line
PatternMatcher::find
  ↓
replacement expansion (& / escaped char)
  ↓
Buffer::replace
  ↓
condition/current-line
```

## 12. G global

```text
GlobalCommandNode
  ↓ outer range/default whole buffer
  ↓
PatternMatcher per line
  ↓ selected/inverted lines
  ↓
nested P/D/Z/S execution
  ↓
counter = selected count
```

Current nested-command limitations are documented in RUNTIME/SOURCE-LIMITS.

## 13. ZG output capture

```text
ExecutionContext Output = original
  ↓ exchange_output(StringOutput)
nested CommandExecutor
  ↓
captured text
  ↓ restore original Output
  ↓
split lines
  ↓
destination Buffer
```

Restoration also occurs on exceptions.

## 14. Help flow

```text
docs/fr Markdown
  ↓ CMake configure
generated EmbeddedHelp.cpp
  ↓
HelpManager::load()
  ↓
render_markdown_for_terminal()
  ↓
HelpPager (desktop)
or StringOutput/Compose transcript (Android)
```

Help source is not modified by the build.

## 15. Version flow

```text
CMake project VERSION
Git query at configure time
  ↓
generated VersionInfo.hpp
  ↓
src/version.cpp
  ↓
fredpp::version/git_commit/source_state
  ↓
--version / ?version / Android version()
```

## 16. Android command

```text
Compose input
  ↓
NativeSession
  ↓ JNI executeLine(handle, text)
  ↓
AndroidSession mutex
  ↓
meta classification OR
central Flow expansion
  ↓
FlowCharacterStream
  ↓
Lexer / TokenStream / CommandParser
  ↓
queued AST
  ↓
CommandExecutor
  ↓
StringOutput
  ↓
JNI string
  ↓
Compose transcript
```

The engine path is shared with desktop.

## 17. Android A/I/C queue

```text
command line: A ... P
  ↓ parser queues A, P
  ↓
A becomes pending_text_command
  ↓
UI submissions collected
  ↓ exact \F
execute_append()
  ↓
resume queued P
```

This queue is front-end state only.

## 18. Exit flow

```text
Q/QQ AST
  ↓
CommandExecutor
  ↓
ExecutionContext::request_exit()
  ↓
front-end loop observes exit_requested()
```

Desktop breaks the REPL/command chain.

Android keeps the session allocated but blocks further execution until Reset.

No special `:quit` path is required.
