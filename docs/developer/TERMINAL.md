# Terminal and help-pager architecture

## 1. Scope

Terminal-specific behavior is isolated in:

```text
include/Terminal.h
src/Terminal.cpp
```

Interactive help paging is implemented in:

```text
include/HelpPager.h
src/HelpPager.cpp
```

These files belong to the presentation/front-end layer, not `fred::runtime`.

## 2. `PagerKey`

The pager consumes a small platform-independent logical enum:

```text
PageUp
PageDown
Quit
Other
```

Platform-specific byte/key sequences are translated into this enum by
`read_pager_key()`.

HelpPager therefore does not contain Windows/POSIX input decoding.

## 3. Terminal clearing

`clear_terminal()` first attempts the native Windows Console API on Windows.

The native helper:

1. obtains stdout console handle;
2. reads screen-buffer information;
3. fills the entire console screen buffer with spaces;
4. restores the existing text attributes;
5. moves cursor to coordinate 0,0.

If any Windows step fails, execution falls through to the portable ANSI
fallback.

ANSI fallback:

```text
ESC [ 2 J
ESC [ H
```

followed by flush.

On non-Windows builds the ANSI path is used directly.

## 4. `stdin_is_terminal()`

Windows:

```cpp
_isatty(_fileno(stdin))
```

POSIX:

```cpp
isatty(STDIN_FILENO)
```

This decision is used by HelpPager to choose interactive paging.

## 5. Pager non-interactive detection

HelpPager decides interactivity from **stdin**, not stdout.

Thus redirected/non-terminal stdin causes whole-help printing even if stdout is
still attached to a terminal.

This is the current design.

## 6. Terminal height

Windows obtains visible rows from:

```text
srWindow.Bottom - srWindow.Top + 1
```

rather than the complete underlying screen-buffer height.

POSIX uses:

```cpp
ioctl(STDOUT_FILENO, TIOCGWINSZ, ...)
```

Note the mixed dependency:

```text
pager interactivity -> stdin
visible height       -> stdout
```

## 7. Row fallback

When the visible size cannot be determined:

```text
25 rows
```

is returned.

The API never intentionally returns zero.

## 8. Windows pager input

Windows uses `_getch()` so Enter is not required.

Recognized immediate keys:

```text
q
Q
ESC
```

all map to Quit.

Extended-key prefixes:

```text
0
224
```

cause a second `_getch()`.

Current scan codes:

```text
73 -> PageUp
81 -> PageDown
```

Everything else maps to Other.

## 9. POSIX raw mode

`RawTerminalMode` is an internal RAII helper.

Construction:

1. saves current termios using `tcgetattr`;
2. clears `ICANON` and `ECHO`;
3. sets:
   - `VMIN = 1`;
   - `VTIME = 0`;
4. activates raw-ish mode with `tcsetattr(TCSANOW)`.

Destruction restores the original saved termios when activation succeeded.

This protects terminal state on ordinary returns from `read_pager_key()`.

## 10. POSIX pager input

The first byte is read synchronously.

Recognized:

```text
q/Q -> Quit
ordinary non-ESC -> Other
```

Escape introduces a possible terminal escape sequence.

## 11. Escape-sequence timeout

POSIX PageUp/PageDown decoding uses `select()` with a 100000 microsecond
(100 ms) timeout for subsequent bytes.

A lone Escape whose second byte does not arrive within that timeout maps to
Quit.

Current recognized sequences:

```text
ESC [ 5 ~ -> PageUp
ESC [ 6 ~ -> PageDown
```

Malformed/incomplete sequences usually map to Other.

## 12. Input/setup failures

On POSIX, failure to activate RawTerminalMode returns Quit.

Failure of the initial blocking byte read also returns Quit.

This favors exiting the pager safely rather than propagating terminal I/O
exceptions.

## 13. HelpPager non-interactive mode

When stdin is not a terminal:

```cpp
std::cout << text;
```

is used.

If text is empty or lacks a final newline, one newline is appended.

No screen clearing or key reads occur.

## 14. HelpPager line splitting

Interactive mode copies the help into a vector of logical lines.

Trailing `\r` is removed.

If no lines are produced, one empty line is inserted so at least one page can
be displayed.

## 15. Reserved footer rows

Pager constant:

```text
footer rows = 2
```

Visible content rows are:

```text
terminal_rows - 2
```

when possible, with a minimum of one content row.

Thus even a terminal reporting one/two rows still yields a usable one-row
content page.

## 16. Page count

Page count is ceiling division:

```text
(lines + content_rows - 1) / content_rows
```

with an enforced minimum of one page.

## 17. Interactive redraw

Every pager iteration:

1. clears terminal;
2. prints the current slice;
3. fills unused content rows with blank lines;
4. prints a horizontal rule;
5. prints:
   - current page;
   - total pages;
   - PgUp/PgDn/Q help;
6. flushes;
7. reads one logical PagerKey.

The blank-row fill keeps the footer at a stable vertical location.

## 18. Navigation

PageUp at page 0 does nothing.

PageDown at final page does nothing.

Navigation does not wrap.

Other keys do nothing.

Quit clears the terminal and returns.

## 19. Direct std::cout use

HelpPager and Terminal use std::cout directly.

This is acceptable because they are CLI presentation infrastructure.

The Runtime architectural rule requiring visible command output through
`fred::Output` applies to Runtime command execution, not front-end UI primitives.

## 20. Shared terminal clear primitive

`clear_terminal()` is shared by:

```text
:cls
HelpPager
```

This prevents duplicate platform-specific terminal-clearing logic in the CLI.

## 21. Current limitations

These are presentation/portability limits, not FRED language rules:

- terminal width is not queried or used for line wrapping;
- HelpPager pages by physical rendered lines only;
- long lines may wrap visually in the terminal without pager accounting;
- ANSI clearing assumes a compatible terminal when native Windows clearing is
  unavailable;
- POSIX decoder recognizes only the specific PageUp/PageDown sequences above;
- terminal resize is read once when entering `show_paged_help()`, not on every
  page redraw.

## 22. Architectural boundary

`Terminal` should contain platform terminal primitives only.

`HelpPager` should contain paging/presentation only.

Neither should:

- parse FRED commands;
- access BufferManager;
- execute Runtime AST;
- own help-topic lookup/rendering rules.

The dependency direction is:

```text
HelpManager -> rendered text
                    ↓
               HelpPager
                    ↓
                Terminal
```
