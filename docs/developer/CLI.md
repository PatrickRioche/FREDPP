# CLI source architecture

## 1. Scope

The desktop command-line front end is implemented in:

```text
src/cli/main.cpp
```

It is deliberately an orchestration layer around already documented services:

```text
HelpManager
Terminal / HelpPager
BufferManager
CommandInputExpansion
Lexer / TokenStream / CommandParser
CommandExecutor / ExecutionContext
ProcedureRunner
version API
```

`main.cpp` is not copied into this documentation lot because it is a large
front-end orchestration file. Its current functions and control flow are
documented here without risking an accidental behavioral edit.

## 2. CLI architectural role

The CLI is responsible for:

- process startup and Windows console setup;
- `--version`;
- optional procedure-file/bootstrap mode;
- user `.init` handling;
- interactive prompt/read loop;
- FRED help dispatch;
- FREDPP development commands;
- central command-input flow expansion;
- command parsing/chaining;
- interactive A/I/C text collection;
- presentation of top-level errors.

The CLI must not reimplement core command semantics. Parsed FRED commands are
delegated to `CommandExecutor`.

## 3. Windows UTF-8 console setup

`configure_console_utf8()` is a no-op outside Windows.

On Windows it calls:

```cpp
SetConsoleOutputCP(CP_UTF8);
SetConsoleCP(CP_UTF8);
```

The current implementation does not test those return values.

This setup belongs to the presentation/process layer; Lexer still operates on
the bytes supplied by its input stream.

## 4. `--version`

The first process-level special case is:

```text
fredpp --version
```

It is checked before HelpManager, buffers, Runtime or user init are constructed.

Output is produced by `print_version_info()` using:

```cpp
fredpp::version()
fredpp::git_commit()
fredpp::source_state()
```

The version is generated from CMake. The CLI does not own a separate hard-coded
software-version constant.

## 5. Version information shown by the CLI

`print_version_info()` displays:

```text
FREDPP v<version>
Commit Git : <commit>
État des sources : <state>
Feuille de route : ROADMAP.md — section v<version>
```

The same helper is used for interactive:

```text
?version
```

`?version` is a CLI special case and is not an ordinary embedded HelpManager
topic.

## 6. Runtime service construction

Normal startup constructs the service graph in this order:

```text
HelpManager
BufferManager
ConsoleOutput(std::cout)
ExecutionContext(BufferManager, Output)
CommandExecutor
core CommandRegistry
ProcedureRunner
```

The ownership is stack-based in `main()`.

ExecutionContext and ProcedureRunner borrow the longer-lived objects.

## 7. Two startup modes

There are two materially different CLI modes.

### No procedure argument

With no positional argument, the program proceeds directly to the interactive
banner/REPL.

The historical bootstrap buffers `d`, `t`, `u` and the user init file are
**not** initialized by the current no-argument branch.

BufferManager still supplies its normal bootstrap buffer `"0"`.

This is a current CLI implementation fact.

### Procedure argument

When `argc >= 2`, the first argument is treated as a procedure name/path unless
it was the already-handled `--version`.

The CLI performs the historical/bootstrap sequence before running it.

## 8. Procedure-mode bootstrap order

Current order:

```text
1. initialize_historical_bootstrap_environment()
2. execute_user_init_if_present()
3. if init requested Q/QQ -> return 0
4. initialize_parameter_buffer(argc, argv)
5. resolve_procedure_path(argv[1])
6. select B(0)
7. ProcedureRunner::load_and_execute_file()
```

The ordering matters because the user init runs before command-line parameters
are copied into B(0).

## 9. Historical bootstrap buffers

`initialize_historical_bootstrap_environment()` obtains current local time and
creates:

```text
B(d) = mm/dd/yy
B(t) = HH:MM
B(u) = user id
```

The source comment identifies the historical reference as DNB11A.

The buffers are created with `get_or_create()` and explicitly selected so they
appear in BufferManager's recent-selection order used by FB.

After initialization the function explicitly selects `u`.

### Current modified-state side effect

The helper populates these buffers through `Buffer::append()`.

As a consequence, under the current Buffer implementation these bootstrap
buffers are marked modified.

This document records the behavior; it does not classify it as either correct
or incorrect historical semantics.

## 10. Local date/time portability

`current_local_time()` uses:

```text
Windows -> localtime_s
POSIX   -> localtime_r
```

Failure to obtain or convert time raises `std::runtime_error`.

`format_bootstrap_time()` uses `std::put_time` and verifies the output stream
state.

## 11. User identity

`current_user_id()` resolves:

```text
Windows:
    USERNAME

POSIX:
    USER
    then LOGNAME
```

