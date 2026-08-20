# Parser source architecture

## 1. Overview

The parser layer converts lexical tokens or delimited pattern fragments into
owned AST nodes.

```text
Lexer -> TokenStream
             |
             +-> AddressParser -> AddressNode
             |
             +-> CommandParser -> CommandNode
                      |
                      +-> AddressParser
                      +-> PatternParser -> PatternNode
                      +-> CommandRegistry
```

No parser is allowed to access `Buffer`.

## 2. `ParseError`

`ParseError` derives from `std::runtime_error` and adds a `SourceLocation`.

Inputs:

```cpp
ParseError(std::string message, SourceLocation location)
```

Outputs:

- `what()` -> message from `std::runtime_error`;
- `location()` -> structured source position.

Formatting and terminal output are separate concerns.

## 3. `AddressParser`

### Input

```cpp
AddressParser(TokenStream& tokens)
```

The TokenStream is borrowed.

### `parse()`

Parses a complete expression and requires:

```text
address [NewLine] End
```

On failure it restores the TokenStream cursor to the position observed when the
method was entered.

### `parse_prefix()`

Parses the same address grammar but does not require end-of-input. This is the
entry point used by `CommandParser`.

It also rolls back its own starting mark on failure.

### Accepted syntax

```text
number
.
$
+number
-number
single,single
```

### Numeric conversion

Numbers are converted manually to `std::size_t`.

Before:

```cpp
value = value * 10 + digit
```

the parser checks:

```cpp
value > (max - digit) / 10
```

and throws `ParseError("address number is too large", ...)` rather than
wrapping.

### Important semantic boundary

`0` is accepted and preserved. Existence and Buffer-bound validation belong to
`AddressEvaluator`/runtime.

## 4. `PatternParser`

### Input form

`PatternParser` parses a **complete delimited source fragment**, not TokenStream.

Examples:

```text
/A/
/A|B/
?hello?
!hello!
```

The direct parser accepts a symbolic delimiter provided it is not alphanumeric,
whitespace, `_`, or `\`.

Command-specific syntax may be narrower. `CommandParser` restricts G patterns
to `/` or `?`.

### Owned and borrowed state

```text
source_             string_view, borrowed
interpretations_    vector, owned
position_           cursor
flow_level_         metadata
delimiter_          current delimiter
```

If interpretation metadata is supplied and is non-empty, its size must exactly
match `source.size()` or the constructor throws `std::invalid_argument`.

### Flow metadata

`is_special(position)` returns false only for
`CharacterInterpretation::Literal`.

Therefore:

```text
Normal        -> metasyntax active
Literal       -> metasyntax neutralized
ForcedSpecial -> metasyntax active
```

This is why text produced/protected by flow expansion can safely contain
characters such as `.` or a delimiter without necessarily changing pattern
structure.

### Recursive-descent precedence

```text
alternation
    ↓
sequence
    ↓
repetition
    ↓
atom
```

This produces the expected ownership tree for grouping and repetition.

### Current pattern-language limits

The current implementation intentionally does not interpret unimplemented FRED
forms as PCRE.

SPEC-007 records later compatibility work including:

- column assertions;
- tags;
- word boundaries;
- defined patterns;
- fence;
- FRED class/case modifiers.

### Location limitation

`PatternParser::location()` returns:

```cpp
SourceLocation{position_, 1, position_ + 1, flow_level_}
```

So pattern-node locations are relative to the reconstructed pattern fragment.
The original command-line offset/column is not retained, except for flow level.

This is a **current technical limitation**, not a historical FRED rule.

## 5. `CommandParser`

### Inputs

```cpp
CommandParser(TokenStream& tokens,
              const CommandRegistry& registry)
```

Both are borrowed and must outlive the parser.

### Transactional parse

`parse()` saves:

```cpp
const auto mark = tokens_->position();
```

and catches all escaping exceptions. On failure:

```cpp
tokens_->rewind(mark);
throw;
```

The caller therefore sees either a successfully consumed command or the
original token position.

### `parse_one()`

`parse_one()` temporarily sets:

```cpp
allow_trailing_commands_ = true;
```

This permits parsing command chains from one TokenStream. The previous value is
restored on success and on failure.

### Address prefix

A command may begin with:

```text
number
.
$
+
-
```

which is delegated to `AddressParser::parse_prefix()`.

`*` is handled separately as a whole-buffer alias and is materialized as:

```text
RangeAddressNode(
    AbsoluteAddressNode(1),
    LastAddressNode
)
```

When `*` appears alone, the parser synthesizes an implicit `P`.

### Registry role

The parser resolves the final one-letter mnemonic through `CommandRegistry`.

The historical Step 7.1 document `SPEC-008` describes a smaller architecture
where behavior came entirely from registry descriptors. The current parser has
grown beyond that milestone: command-specific operand parsing is now explicitly
implemented in `CommandParser`, with descriptor parsing used as the generic
fallback for simple commands.

This is not treated as an error in this developer document; it is a distinction
between an early milestone specification and the current source.

## 6. Compact/multi-letter forms recognized by `CommandParser`

Before registry lookup, selected two-character identifiers are normalized:

```text
QQ -> Q + immediate flag
WA -> W + Ascii
WU -> W + Utf8
WB -> W + BcdUnsupported
FB -> F + Buffers
FO -> F + Options
JM -> J + newline=true
JP -> J + newline=false
Bx -> B + compact buffer x
ZG -> Z + gather mode
```

The parser is case-insensitive for these command letters.

## 7. Command-specific parser rules

### `!`

- no line address;
- requires non-empty reconstructed text;
- produces `SystemCommandNode`.

### `"`

