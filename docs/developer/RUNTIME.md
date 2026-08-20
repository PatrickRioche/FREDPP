# Runtime source architecture

## 1. Purpose

The Runtime layer executes already-parsed FRED AST against editor state.

```text
CommandParser
    ↓
CommandNode AST
    ↓
CommandExecutor
    ├── AddressEvaluator
    ├── PatternMatcher
    └── ExecutionContext
          ├── BufferManager / Buffer
          └── Output
```

`ProcedureRunner` coordinates the same language pipeline for procedure buffers:

```text
procedure line
    ↓
central flow expansion
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

Runtime does **not** parse raw command grammar itself.

## 2. Milestone specification vs current source

`SPEC-009-RUNTIME-FOUNDATION.md` documents the early Step 7.2A foundation. At
that milestone, `CommandExecutor::execute()` intentionally reported every
recognized command as not implemented.

That is no longer the current implementation.

The present Runtime executes most core commands, performs file I/O, pattern
matching, global execution, procedure flow, options, facts and exit handling.

The old specification remains useful for the architectural boundary:

- Parser builds AST and never accesses editor buffers.
- Executor receives AST and does not parse command source.
- ExecutionContext borrows runtime services.
- Runtime-visible text goes through Output.

## 3. Runtime error model

### `CommandExecutionError`

Base runtime exception:

```cpp
class CommandExecutionError : public std::runtime_error
```

Use it for semantic/editor/I/O errors detected after parsing.

Examples:

```text
current buffer is empty
line address out of range: N
cannot open file: ...
no text changed
modified buffer(s): ...; use W or QQ
```

Parser syntax errors remain `ParseError`; do not blur those two layers.

### `CommandNotImplementedError`

This class stores a command name and formats:

```text
command not implemented: <name>
```

Current code search shows no active Runtime caller. It remains an explicit error
type available for future recognized-but-unimplemented commands.

This is a current **unused type**, not proof that no individual command form is
unsupported.

## 4. `Output`

`Output` is the single runtime text-output abstraction.

Primitive:

```cpp
virtual void write(std::string_view text) = 0;
```

Convenience:

```cpp
write_line(text)
```

is exactly:

```text
write(text)
write("\n")
```

### Architectural prohibition

Runtime command code must not bypass Output with direct `std::cout`.

This keeps the same executor usable from:

- CLI;
- tests;
- Android;
- other embedders.

## 5. `StringOutput`

`StringOutput` owns one accumulating `std::string`.

Operations:

```text
write()    append
content()  const reference
empty()    state query
clear()    erase accumulated output
```

It is used both by tests/embedders and internally by `ZG` to capture nested
command output.

## 6. `ConsoleOutput`

`ConsoleOutput` borrows:

```cpp
std::ostream*
```

The stream must outlive the adapter.

`write()` uses:

```cpp
ostream::write(data, size)
```

rather than formatted `operator<<`.

It does not convert stream failure state to `CommandExecutionError`.

## 7. `ExecutionContext`

`ExecutionContext` is the mutable execution state shared by commands and
procedure control.

Borrowed dependencies:

```text
BufferManager*
Output*
```

Owned execution state:

```text
counter
numeric registers
condition
input-parentheses option
monitor option
exit requested
immediate exit requested
```

### Initial values

```text
counter                     0
numeric registers           empty
condition                   false
input parentheses required  true
monitor commands            false
exit requested              false
immediate exit              false
```

## 8. Output exchange

`exchange_output(new_output)`:

1. saves the previous `Output*`;
2. switches the context to the new borrowed sink;
3. returns a reference to the old sink.

This is used by `ZG`.

The caller owns both output objects and must keep them alive.

`ZG` restores the previous sink even when nested execution throws.

## 9. Counter

The signed 64-bit counter is generic shared procedure/runtime state.

Current CommandExecutor updates it notably for:

- `R`: number of lines read/inserted;
- `W`: number of lines written;
- `G`: number of selected lines.

Procedure numeric operand `#` reads this counter.

## 10. Numeric registers

Storage:

```cpp
unordered_map<string, int64_t>
```

`set_numeric_register()` does not validate register syntax.

That validation belongs to `ProcedureRunner`.

`numeric_register(name)` returns:

```text
stored value, if present
0, if absent
```

Use `has_numeric_register()` when absence and explicit zero must be
distinguished.

## 11. Condition flag

The shared boolean condition is used by procedure conditional jumps.

Current producers include:

- successful `R` -> true;
- successful `W` -> true;
- `S` -> true when at least one substitution match exists;
- `S` no match -> false, then throws;
- system `!` -> true only when shell process status is zero;
- numeric register comparisons -> comparison result.

Note that `G` itself updates the counter, not the condition flag.

## 12. Options stored in context

Current implemented options:

```text
O+I( / O-I(
O+M  / O-M
```

They map to:

```cpp
input_parentheses_required
monitor_commands
```

The initial input-parentheses setting is true.

## 13. Exit state

`request_exit(immediate)` sets:

```text
exit_requested = true
immediate_exit_requested = immediate
```

`ProcedureRunner` and Android command loops inspect `exit_requested()` to stop
further command execution.

## 14. `AddressEvaluator`

This class is the only generic bridge between AddressNode AST and Buffer line
state.

### Null address

A null address means:

```text
current line
```

The current line must exist.

### Empty Buffer

Generic evaluation rejects an empty Buffer:

```text
current buffer is empty
```

Commands with insertion-specific empty-buffer semantics implement those
exceptions in CommandExecutor rather than weakening AddressEvaluator.

### Absolute address

The parsed absolute number must be an existing line:

```text
1..line_count()
```

Zero is rejected here.

### `.`

Resolves to:

```cpp
buffer.current_line()
```

which must exist.

### `$`

Resolves to:

```cpp
buffer.line_count()
```

### Relative `+n`

Computed from the current line, then validated as existing.

### Relative `-n`

If:

```text
distance >= current line
```

Runtime rejects before unsigned subtraction.

### Range

Both endpoints are evaluated independently.

Then:

```text
first <= last
```

is required.

### Current technical anomaly

Forward relative evaluation currently computes:

```cpp
current + distance
```

in unsigned `size_t` before range validation.

There is no explicit overflow guard. In an extreme artificial value, wraparound
could theoretically produce an apparently valid small line number.

This is a **technical implementation issue**, not FRED language behavior.

## 15. `PatternMatcher`

PatternMatcher executes PatternNode AST only. It does not parse pattern syntax
and is not PCRE.

### Match representation

```cpp
PatternMatch { start, end }
```

uses a half-open byte interval:

```text
[start, end)
```

### Search policy

`find(pattern,text,start_offset)` scans candidate starts from start_offset to
`text.size()`.

At the first start that can match:

- earliest start wins;
- if several ends are possible, the greatest end is selected.

Thus matching is effectively first-start / longest-end.

### Byte orientation

Offsets index `std::string_view` bytes.

There is no Unicode code-point traversal in PatternMatcher.

### Repetition protection

Repetition tracks unique reachable end positions.

A branch that returns the same position consumes zero characters and is not
added to the next repetition frontier. This prevents infinite loops on
zero-length repeated operands.

### Unknown AST kind

Internal matcher fallback returns no match rather than throwing.

Normal architecture prevents command/address AST from reaching this helper.

## 16. `CommandExecutor`

`CommandExecutor::execute()` dispatches using `AstNodeKind`.

It receives AST and ExecutionContext only.

It does not instantiate Lexer or CommandParser.

## 17. P — Print

Address:

```text
explicit address/range
or current line by default
```

Execution:

1. evaluate range;
2. send each Buffer line through `Output::write_line`;
3. set current line to the last printed line.

No Buffer content is modified.

## 18. D — Delete

AddressEvaluator resolves the target/default range.

Then:

```cpp
buffer.erase(first,last)
```

handles current-line repositioning and dirty state.

## 19. L — List file

Current implemented L requires an explicit filename.

Without one Runtime throws:

```text
L without a filename requires current-file support (not implemented yet)
```

This is **NON IMPLEMENTED** current-file L behavior.

With filename:

- open binary;
- read line by line;
- strip a trailing `\r` from lines;
- write each line through Output;
- report open/read errors as CommandExecutionError.

L does not load content into the current Buffer.

## 20. File decoding used by R

Read files are treated as text only.

### Binary rejection

Any NUL byte causes:

```text
binary file is not supported
```

### UTF-8 BOM

Leading UTF-8 BOM:

```text
EF BB BF
```

is removed.

A BOM makes recorded encoding UTF-8.

### Encoding detection

If every byte is <= 0x7F and there was no BOM:

```text
Ascii
```

Otherwise the complete text must pass the internal UTF-8 validator.

Invalid non-ASCII data is rejected.

### UTF-8 validation

The validator checks:

- legal leading-byte ranges;
- continuation bytes;
- overlong 3/4-byte encodings;
- UTF-16 surrogate range;
- code points above U+10FFFF.

### Line endings

If the byte stream contains at least one CRLF sequence, stored metadata becomes:

```text
CrLf
```

otherwise:

```text
Lf
```

Line splitting occurs on `\n`; one trailing `\r` is removed from each split
line.

### Final newline

Recorded as:

```cpp
!bytes.empty() && bytes.back() == '\n'
```

## 21. R — Read

Filename resolution:

```text
explicit command filename
else associated current-buffer file
else error
```

### R without address

The current Buffer must be empty.

Then Runtime:

1. reads/decodes the file;
2. replaces Buffer content with `load_file`;
3. associates the file metadata;
4. marks the Buffer clean through load_file;
5. sets counter to line count;
6. sets condition true.

### Addressed R

R acts as insertion.

On a non-empty Buffer the address must resolve to exactly one line.

On an empty Buffer, only historical `$` receives a command-specific exception:

```text
$ -> insertion position 0
```

Other addressed forms on empty Buffer fail.

Inserted file data does not replace the Buffer's existing file association.

Counter becomes number of inserted lines and condition true.

## 22. W / WA / WU / WB — Write

### WB

Runtime rejects:

```text
WB is the historical GCOS/BCD format and is not supported;
use WA for ASCII or WU for UTF-8
```

This is **historical syntax recognized, execution unsupported**.

### Filename resolution

Same principle as R:

```text
explicit filename
else Buffer associated filename
else error
```

### Empty Buffer

An empty Buffer can be written only without an address.

It creates/truncates the file and writes zero lines.

### Default range

For a non-empty Buffer with no address:

```text
1..line_count()
```

W therefore differs from generic current-line default.

### Encoding selection

```text
WA -> ASCII
WU -> UTF-8
plain W + Buffer encoding ASCII -> ASCII
otherwise -> UTF-8
```

### Validation

ASCII write rejects any byte > 0x7F.

UTF-8 write validates every selected line.

### Newline choice

Uses Buffer metadata:

```text
Lf   -> "\n"
CrLf -> "\r\n"
```

For a full-buffer write, the final line ending follows
`buffer.final_newline()`.

For a partial range, Runtime writes a newline after the final selected line as
well.

### Postconditions

Counter becomes lines written.

Condition becomes true.

Only a **full-buffer** write:

- associates the target filename;
- updates stored encoding metadata;
- marks the Buffer clean.

Partial-range W does not redefine the Buffer's associated file or clean state.

## 23. A — Append text

`execute()` itself rejects A:

```text
A requires text input mode
```

A caller must collect lines and call `execute_append()`.

Default address is historical:

```text
$
```

which maps to insertion after the current final line.

Absolute address 0 is a special insertion position accepted by A.

Ranges are rejected.

Collected lines are passed to `Buffer::insert_after`.

## 24. I — Insert text

`execute()` rejects I without text-input collection.

Default:

```text
before current line
```

implemented as insertion after:

```text
current_line - 1
```

For an empty Buffer this position is 0.

Absolute address 0 also maps to position 0.

Ranges are rejected.

## 25. C — Change text

`execute()` rejects C without text-input collection.

`execute_change()`:

1. evaluates the target/default range;
2. remembers insertion position `first - 1`;
3. erases the range;
4. inserts collected lines at that position when non-empty.

An empty text block therefore makes C behave as deletion.

## 26. M — Move to Buffer

Source range uses normal AddressEvaluator semantics.

Runtime copies the selected lines before modifying buffers.

### Destination equals source Buffer

Current behavior replaces the entire same Buffer with only the selected lines.

This is a specific implementation behavior:

```text
selected source range -> new complete content
```

### Destination is another Buffer

