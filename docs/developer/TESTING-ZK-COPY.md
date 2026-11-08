# Windows development test: historical ZK insertion copy

`ZK` copies addressed lines from the current Buffer **after the destination
Buffer's current line**. It retains the target's old lines and never removes
source lines. `K` differs by replacing the target's entire content.

## Manual Windows 11 scenario

Run from the checkout root in PowerShell:

```powershell
.\scripts\build.bat
$env:FREDPP_INIT = ""
$buildDir = if ($env:FREDPP_BUILD_DIR) { $env:FREDPP_BUILD_DIR } else { Join-Path $env:LOCALAPPDATA 'FREDPP\build\x64-Debug' }
& "$buildDir\Debug\fredpp.exe" .\tests\fixtures\cli_zap_copy.fredpp
```

### If MSVC Debug linking fails with LNK1163

On one Windows 11 machine, the pre-existing build folder under a Google Drive
sync directory produced COMDAT-related `LNK1163` failures. A fresh build in an
external folder succeeded. This does **not** prove that Google Drive was the
cause: stale objects or cache may also explain the difference.

The development build script now defaults to a persistent directory under
`%LOCALAPPDATA%`. For a different location such as
`C:\dev\FREDPP-build-debug`, set `FREDPP_BUILD_DIR`. Avoid `%TEMP%` for build
outputs because MSBuild reports `MSB8029` there:

```powershell
New-Item -ItemType Directory -Path 'C:\dev' -Force | Out-Null
$buildDir = 'C:\dev\FREDPP-build-debug'
cmake -S . -B $buildDir -G 'Visual Studio 18 2026' -A x64 -DFREDPP_BUILD_TESTS=ON
cmake --build $buildDir --config Debug --parallel 1 --target fredpp test_zap_copy
& "$buildDir\Debug\fredpp.exe" .\tests\fixtures\cli_zap_copy.fredpp
ctest --test-dir $buildDir -C Debug --output-on-failure -R '^(test_zap_copy|cli_zap_copy)$'
```

The external directory is for generated build files only. Inspect an existing
folder before reusing it; do not remove project source files or untracked logs.

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
.\scripts\test.ps1 -R "^(test_zap_copy|cli_zap_copy)$"
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