If no non-empty environment variable is available:

```text
unknown
```

is stored in B(u).

## 12. Parameter buffer B(0)

`initialize_parameter_buffer()` obtains existing/creates `"0"` and requires it
to be:

```text
empty
unmodified
without associated file
```

It appends every process argument beginning at:

```cpp
argv[2]
```

Therefore:

```text
argv[1] -> procedure
argv[2..] -> one B(0) line per parameter
```

Appending arguments marks B(0) modified under current Buffer semantics.

## 13. Procedure-name resolution

`resolve_procedure_path(name)` rejects an empty name.

### Explicit requests

A request is considered explicit if its path:

- is absolute; or
- contains a parent path; or
- already has an extension.

The requested path is checked directly.

If an explicit path has no extension, `.fredpp` is also tried.

### Simple names

A simple extensionless name is converted to:

```text
<name>.fredpp
```

Candidates are tried in order.

All platforms:

```text
current working directory / <name>.fredpp
```

Windows additionally:

```text
C:\fredpp\library\<name>.fredpp
```

There is currently no equivalent fixed-library fallback on non-Windows builds.

### Failure diagnostic

If no regular file exists, the error lists every path that was tried.

## 14. User init location

`resolve_user_init_location()` first examines:

```text
FREDPP_INIT
```

### `FREDPP_INIT` non-empty

The exact supplied path is used as an explicit override.

If it does not exist, startup fails.

### `FREDPP_INIT` exists but is empty

User init is explicitly disabled.

### No `FREDPP_INIT`

Default home source:

```text
Windows -> USERPROFILE
POSIX   -> HOME
```

Default file:

```text
<home>/fredpp/.init.fredpp
```

If the relevant home variable is missing/empty, init is disabled.

## 15. User init execution

For the default location, a missing `.init.fredpp` is not an error.

For an explicit `FREDPP_INIT`, a missing file is an error.

An existing path must be a regular file.

The reserved temporary Buffer is:

```text
__init
```

If it already exists, startup fails.

The file is loaded/executed through:

```cpp
ProcedureRunner::load_and_execute_file(path, "__init")
```

The reserved buffer is erased after success and also in error paths.

Failures are wrapped with the init filename/path for context.

## 16. Exit requested by init

Immediately after user init:

```cpp
if (execution_context.exit_requested()) {
    return 0;
}
```

Thus `Q`/`QQ` from init can stop procedure-mode startup before B(0) parameters
and target-procedure execution.

## 17. Procedure success and failure

If target procedure execution succeeds:

```text
process returns 0
```

and no interactive REPL is entered.

If target procedure execution throws:

```text
error: <message>
procedure stopped; entering interactive debug mode
```

is written to stderr and the process intentionally falls through into the
interactive loop.

The existing BufferManager/ExecutionContext are preserved, allowing inspection
of the failed procedure state.

## 18. Interactive banner

Interactive mode prints a banner containing the generated version and tells the
user:

```text
?   -> FRED help
?:  -> FREDPP commands
Q/QQ -> exit
```

There is no special `:quit` command in the current CLI.

Historical `Q`/`QQ` own the exit behavior through Runtime.

## 19. Prompt

Each loop iteration writes:

```text
<current-buffer-name>>
```

The prompt therefore follows BufferManager selection dynamically.

Input uses:

```cpp
std::getline(std::cin, input)
```

EOF ends the loop.

## 20. Help dispatch precedes command parsing

The CLI finds the first non-space character among:

```text
space, tab, CR
```

If that byte is `?`, the entire line is handled as help before flow expansion
or FRED command parsing.

The topic is the remaining text after `?`, with leading spaces/tabs removed.

## 21. `?version`

`?version` invokes `print_version_info()` directly.

It is intentionally not looked up through HelpManager.

This keeps current build identity dynamic/generated rather than stored in a
Markdown help page.

## 22. Other `?topic`

The helper `print_help_topic()`:

1. calls `HelpManager::load_for_terminal(topic)`;
2. passes the result to `show_paged_help()`;
3. returns true.

It catches any `std::exception` and returns false.

The caller then prints:

```text
Aucune rubrique d'aide : <topic>
```

with `index` shown for an empty topic.

### Current diagnostic trade-off

Because `print_help_topic()` catches every `std::exception`, a future renderer
failure would be presented the same way as a missing topic.

This is a current presentation-layer limitation.

## 23. Immediate development commands

Two development commands are recognized before ordinary command expansion:

```text
:cls
:print
```

### `:cls`

Calls:

```cpp
fredpp::clear_terminal()
```

### `:print`

