# Help subsystem architecture

## 1. Scope

The help subsystem spans three layers:

```text
docs/fr/... Markdown source
        ↓
CMake configuration
        ↓
generated EmbeddedHelp.hpp/.cpp
        ↓
HelpManager
        ↓
terminal renderer
        ↓
HelpPager
```

The build never edits the source Markdown help pages.

## 2. Embedded help generation

`CMakeLists.txt` defines the ordinary implemented help topics:

```text
a b c d fb fo g i jm jp l m p q r s t w z zg
```

These are sourced from:

```text
docs/fr/reference/commandes/<topic>.md
```

CMake verifies every listed file exists.

## 3. Special embedded topics

The generated help table also contains special entries:

```text
index
:
!
*
"
oi(
om
procedure
wu
```

This makes 9 special entries in addition to the normal implemented command
topics.

The main `?` index is intentionally not identical to `HelpManager::topics()`.

For example, current tests verify that `wu` and `procedure` exist as topics but
are not listed in the main FRED command index.

## 4. Generated files

CMake writes:

```text
<build>/generated/EmbeddedHelp.hpp
<build>/generated/EmbeddedHelp.cpp
```

The generated API is:

```cpp
const std::string_view* find(std::string_view topic) noexcept;
std::vector<std::string> topics();
```

The generated table is a `constexpr std::array` of topic/content pairs.

Lookup currently performs a linear scan.

## 5. Raw-string safety

Every Markdown page embedded into generated C++ is checked for collision with
the reserved raw-string delimiter:

```text
)FREDPP_HELP"
```

If a source page contains that sequence, CMake configuration fails rather than
generate invalid C++.

## 6. Source Markdown authority

The build comments explicitly state that files under:

```text
docs/fr/reference/commandes
```

are source documentation and are never modified by the build.

HelpManager::load() therefore exposes embedded copies of those Markdown pages,
not a rewritten source file.

## 7. `HelpManager`

HelpManager has no data members.

Every lookup delegates to the generated `embedded_help` namespace.

This makes the class effectively stateless/read-only.

## 8. Topic normalization

`normalize_topic()`:

1. trims `space`, tab, CR and LF from both ends;
2. returns `index` for all-whitespace input;
3. lowercases the remaining bytes;
4. maps `h` and `help` to `index`.

Examples:

```text
""        -> index
" HELP "  -> index
"  h  "   -> index
"B"        -> b
"Z"        -> z
```

The current topic set is ASCII/symbol-oriented, so byte-wise lowercase is
sufficient for existing keys.

## 9. Existence and raw loading

`exists(topic)` normalizes then calls:

```cpp
embedded_help::find()
```

`load(topic)` returns a newly allocated `std::string` copy.

Missing topic error:

```text
Aucune rubrique d'aide : <normalized>
```

## 10. Raw vs terminal help

Two distinct APIs exist intentionally.

### `load()`

Returns raw embedded Markdown.

This is useful for tests, alternate renderers or embedders.

### `load_for_terminal()`

Equivalent to:

```text
load()
↓
render_markdown_for_terminal()
```

The Markdown source remains untouched.

## 11. Terminal renderer purpose

`render_markdown_for_terminal()` is a small purpose-built formatter for the
project's help pages.

It is **not** a complete CommonMark/Markdown implementation.

Its goal is readable, deterministic plain terminal text.

## 12. Input line normalization

The renderer reads Markdown line by line.

A trailing `\r` is removed, allowing CRLF source/content to render like LF.

## 13. Heading rendering

Supported heading prefixes:

```text
#
##
###
```

Level 1 becomes:

```text
============================================================
UPPERCASE HEADING
============================================================
```

Levels 2 and 3 use:

```text
UPPERCASE HEADING
------------------------------------------------------------
```

The implementation currently renders levels 2 and 3 with the same visual rule.

## 14. Heading uppercase

ASCII bytes are uppercased with `std::toupper`.

A fixed replacement table handles common French lowercase accented letters:

```text
à â ä ç é è ê ë î ï ô ö ù û ü
```

before ASCII uppercasing.

This is intentionally limited, not a general Unicode case-mapping engine.

## 15. Display-width approximation

Table layout uses `display_width()`.

