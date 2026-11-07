# SPEC-028 — Historical K copy-to-Buffer command

**Status:** Implemented development candidate; historical homologation deferred

**Primary reference:** `docs/fr/reference/commandes/k.md` (unchanged)

## Syntax

```fred
[address[,address]]K(destination)
```

An omitted address selects the source Buffer's current line. A range copies
all addressed lines in order. The destination is the named Buffer.

## Expected semantics

1. Resolve the source range before modifying either Buffer.
2. Snapshot the selected lines to support source/destination aliasing safely.
3. Replace (not append to) the destination's prior text with those lines.
4. Set the destination current-line cursor to its last copied line.
5. Preserve the source text and its modified flag. When the destination is
   different from the source, set the source current line to the last copied
   source line. The selected Buffer remains the source.
6. When destination equals source, the snapshot replaces the Buffer's entire
   text; its cursor points to the final copied line.
7. K writes nothing to Output. Invalid source ranges leave both Buffers
   unchanged and do not create a destination.

## Implementation boundaries

- `CopyCommandNode` owns the optional address and destination name.
- `CommandParser` requires a parenthesized destination.
- `AddressEvaluator` provides the shared range rules.
- `CommandExecutor::execute_copy` owns Buffer mutation.
- `BufferManager::get_or_create` avoids selecting the destination.
- `tests/test_copy.cpp` and `tests/test_cli_copy.cmake` validate the
  isolated and end-to-end behavior on Windows/MSVC.

The historical reference mentions adjacent text segments with no carriage
return. FREDPP's present Buffer model stores separate logical lines and does
not expose an independent per-line CR flag; broader fidelity is deferred.

No changes to M, T, Flow, or non-Windows front ends are part of this lot.