Calls the CLI-only `print_buffer()` helper to display every line with:

```text
[buffer-name]
>line: text    current line
 line: text    other line
```

This is a development/debug rendering, not FRED P semantics.

### Exact-input detail

These two checks currently compare the whole `input` string exactly.

Leading/trailing whitespace therefore prevents these exact early matches.

## 24. Central interactive command expansion

The CLI starts with:

```cpp
make_command_input(input)
```

and replaces it with:

```cpp
expand_command_input_with_metadata(input, manager)
```

when the first non-space input byte is neither:

```text
:
"
```

Thus:

- normal FRED command lines receive central flow expansion;
- development commands do not;
- comment lines do not.

This preserves the permanent architecture:

```text
flow expansion -> FlowCharacterStream -> Lexer -> Parser
```

## 25. Comment expansion behavior

If the first non-space byte is `"`, command-input flow expansion is bypassed.

The unexpanded text still reaches CommandParser and becomes a CommentCommandNode.

This is consistent with ProcedureRunner's separate comment handling: flow
directives inside comments do not execute.

## 26. Standalone procedure `\B(buffer)` in the REPL

After command-input expansion, the CLI calls:

```cpp
procedure_runner.execute_buffer_directive(command_input)
```

before development-command dispatch.

If it recognizes `\B(buffer)`, the procedure runs immediately.

If it requests exit, the interactive loop stops.

## 27. Development-command parser

For remaining lines the CLI uses an `std::istringstream` over the **original
input**, not `command_input`.

The first whitespace-delimited word becomes the development command.

Current implemented debugging commands:

```text
:flow <buffer>
:lex <text>
:tokens <text>
:address <text>
:command <text>
:pattern <text>
```

These are FREDPP developer utilities, not historical FRED commands.

## 28. `:flow`

Creates a temporary:

```cpp
FlowEngine flow(manager)
```

and prints:

```cpp
flow.expand_buffer(name)
```

This exercises full buffer-flow expansion.

## 29. `:lex`

Passes supplied text directly to:

```cpp
Lexer::tokenize()
```

and prints for every Token:

```text
type "lexeme" @ line:column level=N
```

The helper does **not** run central command-input expansion first.

It is a raw Lexer diagnostic.

## 30. `:tokens`

Exercises TokenStream instead of Lexer::tokenize().

It repeatedly consumes through the final End token and prints the same token
metadata.

It is intended to inspect lazy TokenStream behavior from the CLI.

## 31. `:address`

Pipeline:

```text
raw diagnostic text
-> Lexer
-> TokenStream
-> AddressParser::parse()
```

It prints a compact structural description for:

```text
Absolute
Current
Last
Relative
Range
```

For Range it currently prints numeric `AstNodeKind` values of the endpoints,
not a recursive human-readable address representation.

## 32. `:command`

Pipeline:

```text
raw diagnostic text
-> Lexer
-> TokenStream
-> core CommandRegistry
-> CommandParser::parse()
```

It maps many AstNodeKind values to an English name, then prints whether an
address exists.

### Current diagnostic coverage

The mapping does not explicitly name every current command kind: for example
SystemCommand and ZapGatherCommand fall through to generic:

```text
Command
```

This is only a development-display limitation; normal parsing/execution still
supports those AST types.

## 33. `:pattern`

Passes raw text directly to PatternParser and prints:

```cpp
describe_pattern(*node)
```

It bypasses command-input flow expansion and is therefore a parser-debug tool,
not a simulation of an S/G command line.

## 34. Unknown development commands

For input whose **first byte** is `:` and which matched no known development
command, the CLI prints:

```text
Unknown development command
```

### Current whitespace asymmetry

Earlier flow-bypass detection uses the first **non-space** character.

The final unknown-development-command test uses:

```cpp
input.front() == ':'
```

Consequently, an unknown development command preceded by whitespace may not
take the same error branch and can fall into ordinary parsing.

This is a current technical edge case, not a FRED language rule.

## 35. Normal FRED parsing

For non-development, non-empty input:

```text
ExpandedCommandInput.characters
        ↓
FlowCharacterStream
        ↓
Lexer
        ↓
TokenStream
        ↓
CommandParser
```

The parser uses the long-lived core CommandRegistry created at startup.

## 36. Command chaining

The CLI loops:

```cpp
while (!tokens.eof() &&
       !execution_context.exit_requested()) {
    parser.parse_one();
    ...
}
```

Thus multiple commands on one input line are executed sequentially.

The same `ExecutionContext` is shared across the chain.

An exit request stops parsing/execution of further commands.

## 37. Normal command execution