M semantics currently:

1. select/create destination;
2. erase all existing destination content;
3. append selected source lines;
4. reselect original source Buffer;
5. erase moved range from source.

So M **replaces**, rather than appends to, an existing destination Buffer.

## 27. T — Transfer/copy

Source address uses generic evaluation.

A destination AST is required and must be a single address.

Absolute destination 0 is allowed as an insertion position.

Runtime copies source lines, then:

```cpp
buffer.insert_after(destination, copied_lines)
```

Source lines remain in place.

T operates within the current Buffer; it is not a cross-buffer command.

## 28. S — Substitute

Address/default range uses AddressEvaluator.

For each selected line Runtime searches the full pattern repeatedly.

### Replacement syntax

Current replacement processing:

```text
&   -> complete matched text
\x  -> literal x for any following byte x
other byte -> itself
```

A trailing backslash with no following byte is copied as a normal backslash by
the current loop.

### All matches

Substitution continues through the line.

A non-zero-length match resumes at the end of the match.

A zero-length match explicitly advances through the input to avoid an infinite
loop while retaining the character at that position.

### Condition/error behavior

If no pattern match occurs in any selected line:

```text
condition = false
throw "no text changed"
```

If at least one pattern match occurs:

```text
condition = true
current line = last line on which substitution matched
```

Optional trailing P prints that final line.

### Important semantic detail

The internal `changed` flag actually means **a substitution pattern matched**.

If the computed replacement text equals the original text, Buffer::replace may
leave the modified flag unchanged, but S still treats the substitution as
successful.

The error text "no text changed" is therefore slightly broader than the exact
implementation condition.

## 29. FB / FO — Facts

### FO

Outputs current option state as:

```text
o+i( or o-i(
o+m  or o-m
```

### FB

Iterates `BufferManager::recent_names()` and prints:

```text
b(name) current,line_count [associated-file] [?]
```

`?` marks modified buffers.

Order is MRU selection order, not lexicographic name order.

## 30. O — Option

Current Runtime handles:

```text
InputParenthesis
Monitor
```

by writing the corresponding bool into ExecutionContext.

No output is generated.

## 31. Q / QQ — Quit

### Q

Before requesting exit, Runtime collects all modified Buffer names.

If any exist, it throws:

```text
modified buffer(s): name1, name2; use W or QQ
```

So normal Q protects unsaved buffers.

### QQ

Skips that modified-buffer check.

Then:

```cpp
context.request_exit(true)
```

### Current architectural consequence

Front-end execution loops should observe `exit_requested()` rather than add an
independent quit mechanism.

## 32. Z — Zap/current-line positioning

Current Z implementation evaluates one line.

A range is rejected.

It simply makes the resolved line current.

## 33. G — Global

### Default range

Unlike ordinary current-line default, G with no outer address selects:

```text
1..line_count()
```

### Current nested-command limit

Nested commands with their own address are rejected.

Only these nested command kinds are currently supported:

```text
P
D
Z
S
```

Anything else raises:

```text
G currently supports nested P, D, Z and S commands
```

This is **CURRENTLY UNSUPPORTED**, not an architectural prohibition.

### Selection

PatternMatcher tests each line.

`G~` inverts selection.

### Mutation-safe loop

Deletion adjusts the tracked final line while preserving the current loop index,
so lines that shift into the deleted position are not skipped.

### Counter

Counter becomes number of selected lines.

## 34. `!` — System command

Runtime appends:

```text
 2>&1
```

to the shell command so stderr is captured with stdout.

Platform execution:

```text
Windows -> _popen / _pclose
others  -> popen / pclose
```

Output is read in 4096-byte chunks and forwarded through Output.

Condition is true only when the returned close/status integer equals 0.

The command executes through the host shell. Callers/front ends must therefore
treat system-command input as intentionally powerful host interaction.

## 35. ZG — Zap/Gather output capture

ZG temporarily swaps ExecutionContext output to a local `StringOutput`.

It executes the nested command through a fresh stateless `CommandExecutor`.

On success or exception, the previous Output sink is restored.

Captured output is split into lines.

If no captured lines exist, destination is unchanged.

Destination Buffer is obtained with:

```cpp
get_or_create()
```

so ZG does not select it.

