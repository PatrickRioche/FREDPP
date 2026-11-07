# Windows development test: historical K copy

Historical `K` copies addressed lines to a designated Buffer, replacing the
destination's previous content. Unlike `M`, it leaves the source text intact.

## Manual Windows 11 scenario

From the repository root in PowerShell:

```powershell
.\scripts\build.bat
$env:FREDPP_INIT = ""
.\out\build\x64-Debug\Debug\fredpp.exe .\tests\fixtures\cli_copy.fredpp
```

Expected transcript:

```text
FREDPP-K-SOURCE-CURRENT
beta
FREDPP-K-SOURCE-ALL
alpha
beta
gamma
FREDPP-K-DEST-CURRENT
beta
FREDPP-K-DEST-ALL
alpha
beta
```

The destination begins with an obsolete line; its absence proves replacement,
not append. Both Buffer cursors are checked by an unaddressed `P`.

## Automated focused checks

```powershell
ctest --test-dir .\out\build\x64-Debug -C Debug --output-on-failure -R "^(test_copy|cli_copy)$"
```

The CLI CTest executes the exact fixture with user init disabled and compares
stdout exactly. The unit test verifies the source's unchanged modification
flag, self-target copy, empty source and invalid address errors.

## Ownership and boundaries

- AST: `CopyCommandNode` and `AstNodeKind::CopyCommand`
- Registry/parser: `CommandRegistry`, `CommandParser`
- Runtime: `execute_copy` in `CommandExecutor.cpp`
- Historical manual: `docs/fr/reference/commandes/k.md` (untouched)
- Design contract: `docs/specs/SPEC-028-COPY.md`

This does not claim full historical compatibility. It is a Windows-focused
implementation candidate with targeted regressions.
