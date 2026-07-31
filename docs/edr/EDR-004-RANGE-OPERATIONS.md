# EDR-004 — Range operations

**Status:** Accepted incrementally

## Decision

Commands that manipulate line ranges must share consistent buffer semantics. Sprint 2.6 introduces `T` using the same validated range evaluation and insertion primitives already used by `M`.

A separate public `RangeOperations` class is deferred until at least two commands require additional shared algorithms beyond `AddressEvaluator`, `Buffer::erase`, and `Buffer::insert_after`. This avoids premature abstraction while preserving a clear future extraction point.

## Consequences

- `M` and `T` use identical address and insertion conventions.
- Destination `0` means insertion before the first line.
- The current line is the final inserted line.
- A dedicated range service may be extracted later without changing command syntax or AST contracts.