If destination is empty, captured lines are appended.

Otherwise they are inserted after the destination's current line.

## 36. `"` — Comment

Runtime comment execution is an intentional no-op.

No Buffer/output/context state is changed.

## 37. JM / JP — Message

Message text is already parsed/expanded before Runtime.

Runtime enforces again:

```text
maximum 2000 characters
```

This is identified in the source as a historical limit.

JM uses `write_line()`.

JP uses `write()` without newline.

## 38. B — Buffer

Short-form B is rejected at execution when:

```cpp
context.input_parentheses_required() == true
```

with:

```text
buff/reg name invalid
```

Otherwise Runtime delegates to:

```cpp
BufferManager::create_or_select()
```

Any BufferManager exception is translated to CommandExecutionError.

This includes the FREDPP 64-character buffer-name limit.

## 39. `ProcedureRunner`

ProcedureRunner coordinates:

```text
BufferManager
ExecutionContext
CommandRegistry
CommandExecutor
```

through borrowed pointers.

It owns no service.

## 40. Procedure depth

Default:

```text
256
```

`execute_buffer_impl()` rejects when:

```cpp
depth >= maximum_depth
```

with:

```text
maximum procedure buffer-flow depth exceeded
```

### Technical edge case

The constructor is `noexcept` and does not validate zero.

A runner created with maximum_depth=0 is legal to construct but every
`execute_buffer()` fails immediately at depth 0.

This is current API behavior.

## 41. Procedure buffer snapshot

`execute_buffer_impl()` copies:

```cpp
buffers_->get(buffer_name).lines()
```

into a local `vector<string>` before execution.

Therefore the procedure source being executed is a **snapshot**.

If commands modify the source Buffer during that execution, the already-running
procedure's instruction list does not change.

This is an important runtime invariant.

## 42. Procedure comments

A trimmed line beginning with `"` is considered a comment before flow expansion.

Comments use:

```cpp
make_command_input(raw_value)
```

instead of `expand_command_input_with_metadata()`.

Thus flow directives written inside a comment are not expanded.

## 43. Procedure command expansion

Every non-comment ordinary procedure line is centrally expanded using:

```cpp
expand_command_input_with_metadata(
    raw_value,
    *buffers_,
    maximum_depth_)
```

before procedure directives or CommandParser processing.

This preserves the permanent transversal rule:

```text
flow expansion -> parsing
```

also inside procedures.

## 44. Procedure `\B(buffer)`

`ProcedureRunner` recognizes a procedure-buffer directive separately from the
ordinary B command.

Accepted shape after trimming:

```text
\B(buffer)
```

The entire trimmed line must form that directive.

Empty names and malformed trailing/parenthesis structure are rejected.

Nested procedure buffers increment the procedure depth.

When monitoring is enabled, the expanded directive is echoed before execution.

## 45. Procedure labels

Definition:

```text
@(label)
```

Rules:

- non-empty;
- maximum 15 characters;
- no `(` or `)` inside.

The 15-character limit is explicitly historical.

Label matching for J is case-insensitive.

Definitions themselves execute no command.

## 46. Procedure jumps

Form:

```text
J(label)
J(label)T
J(label)F
```

No suffix means unconditional jump.

T means jump only when context.condition() is true.

F means jump only when condition is false.

### Search policy

Jump target search first scans forward from the instruction after the current
one.

If not found, it wraps and scans from procedure start up to the current
instruction.

This allows backward loops while preserving forward-search preference.

Missing label raises:

```text
? label not found
```

## 47. Numeric procedure register names

`N(name)` register names:

- must be non-empty;
- maximum 14 characters;
- cannot contain parentheses.

The 14-character limit is identified as historical.

## 48. Numeric operands

Current minimal numeric expressions accept:

```text
signed decimal integer
$  -> current Buffer line_count
.  -> current Buffer current_line
#  -> ExecutionContext counter
```

Decimal parsing uses `std::from_chars` into `int64_t`.

Out-of-range integer text is rejected.

## 49. Numeric operations

Current `N(register)` sequence recognizes:

```text
:value   assign register
=value   set condition register == value
<value   set condition register < value
>value   set condition register > value
```

After each operation:

```cpp
counter = numeric_register(register)
```