Non-A/I/C AST nodes are passed directly to:

```cpp
command_executor.execute(*node, execution_context)
```

The CLI does not contain their semantics.

## 38. Interactive A/I/C text mode

For Append, Insert and Change the CLI prints:

```text
-- text input; finish with \F --
```

Then prompts:

```text
text>
```

Collected physical lines are stored as ordinary strings.

They are passed to:

```text
execute_append
execute_insert
execute_change
```

after the terminator.

## 39. Interactive text terminator

The current interactive CLI accepts exactly:

```text
\F
```

The comparison is case-sensitive and does not trim whitespace.

This differs from ProcedureRunner, whose text terminator accepts trimmed
`\F` or `\f`.

This is a **current front-end behavioral difference** that should be considered
before changing either side.

## 40. Flow inside interactive text blocks

Text lines collected for A/I/C are not passed through command-input flow
expansion by the CLI.

They are inserted/replaced as raw text.

This is separate from flow directives appearing in the command's arguments.

## 41. EOF during text input

If stdin ends before exact `\F`, Runtime execution is cancelled and the CLI
throws:

```text
end of input before \F; text command cancelled
```

The outer interactive catch reports it and, subject to stream EOF, the loop
ends.

## 42. Exit behavior

Runtime Q/QQ sets:

```cpp
execution_context.exit_requested()
```

The CLI checks this:

- during command chains;
- after standalone procedure-buffer directives;
- after an ordinary input line.

No separate CLI quit flag exists.

This is why `:quit` must not be reintroduced alongside historical Q.

## 43. Interactive error policy

The complete body of each REPL iteration is wrapped in:

```cpp
catch (const std::exception& error)
```

and reports:

```text
error: <what()>
```

to stderr.

The loop normally continues after recoverable input errors.

There is no catch-all `catch (...)` in the desktop REPL.

## 44. `print_buffer()`

This anonymous helper is strictly a debug view.

It writes directly to std::cout because it belongs to the CLI layer rather than
Runtime.

It does not mutate Buffer state.

## 45. `print_tokens()` / `print_token_stream()`

These helpers intentionally expose internal lexical details including:

```text
SourceLocation.flow_level
```

They are useful for verifying Flow/Lexer integration but receive raw supplied
diagnostic text when invoked through `:lex`/`:tokens`.

## 46. Version API

The header:

```text
include/fredpp/version.hpp
```

is implemented by:

```text
src/version.cpp
```

Every function returns generated constants from:

```text
VersionInfo.hpp
```

which CMake writes at configure time.

No Git subprocess is launched by these functions at program runtime.

## 47. Version source of truth

CMake defines the software version through:

```cmake
project(FREDPP VERSION ...)
```

and generates:

```text
FREDPP_VERSION_STRING
```

The developer/user documentation should therefore avoid copying a current
version number that can become stale.

The running executable exposes the authoritative build value through:

```text
?version
--version
```

## 48. Git identity generation

At CMake configure time, when Git and `.git` are available, CMake runs:

```text
git rev-parse --short=7 HEAD
git status --porcelain --untracked-files=normal
```

Generated states are:

```text
propre
modifié
inconnu
```

and a separate integer dirty flag backs `source_is_dirty()`.

These values represent **configure-time state**.

If files change after CMake configuration without reconfiguring, the executable's
generated source-state metadata does not automatically refresh.

## 49. CLI tests

The test manifest currently includes direct CLI coverage for, among other
things:

```text
cli_smoke (--version)
cli_script
cli_procedure_bootstrap
cli_bootstrap_environment
cli_user_init
cli_bootstrap_facts
cli_move_buffer
cli_flow_s_system
cli_procedure_debug
cli_flow_s_global
cli_flow_s_global_interactive
cli_flow_s_command_input
cli_jump_label
cli_numeric_jump
cli_command_chaining
cli_message_delimiters
```

These integration tests are important because they exercise the actual
executable boundary rather than only individual C++ classes.

## 50. Architectural limits

The CLI may:

- perform process/environment/bootstrap work;
- read stdin and write prompt/debug presentation to stdout/stderr;
- coordinate service objects;
- collect interactive text blocks.

The CLI should not:

- implement Buffer mutation semantics for normal FRED commands;
- duplicate Flow substitutions inside specific commands;
- bypass CommandParser for ordinary FRED grammar;
- invent another exit mechanism parallel to Q/QQ.

The stable interactive command pipeline is:

```text
raw line
  ↓
central Flow expansion
  ↓
FlowCharacterStream
  ↓
Lexer
  ↓
TokenStream
  ↓
CommandParser
  ↓
CommandExecutor
```