- no line address;
- remaining line becomes `CommentCommandNode`.

### `R`

- optional filename;
- optional single insertion address;
- range insertion address rejected.

### `W` / `WA` / `WU` / `WB`

- optional address;
- optional filename;
- carries `FileWriteMode`.

`WB` is represented in AST but marked `BcdUnsupported`.

### `S`

- one symbolic delimiter;
- parses a pattern and replacement;
- optional trailing `P` sets `print_after`.

### `Q` / `QQ`

- no address;
- `QQ` sets immediate mode;
- `Q!` is explicitly rejected because external TSS command support is not
  implemented.

### `JM` / `JP`

- no address;
- accepts plain or delimited message form;
- message length over 2000 is rejected as the historical limit.

Current code recognizes only JM and JP.

### `FB` / `FO`

Only these F forms are currently implemented.

### `O`

Only these option families are currently parsed:

```text
O+M / O-M
O+I( / O-I(
```

Other options are rejected at parser level.

### `G`

- optional outer address;
- optional `~` inversion flag;
- G pattern delimiter must be `/` or `?`;
- requires a nested command;
- nested command carrying its own address is currently rejected.

### `ZG`

- no line address;
- requires destination buffer in parentheses;
- requires nested command;
- addressed nested command currently rejected.

### `Z`

Normal Z accepts at most one line address; a range is rejected.

### `B`

Accepted current forms include:

```text
B(name)
Bx
B1
B<adjacent one-character token>
```

The compact form requires the one-character buffer name to be lexically
adjacent to `B`; whitespace between `B` and that short name is therefore not
equivalent to the compact form. The parenthesized parser requires a non-empty
name.

The parser itself does not enforce the global semantic buffer-name maximum;
such validation belongs to the appropriate later layer.

### `M`

Requires destination buffer in parentheses.

### `T`

Requires a destination address. Destination ranges are rejected.

### `L`

Does not accept a line address and has an optional filename.

## 8. Reconstruction of text after lexing

Lexer discards Normal horizontal whitespace. For constructs that need textual
operands, CommandParser reconstructs gaps from token columns.

This is used for:

- buffer names;
- filenames;
- comments/system text;
- messages;
- pattern fragments;
- replacement text.

A gap is reconstructed using ordinary spaces.

Therefore original horizontal whitespace is **not byte-for-byte preserved**:
tabs or carriage returns skipped by Lexer are represented as spaces according
to location gaps.

## 9. Delimited pattern reconstruction

For G/S, CommandParser keeps
`CharacterInterpretation` metadata alongside reconstructed pattern bytes.

A candidate closing delimiter is accepted only when:

- the token is exactly that delimiter;
- its interpretation is not Literal;
- it is preceded by an even number of backslashes.

This prevents escaped or flow-protected delimiters from prematurely ending the
pattern.

## 10. Parser limits vs architectural prohibitions

### Architectural prohibitions

Parsers must not:

- inspect Buffer contents;
- evaluate an address against a Buffer;
- execute a command;
- perform runtime side effects.

### Current implementation limits

Examples:

- addressed nested commands inside G/ZG;
- Q! external TSS support;
- unimplemented F/O forms;
- fragment-relative PatternParser locations;
- horizontal-whitespace reconstruction rather than exact preservation.

These are implementation facts and should not be confused with permanent
architectural rules or with historical FRED language restrictions.

## 11. Evidence used for this documentation

Current behavior was cross-checked against:

- `src/parser/AddressParser.cpp`;
- `src/parser/PatternParser.cpp`;
- `src/parser/CommandParser.cpp`;
- `tests/test_address_parser.cpp`;
- `tests/test_pattern_parser.cpp`;
- `tests/test_command_parser.cpp`;
- `SPEC-006-ADDRESSES.md`;
- `SPEC-007-PATTERNS.md`;
- `SPEC-008-COMMAND-PARSER.md`.

When an older milestone SPEC is narrower than current code/tests, this document
describes the current implementation and explicitly notes the difference.
