# SPEC-020 — Transfer (`T`)

**Status:** Implemented candidate  
**Sprint:** 2.6 / SPR-0008  
**Command:** CMD-007 `T`

## Purpose

`T` copies one addressed line or range and inserts the copy after a destination address. The source lines remain unchanged.

## Syntax

```text
[address[,address]]Tdestination
```

The destination is a single address. Address `0` inserts at the start of the buffer.

## Semantics

1. Evaluate the source range in the current buffer.
2. Evaluate the destination in the original buffer.
3. Copy the source lines while preserving order.
4. Insert the copy after the destination.
5. Set the current line to the last inserted line.

Unlike `M`, a destination inside the source range is valid because no source line is removed.

## Errors

- Empty current buffer.
- Missing destination.
- Destination expressed as a range.
- Source or destination outside the buffer.

## Traceability

- AST: `TransferCommandNode`
- Parser: `CommandParser.cpp`
- Registry: `CommandRegistry.cpp`
- Runtime: `CommandExecutor.cpp`
- Unit test: `tests/test_transfer.cpp`
- Homologation: `tests/homologation/transfer.fred`
