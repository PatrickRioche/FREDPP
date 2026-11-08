# Windows verification — historical FF file facts

The development build **v0.0.25** adds the historically referenced `FF`
command (see the unchanged
[`ff.md`](../fr/reference/commandes/ff.md)). It lists every buffer that
has an associated file, even when that buffer has never been selected.

## Command contract under test

- `FF` and `ff` both work; `FF` accepts no address or parameter.
- Each line has the form `b(buffer-name) associated-path`.
- A modified buffer gets a trailing ` ?` marker.
- Buffers without an associated file do not appear.
- Ordering is lexicographical by buffer name, for deterministic results.
- The command does not alter buffer selection, cursor, counter, condition
  or modification flags. Existing `FB` and `FO` behavior remains unchanged.
- `?ff` is embedded from the historical reference at CMake configure/build
  time without changing that Markdown source.

The unit tests cover all the above behaviors. This targeted Windows
engineering validation does **not** amount to full historical certification.

## Windows 11 / PowerShell

From the FREDPP repository root on branch `dev`:

```powershell
git pull --ff-only origin dev
.\scripts\build.bat
.\scripts\test.ps1 -R "^(test_facts|test_help_manager)$"
& "$env:LOCALAPPDATA\FREDPP\build\x64-Debug\Debug\fredpp.exe" --version
```

Expected: CTest reports **2/2 tests passed**, and the executable reports
`FREDPP v0.0.25`. A clean `dev` state is not implied by a successful build:
the `--version` output reports the working-tree modification flag separately.

The PowerShell-native test entry point is required for filters containing
`|`; use `scripts/test.ps1`, not `scripts/test.bat`, for such expressions.
Build objects remain under `%LOCALAPPDATA%\FREDPP\build\x64-Debug` by
default, outside the Google Drive-synchronized source folder.
