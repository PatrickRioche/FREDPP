# Windows verification — historical equals line-number command

Development build **v0.0.26** adds the historical special command `=`
(see the unchanged
[`equals.md`](../fr/reference/commandes/equals.md)). The reference
prints the current line number and gives `$=` as an addressed example.

## Behavior covered by regression tests

- `=` prints the current line number without moving the cursor.
- `2=` prints `2`, and `$=` prints the last existing line's number.
- `=` on an empty buffer prints the initial cursor position `0`.
- Only an optional **single** line address is accepted; ranges and extra
  operands are rejected.
- An explicit out-of-range line number fails without changing editor state.
- The buffer selection, cursor, modified flag, counter and condition stay
  unchanged.
- `?=` opens the embedded historical Markdown reference.
- Previous commands and version-count guards remain covered by regression
  tests. No complete historical compatibility certification is claimed.

A focused CLI fixture prints:

```text
FREDPP-EQUALS-BEGIN
1
3
2
1
FREDPP-EQUALS-END
```

The trailing `1` confirms that `2=` did not change the line selected before
the query.

## Windows 11 / PowerShell

From the repository root on branch `dev`:

```powershell
git pull --ff-only origin dev
.\scripts\build.bat
.\scripts\test.ps1 -R "^(test_line_number|cli_line_number|test_help_manager)$"
& "$env:LOCALAPPDATA\FREDPP\build\x64-Debug\Debug\fredpp.exe" --version
```

Expected: **3/3 tests passed** and `FREDPP v0.0.26`.
Use `scripts/test.ps1` for PowerShell regex alternation with `|`; calling
`test.bat` directly with such a filter can misinterpret the pipe.
The generated Debug build lives outside Google Drive at
`%LOCALAPPDATA%\FREDPP\build\x64-Debug` by default.

Two `android/hs_err_pid*.log` / `replay_pid*.log` files were observed as
untracked JVM diagnostic logs on the user's workstation during v0.0.25.
They are not tracked source changes and were not deleted by this lot.
