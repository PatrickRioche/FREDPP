# Windows development test: historical R file-list fallback

The historical `R` command attempts file names in order until one can be
read. This incremental implementation preserves the existing single-file
behavior, including unquoted paths containing spaces: the complete unquoted
operand is tried first, then whitespace-separated alternatives. Quoting an
individual alternative allows embedded spaces:

```fred
R "missing candidate.txt" "available candidate.txt"
1R "missing candidate.txt" "available candidate.txt"
```

## Run on Windows 11

From the `dev` checkout root in PowerShell:

```powershell
git pull --ff-only origin dev
.\scripts\build.bat
.\scripts\test.ps1 -R "^test_file_io$"
```

The regression checks three focused cases:

1. An unavailable first path followed by a readable second path: the second
   file is loaded and becomes the file association for unaddressed `R`.
2. Addressed `R` inserts the selected file's lines but preserves the
   destination's existing file association and modification state.
3. When every path is unreadable, no buffer mutation occurs.

The existing test also covers an entire filename containing spaces and the
previous ASCII/UTF-8, CRLF/LF, and W file I/O behavior. New regression cases
cover quoted alternatives with spaces, priority of the first readable file,
addressed insertion, all-file failure and unterminated quotes. See
`docs/specs/SPEC-026-READ-WRITE.md`.

CI Windows/MSVC: [run 42469707212](https://github.com/PatrickRioche/FREDPP/actions/runs/42469707212) passed all 16 Release checks and the Debug file-I/O test (1/1), including the new quoted-list cases. The preceding unquoted-list version was confirmed locally on Windows 11 on 8 November 2026 (`test_file_io` 1/1, 0.90 s; CTest total 1.53 s). Local confirmation of the **new quoted-list cases** remains pending.

This is a focused Windows regression, not historical multi-platform
homologation. The `WX` block-statistics behavior remains deferred until the
historical block definition can be established.
