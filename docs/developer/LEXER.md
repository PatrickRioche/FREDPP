# Lexer, CharacterStream and TokenStream

## 1. Role in the architecture

The lexer module converts an already interpreted character stream into the
token stream consumed by FREDPP parsers.

```text
raw/direct input ───────────────┐
                               ▼
                         StringCharacterStream
                               │
FlowEngine / expanded input ───► CharacterStream
                               │
                               ▼
                              Lexer
                               │ Token
                               ▼
                           TokenStream
                               │
                               ▼
                            parsers
```

The lexer is a **syntax layer only**. It must not access `Buffer`, execute a
command, resolve an address against runtime state, or decide whether a
single-letter command mnemonic is registered.

## 2. Data types

### `SourceLocation`

Purpose: preserve the origin of a character/token.

| Field | Type | Meaning |
|---|---|---|
| `offset` | `std::size_t` | Zero-based character offset |
| `line` | `std::size_t` | One-based line number |
| `column` | `std::size_t` | One-based column number |
| `flow_level` | `std::size_t` | FRED flow-expansion level |

`SourceLocation` owns no source text.

### `CharacterInterpretation`

This metadata is produced before lexical classification.

- `Normal`: ordinary lexical rules apply.
- `Literal`: lexical structure is neutralized where applicable.
- `ForcedSpecial`: structural interpretation remains active and the metadata is
  propagated to the token.

Current lexer behavior is deliberately more precise than simply
"Literal means plain text": literal letters, digits and `_` may still form
identifiers/numbers. Literal punctuation and whitespace become `Symbol`.

### `Character`

Input value for `CharacterStream`/`Lexer`.

```cpp
struct Character {
    char value;
    SourceLocation location;
    CharacterInterpretation interpretation;
};
```

It is returned by value, so callers do not retain references to stream storage.

### `Token`

Output value of `Lexer`.

| Field | Meaning |
|---|---|
| `type` | Lexical category |
| `lexeme` | Owned token text |
| `location` | Location of first character, or end location for `End` |
| `interpretation` | Interpretation of first character |

`Token` is self-contained and does not reference the lexer or source buffer.

## 3. `CharacterStream`

`CharacterStream` is an abstract rewindable character source.

### Input

No global runtime object is passed to the interface. Implementations provide
characters that already carry location and interpretation metadata.

### Output API

| Method | Input | Output | Cursor effect |
|---|---|---|---|
| `peek(lookahead)` | relative character offset | `optional<Character>` | none |
| `consume()` | none | `optional<Character>` | +1 on success |
| `eof()` | none | `bool` | none |
| `position()` | none | absolute cursor | none |
| `rewind(position)` | absolute cursor | none | sets cursor |
| `end_location()` | none | `SourceLocation` | none |

### Limits/invariants

- EOF is represented by `std::nullopt`, not a sentinel character.
- The position exactly after the final character is valid.
- Existing implementations throw `std::out_of_range` for a rewind beyond their
  stored character count.
- Character streams are not documented as thread-safe.
- `CharacterStream` does not tokenize.

## 4. `StringCharacterStream`

`StringCharacterStream` is the direct-source implementation.

### Constructor

```cpp
StringCharacterStream(std::string_view source,
                      std::size_t flow_level = 0);
```

Inputs:

- `source`: copied into internal `std::string storage_`;
- `flow_level`: copied into every generated `SourceLocation`.

Consequences:

- the original `string_view` storage does **not** need to outlive the object;
- construction is O(n);
- metadata storage is O(n);
- `peek()` and `consume()` are O(1).

### Location rules

For `"A\nbc"`:

```text
A   offset 0 line 1 column 1
\n  offset 1 line 1 column 2
b   offset 2 line 2 column 1
c   offset 3 line 2 column 2
EOF offset 4 line 2 column 3
```

### Current limits

The implementation iterates over `char`, so positions count stored bytes/chars
as represented by the C++ string. It does not decode UTF-8 code points.

Only `'\n'` increments the line counter. Other bytes, including `'\r'`, advance
the column unless consumed later as horizontal whitespace by `Lexer`.

## 5. `Lexer`

### Ownership modes

#### Owned source

```cpp
Lexer(std::string_view source, std::size_t flow_level = 0);
```

Creates:

```text
Lexer
 ├── owns unique_ptr<CharacterStream>
 │      └── StringCharacterStream
 └── stream_ points to owned object
```

#### Borrowed source

```cpp
Lexer(CharacterStream& stream);
```

