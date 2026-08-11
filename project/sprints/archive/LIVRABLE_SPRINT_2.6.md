# Sprint 2.6 — Transfer (`T`)

## Added

- `TransferCommandNode` and `AstNodeKind::TransferCommand`.
- Parser support for `Tdestination` and `rangeTdestination`.
- Core command registry entry for `T`.
- Runtime copy-and-insert implementation.
- CLI help and startup banner update.
- Dedicated unit test `test_transfer`.
- Parser and lexer regression coverage.
- SPEC-020, EDR-004, and manual homologation assets.

## Windows validation

Run:

```powershell
cd scripts
.\rebuild.bat
```

Expected result: `17/17 tests passed`.

## Manual homologation

Start `fredpp.exe`, paste the contents of `tests/homologation/transfer.fred`, and compare printed lines with `transfer.out`.

## Suggested commit

```text
Sprint 2.6 - Implement Transfer command
```
