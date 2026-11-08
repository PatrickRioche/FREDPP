# Windows development test: historical ZK insertion copy

`ZK` copies addressed lines from the current Buffer **after the destination
Buffer's current line**. It retains the target's old lines and never removes
source lines. `K` differs by replacing the target's entire content.

## Manual Windows 11 scenario

Run from the checkout root in PowerShell:

```powershell
.\scripts\build.bat
$env:FREDPP_INIT = ""
.\out\build\x64-Debug\Debug\fredpp.exe .\tests\fixtures\cli_zap_copy.fredpp
```

Expected exact transcript:

```text
FREDPP-ZK-SOURCE-CURRENT
beta
FREDPP-ZK-SOURCE-ALL
alpha
beta
gamma
FREDPP-ZK-DEST-CURRENT
beta
FREDPP-ZK-DEST-ALL
old-1
alpha
beta
old-2
```

The target starts with two old lines and cursor on `old-1`; after ZK,
its final order is `old-1 / alpha / beta / old-2`. Both Buffer cursors
are tested by unaddressed `P`.

## Focused tests

```powershell
ctest --test-dir .\out\build\x64-Debug -C Debug --output-on-failure -R "^(test_zap_copy|cli_zap_copy)$"
```

The CLI test disables the user init file and compares stdout exactly.
The unit test verifies source preservation, both cursors, target insertion,
same-buffer restrictions, empty-source and invalid-address failures.
The Windows/MSVC dev workflow runs both tests in Release as well.

## Source ownership

- `ZapCopyCommandNode` and `AstNodeKind::ZapCopyCommand`
- `CommandParser`: recognition of `ZK`, optional source range, required target
- `CommandExecutor::execute`: dispatch to `execute_zap_copy`
- `BufferManager::get_or_create`: target without changing selection
- `Buffer::insert_after`: retain old target content, advance its cursor
- `docs/specs/SPEC-029-ZAP-COPY.md`: contract
- `docs/fr/reference/commandes/zk.md`: historical help, unchanged

This is a Windows-focused implementation candidate; full historical
homologation and other-platform test campaigns remain deferred.