Creates:

```text
external CharacterStream
          ▲
          │ borrowed
       Lexer
```

The external stream must outlive `Lexer`.

The class is deliberately non-copyable and non-movable.

### `next()`

Input: current `CharacterStream` position.

Output: one `Token`.

Side effects: consumes enough characters to form one token, after first
discarding **Normal** horizontal whitespace (`' '`, `'\t'`, `'\r'`).

It does not discard newline.

### `tokenize()`

Consumes all remaining input and returns a vector that includes one final
`TokenType::End`.

### ASCII lexical rules

Current classification is explicitly ASCII-oriented:

- digit: `0` to `9`;
- letter: `A` to `Z` or `a` to `z`;
- identifier continuation: letter, digit or `_`;
- printable symbol range: byte 32 to 126.

A single-letter ASCII identifier becomes `TokenType::Command`.

Important: this means `Command` is only a **lexical candidate**. `Lexer` does
not know the command registry.

### Historical compact command rule

A letter immediately followed by a digit is emitted separately as a command
candidate followed by a number.

Example:

```text
M5
```

becomes:

```text
Command("M")
Number("5")
End
```

This preserves historical compact command syntax such as move-command numeric
operands.

### Literal and forced-special behavior

Literal punctuation that would normally be structural is emitted as `Symbol`.

Examples covered by tests:

```text
Literal ')'  -> Symbol(")")
Literal '\'  -> Symbol("\")
Literal ' '  -> Symbol(" ")
Literal '\n' -> Symbol("\n")
```

Literal letters/digits remain eligible for identifiers. This is necessary for
text produced by flow substitutions such as `\S`/`\L`.

`ForcedSpecial ')'` remains `RightParenthesis`, with
`Token::interpretation == ForcedSpecial`.

### Current lexer limits

- ASCII classification only.
- No semantic command validation.
- No Buffer access.
- No runtime execution.
- No address evaluation.
- No locale-sensitive character classes.
- Token interpretation stores the first character's interpretation; there is no
  per-character interpretation vector inside a multi-character token.

## 6. `TokenStream`

`TokenStream` adds lazy token caching and parser-friendly rewind.

### Constructor

```cpp
TokenStream(Lexer& lexer);
```

The lexer is borrowed and must outlive the token stream.

### Lazy cache

`peek(n)` calls `ensure(position + n)`. `ensure()` repeatedly calls
`Lexer::next()` only until the requested token exists or an `End` token has
been cached.

This lets parsers perform lookahead without tokenizing the entire input.

### End token invariant

The first `End` is cached once.

After that:

- `consume()` returns `End` without advancing;
- repeated `consume()` remains stable;
- `peek(100)` past EOF returns the same cached `End`.

### Rewind invariant

`TokenStream::rewind()` is **backward-only**.

Valid:

```cpp
auto mark = tokens.position();
// consume tokens
tokens.rewind(mark);
```

Invalid:

```cpp
tokens.rewind(position_greater_than_current);
```

The invalid case throws `std::out_of_range`.

This behavior is useful for parser rollback after a failed parse attempt.

### Reference lifetime warning

`peek()` returns `const Token&` into an internal `std::vector<Token>`. A later
operation that causes the vector to grow may reallocate it, invalidating an
older reference. Parser code should therefore use the returned reference
immediately rather than storing it across token-producing operations.

## 7. Diagnostics

`Diagnostic` is structured data; `format_diagnostic()` renders it but performs
no I/O.

Format:

```text
error at <line>:<column> (level <flow_level>): <message>
<optional source line>
<spaces>^
```

A defensive `column == 0` case avoids unsigned underflow while rendering the
caret.

## 8. Tested behavior used as documentation evidence

The current unit tests verify, among other things:

- one-based line and column calculation;
- zero-based offsets;
- propagation of `flow_level`;
- character-stream lookahead without consumption;
- valid rewind to EOF and rejection beyond stream size;
- command/number/identifier tokenization;
- compact `M5` tokenization;
- newline preservation;
- Literal punctuation and whitespace behavior;
- ForcedSpecial structural behavior;
- TokenStream lazy lookahead;
- TokenStream backward rewind;
- stable End consumption and far lookahead.

## 9. Architectural boundary

The module's responsibility ends at tokens:

```text
CharacterStream -> Lexer -> TokenStream
                           |
                           +--> parser
```

It must not grow command-specific execution logic. A future command should
normally be introduced through parser/registry/runtime layers rather than by
adding command semantics to `Lexer`.
