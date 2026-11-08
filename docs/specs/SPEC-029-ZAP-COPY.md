# SPEC-029 — Historical ZK: insert-copy between buffers

**Status:** Windows 11 implementation candidate on `dev`, version `0.0.22`.
Historical conformance is not yet certified.

## Intent

`(.,.)ZK(destination)` copies the addressed lines from the selected
source Buffer into a named destination Buffer **after** the destination's
current line. It never deletes source text and never replaces destination
text. This distinguishes ZK from K (copy with destination replacement)
and M (move with destination replacement).

## Preconditions and syntax

- The optional source address follows normal address evaluation semantics.
  When omitted, it selects the source's current line.
- The destination is named explicitly in parentheses; it can be created on
  demand without selecting it.
- An empty source or invalid source address fails before destination creation.
- A same-Buffer copy is allowed **only if** the source range excludes the
  current line of that Buffer. Violations fail without mutation.
- Buffer naming follows existing FREDPP name validation.

## Observable postconditions

1. Target lines are inserted after its original current-line cursor.
   Target lines before and after the insertion remain present.
2. Target's current line becomes the last inserted line.
3. With distinct source and destination, source current line becomes the
   last **original** copied source line; source text and dirty flag stay intact.
4. With identical source and destination, the sole current line becomes
   the last inserted line.
5. The selected Buffer is not changed; the command produces no output.

Runtime snapshots the source range before mutating the target to preserve
meaning when the buffers alias. The insertion helper handles a target cursor
of zero (copy into an empty Buffer).

## Validation

- `test_zap_copy`: cross-buffer insertion, cursor/dirty invariants,
  default address, self-copy success and rejection, invalid source.
- `cli_zap_copy`: exact output from a Windows-compatible procedure
  showing source preservation and retained target lines.
- `test_command_parser`: ZK AST, address and required destination.
- CI: `.github/workflows/windows-development.yml`.

Historical source: `docs/fr/reference/commandes/zk.md`; deliberately
unchanged. Cross-platform compatibility and exhaustive historical edge cases
remain future work.
