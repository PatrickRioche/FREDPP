# SPEC-011 — Delete command execution

Status: implemented in FRED++ Step 7.3.

## Scope

Step 7.3 makes the historical `D` command executable in the same runtime as
`P` and `L`. Command letters remain case-insensitive, so `D` and `d` are
equivalent.

## Syntax

```text
D
address D
address,address D
```

Horizontal whitespace before the command is accepted by the lexer. No operand
is accepted after `D`.

## Address semantics

- With no explicit address, `D` uses the current line.
- A single address deletes one line.
- A range deletes every line from the first address through the last address,
  inclusively.
- Address evaluation is performed before buffer mutation.
- Invalid addresses and reversed ranges are rejected without changing the
  buffer.
- `D` on an empty buffer is rejected.

## Current-line semantics after deletion

The buffer primitive defines the current line after deletion:

1. If no lines remain, the current line is `0`.
2. If a line now occupies the first deleted line number, that line becomes
   current.
3. Otherwise, the new last line becomes current.

Examples:

```text
AAAA
BBBB
CCCC

2D
```

leaves `AAAA`, `CCCC`, with the new line 2 (`CCCC`) current.

```text
1,$D
```

empties the buffer and sets the current line to `0`.

## Runtime separation

`CommandParser` creates `DeleteCommandNode`. `AddressEvaluator` resolves its
optional address against the active buffer. `CommandExecutor` performs the
mutation through `Buffer::erase`. The parser does not access `Buffer` and the
runtime does not parse source text.
