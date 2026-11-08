# Windows development test: historical R file-list fallback

The historical `R` command attempts file names in order until one can be
read. This incremental implementation preserves the existing single-file
behavior, including paths containing spaces: the complete operand is tried
first, then whitespace-separated alternatives.

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
previous ASCII/UTF-8, CRLF/LF, and W file I/O behavior. A file name **inside
a list** cannot yet contain spaces; see `docs/specs/SPEC-026-READ-WRITE.md`.

CI Windows/MSVC: [run 42468744288](https://github.com/PatrickRioche/FREDPP/actions/runs/42468744288) passed with all 16 Release checks and this Debug file-I/O check (1/1). Local Windows 11 validation was confirmed by the user on 8 November 2026: `test_file_io` passed (1/1) in 0.90 s, with total CTest time 1.53 s.

This is a focused Windows regression, not historical multi-platform
homologation. The `WX` block-statistics behavior remains deferred until the
historical block definition can be established.