An absent register reads as zero for comparisons until assigned.

At least one operation is required.

## 50. Numeric tail restriction

After a numeric sequence on the same procedure line, the current implementation
allows only:

```text
J(label)[T|F]
```

The error message still says:

```text
Lot 4 allows only ...
```

and jump parser messages contain:

```text
in this lot
```

These are **stale development-milestone diagnostics**, not FRED language rules.

They should be cleaned in a future behavioral/diagnostic pass, not in this
documentation-only lot.

## 51. Procedure text-input A/I/C

When CommandParser returns A, I or C inside a procedure, ProcedureRunner gathers
following physical procedure lines until:

```text
\F
```

or lowercase `\f`, after trimming.

The collected text lines are passed directly to the appropriate specialized
CommandExecutor method.

### Current behavior

Text-block lines are taken from the stored procedure snapshot as raw strings.

They are not individually run through command flow expansion by
`execute_single_command()`.

If procedure EOF occurs before `\F`:

```text
end of procedure before \F; text command cancelled
```

## 52. Procedure monitor option

When `monitor_commands()` is true, ProcedureRunner echoes executed procedure
control/command text through Output.

Comments are explicitly excluded from ordinary command echo.

## 53. Procedure error context

When an ordinary procedure line throws, ProcedureRunner outputs:

```text
procedure stopped; remaining input:
<current line>
<next line>
<next line>
...
```

At most three physical lines are shown, followed by `...` when more remain.

It then throws an internal `ReportedProcedureError`.

Nested ProcedureRunner calls detect that wrapper and do not print the context a
second time.

## 54. Loading a procedure file

`load_and_execute_file(filename, buffer_name)`:

1. saves current Buffer name;
2. create/selects procedure Buffer;
3. requires it to be empty, clean and unassociated;
4. builds an R AST and executes normal Read semantics;
5. restores the previously selected Buffer;
6. executes the loaded procedure Buffer.

If the read fails, previous Buffer selection is restored before rethrow.

### Selection after procedure execution

The previous Buffer is restored **before** procedure execution starts.

Commands executed by the procedure can subsequently change the selected Buffer;
there is no final automatic restoration after the procedure ends.

## 55. Current implementation limits

These are not architectural prohibitions:

- L without explicit filename/current-file behavior not implemented;
- WB GCOS/BCD output unsupported;
- G supports only nested P/D/Z/S and no addressed nested commands;
- Procedure N language is a minimal subset;
- same-line tail after N is limited to J(label)[T|F];
- some milestone wording remains in diagnostics;
- PatternMatcher is byte-oriented;
- relative forward address addition lacks explicit overflow detection;
- ProcedureRunner maximum_depth=0 is constructible but unusable.

## 56. Architectural prohibitions

Runtime should not:

- tokenize raw command source in CommandExecutor;
- rebuild Command AST from strings inside execution helpers;
- bypass Output for runtime-visible normal command output;
- duplicate command-input Flow expansion command by command.

Parser should not:

- access Buffer state;
- perform file I/O;
- execute commands.

The stable responsibility chain is:

```text
Flow -> Lexer -> Parser -> AST
                    ↓
                Runtime
                    ↓
         state / I/O / Output
```

## 57. Source files covered

Public interfaces:

```text
include/fred/runtime/AddressEvaluator.hpp
include/fred/runtime/CommandExecutionError.hpp
include/fred/runtime/CommandExecutor.hpp
include/fred/runtime/ConsoleOutput.hpp
include/fred/runtime/ExecutionContext.hpp
include/fred/runtime/Output.hpp
include/fred/runtime/PatternMatcher.hpp
include/fred/runtime/ProcedureRunner.hpp
```

Implementations reviewed:

```text
src/runtime/AddressEvaluator.cpp
src/runtime/CommandExecutor.cpp
src/runtime/ConsoleOutput.cpp
src/runtime/ExecutionContext.cpp
src/runtime/PatternMatcher.cpp
src/runtime/ProcedureRunner.cpp
```

The two large orchestration implementations (`CommandExecutor.cpp` and
`ProcedureRunner.cpp`) are deliberately left behaviorally untouched in this
documentation lot. Their detailed current behavior is documented here instead
of replacing tens of kilobytes only to add commentary.