It counts bytes that are not UTF-8 continuation bytes, effectively approximating
one display column per Unicode code point.

This works acceptably for ordinary French accents.

It does not account for:

- East Asian wide characters;
- combining marks;
- grapheme clusters;
- terminal-specific width rules.

This is a presentation limitation, not a text corruption issue.

## 16. Inline Markdown stripping

Current renderer recognizes/removes:

```text
[label](target) -> label
`code`          -> code
**bold**        -> bold
__bold__        -> bold
```

The renderer is intentionally simple.

It does not claim complete nested/escaped Markdown semantics.

## 17. Fenced code blocks

A line beginning, after trim, with:

```text
```
```

toggles code-block state.

On opening, any suffix becomes an uppercase language label.

Examples:

```text
```fred
```

renders:

```text
[FRED]
------------------------------------------------------------
```

An empty language label renders:

```text
[EXEMPLE]
```

Code lines are indented by four spaces.

## 18. Markdown tables

A table is recognized when:

1. current line is a pipe-delimited row;
2. next line is another pipe-delimited row;
3. the second row consists only of separator syntax using dash, colon and
   whitespace, with at least one dash per cell.

Cells are trimmed and inline Markdown is stripped.

Column widths are computed across all data rows.

The Markdown separator row itself is replaced by a generated plain-text rule.

## 19. Table limitations

The current parser expects simple:

```text
| cell | cell |
```

rows.

Escaped/nested pipes and full Markdown table grammar are not implemented.

That is acceptable for the controlled project help corpus but should not be
mistaken for a general Markdown-table parser.

## 20. Other Markdown forms

Current renderer also handles:

```text
> quote
- bullet
blank line
ordinary paragraph
```

Blockquotes are indented.

Bullets become:

```text
  - item
```

Ordinary lines have the supported inline Markdown markers removed.

## 21. Blank-line compaction

`collapse_blank_lines()` removes repeated blank lines.

The result is stripped of trailing newlines and then receives one final newline.

Therefore rendered output is normalized to a terminal-friendly final newline.

## 22. `topics()`

HelpManager retrieves the generated list and sorts it lexicographically.

Tests explicitly verify sorted order.

This list includes special/internal help keys that the main index may choose not
to display.

## 23. Main help index

CMake constructs the `index` page programmatically rather than loading an
`index.md`.

It includes the ordinary implemented command list and selected historical help
aliases/options.

Current index text includes, among other entries:

```text
?
?!
?oi(
?om
?"
?*
```

The source test also verifies that the index does not expose internal-style
entries such as `?index` or `?:`.

## 24. FREDPP special help `?:`

Topic:

```text
:
```

comes from:

```text
docs/fr/fredpp/commandes-speciales.md
```

The CLI reaches it through:

```text
?:
```

Current tests explicitly verify this rendered help contains development
commands including:

```text
:print
:flow <buffer>
:pattern <modèle>
:cls
```

and does **not** contain the removed `:quit`.

## 25. `?version`

Version help is not an embedded Markdown topic.

The desktop CLI intercepts:

```text
?version
```

and prints generated build identity directly.

This prevents a stale hard-coded current version from living in user help.

## 26. `?wu` and `?procedure`

These are real embedded topics sourced from FREDPP-specific Markdown:

```text
docs/fr/fredpp/extension-wu.md
docs/fr/fredpp/procedures.md
```

They are deliberately available despite not appearing in the ordinary command
index list.

## 27. Help test contract

`test_help_manager.cpp` verifies:

- topic aliases/normalization;
- all important embedded topics;
- raw Markdown remains raw through load();
- terminal Markdown conversion;
- headings;
- tables;
- fenced code;
- compatibility sections;
- special-command help;
- absence of `:quit`;
- sorted topic enumeration;
- missing-topic exception behavior.

This test is the strongest executable specification of current help behavior.

## 28. Architectural boundary

HelpManager may:

- locate embedded help;
- normalize topic keys;
- render controlled Markdown to plain terminal text.

It should not:

- execute FRED commands;
- mutate Buffers;
- query Runtime state for command semantics;
- hard-code a separate current software version.

HelpPager/Terminal own interactive presentation behavior.

CMake owns selection/embedding of source Markdown.
