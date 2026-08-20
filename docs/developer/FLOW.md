# Flow engine and command-input expansion

## 1. Purpose

FRED flow expansion is a language stage that runs before later lexical/parser
interpretation when the historical syntax permits directives in the input being
processed.

For command input the intended pipeline is:

```text
raw command
    ↓
CommandInputExpansion / FlowEngine
    ↓
ExpandedCommandInput
    ├── text
    └── InputCharacter[] metadata
            ↓
     FlowCharacterStream
            ↓
          Lexer
            ↓
       TokenStream
            ↓
       CommandParser
            ↓
           AST
```

This ordering is architecturally important.

## 2. Permanent transversal rule

Flow/substitution capability belongs to the FRED language, not to individual
commands.

Therefore, when historical syntax permits a flow directive in a command
argument:

```text
expand first -> parse the resulting command second
```

Do **not** implement equivalent substitution separately inside B, R, W, M or any
other command.

Example:

```text
B(nombuf) contains:
doc1
```

Raw command:

```text
B(\S(nombuf))
```

Central expansion produces:

```text
B(doc1)
```

Only then does `CommandParser` parse the B argument.

The same mechanism is tested in filenames:

```text
R c:/fredpp/library/\S(nombuf).fredpp
W c:/fredpp/library/\S(nombuf).fredpp
```

This is one of the foundational extensibility properties of FREDPP.

## 3. Historical representation and modern FREDPP representation

Historical FRED implementations could associate flow-control characters with
particular machine character values.

FREDPP source/procedure files are ordinary modern text.

FREDPP therefore does **not** need to reproduce historical encoded bytes merely
to preserve semantics.

Instead it carries:

```cpp
CharacterInterpretation
```

alongside each character:

```text
Normal
Literal
ForcedSpecial
```

This allows modern text representation while retaining the language distinction
required by later Lexer/PatternParser stages.

## 4. `InputCharacter`

An `InputCharacter` contains:

```cpp
char value;
size_t level;
CharacterInterpretation interpretation;
```

### `value`

Actual ordinary character byte.

### `level`

Input/flow nesting level.

Outer input starts at level 0.

Injected nested Buffers normally use higher values.

The level prevents syntactic constructs from accidentally crossing between a
parent source and an injected child source.

### `interpretation`

Carries lexical intent independently from the byte itself.

## 5. `InputSource`

`InputSource` is an abstract, stateful character producer:

```cpp
next()
description()
level()
```

`next()` returns `nullopt` permanently when exhausted.

Concrete sources are owned by `InputStack`.

## 6. `InputStack`

The stack implements nested injection:

```text
parent source
   ↓ reads...
push child
   ↓
child source
   ↓ EOF
pop child
   ↓
resume parent
```

The top source is:

```cpp
sources_.back()
```

When `next()` sees that the top source is exhausted, that source is destroyed
and reading resumes automatically from the previous one.

### Depth limit

Default:

```text
256 simultaneous sources
```

Construction with 0 is rejected.

Pushing when the stack has reached the maximum throws:

```text
maximum buffer-flow depth exceeded
```

This protects against recursive forms such as a Buffer expanding itself through
active `\B`.

## 7. `BufferInputSource`

This adapts a borrowed `Buffer` to `InputSource`.

Configuration:

```text
Buffer reference
flow level
emit newlines?
CharacterInterpretation
```

The Buffer must outlive the source.

### With `emit_newlines = true`

Each stored logical line produces:

```text
line bytes + '\n'
```

including the final logical line.

### With `emit_newlines = false`

Logical lines are concatenated with no separator.

### Interpretation

The configured interpretation applies to every emitted byte and to generated
newlines.

## 8. `FlowCharacterStream`

`FlowCharacterStream` is the adapter from expanded flow data into Lexer.

It converts:

```text
InputCharacter
```

to:

```text
Character
```

while preserving:

```text
flow level
CharacterInterpretation
```

It computes new locations over the **expanded result**:

```text
offset: 0-based
line:   1-based
column: 1-based
```

The original raw-command offsets are therefore not preserved after expansion;
locations describe the expanded stream.

`rewind(position)` accepts any position from 0 through EOF, including forward
repositioning.

## 9. `ExpandedCommandInput`

The command-input boundary uses:

```cpp
struct ExpandedCommandInput {
    std::string text;
    std::vector<InputCharacter> characters;
};
```

The invariant is:

```text
text[i] == characters[i].value
```

while `characters` additionally carries the metadata needed by Lexer.

## 10. `make_command_input()`

This helper performs **no flow expansion**.

It copies the source text and creates one metadata entry per byte:

```text
level          = 0
interpretation = Normal
```

It is useful for paths that intentionally bypass flow expansion while still
using `FlowCharacterStream`.

## 11. `expand_command_input_with_metadata()`

This is the preferred command expansion boundary.

It constructs a fresh:

```cpp
FlowEngine flow(buffers);
```

then calls:

```cpp
flow.expand_command_input_characters(source)
```

and derives `.text` from the resulting character values.

Creating a fresh engine for the operation also isolates command expansion from
stale full-flow stack state.

## 12. Current command-input directive behavior

The command-input helper has semantics specifically designed to preserve
metadata through Lexer.

### `\S(name)` — literal substitution, no newlines

The referenced Buffer's lines are concatenated:

```text
line1
line2
```

becomes:

```text
line1line2
```

Every inserted byte is:

```text
CharacterInterpretation::Literal
```

The inserted content is **not expanded again** in the same pass.

Example tested behavior:

```text
B(literal-doc) contains:
\S(other)
```

then:

```text
\S(literal-doc)
```

produces the literal text:

```text
\S(other)
```

rather than expanding `other`.

### `\L(name)` — literal substitution with newlines

Like `\S`, all bytes are Literal.

Unlike `\S`, each Buffer line contributes a generated `\n`.

Example:

```text
A\S(other)
B(weird),$
```

becomes:

```text
A\S(other)\nB(weird),$\n
```

with the backslash content and generated newlines all marked Literal.

### `\C<char>` — force following character literal

The directive marker disappears.

The following byte is emitted with:

```cpp
CharacterInterpretation::Literal
```

Example:

```text
A\C$B
```

becomes text:

```text
A$B
```

with `$` marked Literal.

This is also how a closing parenthesis can be included in a computed Buffer
name:

```text
\S(c\C)paren)
```

where `\C)` protects the first `)` from acting as the directive terminator.

### `\O<char>` — force following character special

The marker disappears.

The following byte is emitted with:

```cpp
CharacterInterpretation::ForcedSpecial
```

Example:

```text
A\O$B
```

becomes text:

```text
A$B
```

while `$` remains explicitly structural/special for downstream interpretation.

### `\\`

In the current command-input helper, the two source backslashes are preserved as
two `Normal` bytes.

This differs from full-flow `expand_input()` behavior, which collapses a
double-backslash escape to one output backslash.

This is a **current implementation distinction** and should not be casually
changed during unrelated work.

### Unknown directives

The command-input helper currently leaves non-recognized directive text as
ordinary `Normal` source characters.

## 13. Nested computed Buffer names

Command input permits nested `\S`/`\L` processing while parsing a directive
name.

Canonical tested example:

```text
B(nombuf) = doc1
B(doc1)   = BONJOUR
```

then:

```text
\S(\S(nombuf))
```

produces:

```text
BONJOUR
```

The nested first substitution computes the name of the Buffer used by the outer
substitution.

## 14. Full-flow expansion

`expand_input()` and `expand_buffer()` use `InputStack` and
`expand_current_input()`.

This path supports active nested input behavior.

### `\B(name)`

Active Buffer injection.

Configuration:

```text
new flow level
emit newlines = true
interpretation = Normal
```

Because injected characters are Normal, directives inside that Buffer can be
recognized and expanded.

Example structure:

```text
parent -> \B(child) -> child executes -> parent resumes
```

### `\S(name)`

Literal Buffer injection:

```text
emit newlines = false
interpretation = Literal
```

The Buffer is inserted literally and its logical lines are concatenated.

### `\L(name)`

Literal Buffer injection:

```text
emit newlines = true
interpretation = Literal
```

### `\C`

Consumes the marker and immediately outputs the next byte without allowing that
byte to begin another directive in the current expansion.

### `\O`

Consumes the marker and immediately outputs the next byte in this text-only
path.

The explicit ForcedSpecial distinction matters primarily in the metadata-
preserving command-input path.

### `\\`

Full flow collapses the pair to one output backslash.

### Unknown directive

Full flow preserves:

```text
\X
```

as the two characters `\` and `X`.

## 15. Input-level protection

Full-flow directives may not consume structural pieces from a different source
level.

Examples of guarded conditions include:

```text
directive letter crosses level
\C operand crosses level
\O operand crosses level
Buffer name crosses level
```

This prevents a parent source's open construct from being accidentally completed
by unrelated nested input.

## 16. Full-flow Buffer-name parsing

A directive requires:

```text
\B(name)
\S(name)
\L(name)
```

with an opening `(` at the same input level.

Names:

- must not be empty;
- must not exceed 64 characters;
- cannot cross a newline before `)`;
- cannot cross an input level.

### Nested `\S` in a name

Full-flow `parse_buffer_name()` supports nested `\S(...)`.

The referenced Buffer's lines are concatenated with no newline separators and
appended to the name.

### `\C` in a name

`\C` protects the following character.

This can make `)` part of the Buffer name instead of terminating it.

### Unknown nested escape

A non-recognized nested escape is retained in the name as:

```text
\ + directive byte
```

## 17. Expansion depth

There are two related protections.

### Full-flow nested input

`InputStack::maximum_depth()` limits simultaneously active injected sources.

### Recursive command/name expansion

The command-input recursive helper and nested name expansion track depth
explicitly and raise:

```text
maximum flow expansion depth exceeded (<limit>)
```

The default high-level limit is 256.

## 18. FlowEngine reentrancy and failure state

Full-flow operations check:

```cpp
if (!input_.empty())
    throw logic_error("flow engine is already executing");
```

A successful full expansion drains the stack.

A current technical caveat is that an exception occurring while full-flow
sources remain on the stack does not explicitly clear `input_`.

Reusing that same `FlowEngine` instance after such an exception can therefore
fail with "already executing".

High-level command-input helpers construct a fresh FlowEngine per call, avoiding
this issue for the central command-expansion path.

This is a **technical implementation limit**, not a historical FRED rule.

## 19. Buffer-name length compatibility

The Flow layer uses the same:

```cpp
limits::max_buffer_name_length == 64
```

as `BufferManager`.

The historical compatibility note remains:

```text
historical FRED: 15
FREDPP extension: 64
```

## 20. Cross-platform use

The Android native command pipeline explicitly follows the intended ordering:

```text
expand_command_input_with_metadata()
    ↓
FlowCharacterStream
    ↓
Lexer
    ↓
TokenStream
    ↓
CommandParser
```

This demonstrates that flow expansion is not a desktop-CLI-only feature.

The same central Core/Flow implementation is intended to preserve language
semantics across front ends.

## 21. Tests that define current behavior

`tests/test_flow.cpp` covers, among other things:

- plain Buffer expansion;
- active nested `\B`;
- recursive-depth protection;
- literal `\S` without newlines;
- literal `\L` with newlines;
- unknown/missing Buffer failures;
- command argument expansion for B/R/W;
- nested `\S` Buffer-name computation;
- no double expansion of Literal results;
- preservation of CharacterInterpretation metadata;
- `\C` and `\O`;
- protected `)` in computed names;
- 64-character accepted name;
- 65-character rejected name.

These tests are important regression protection for the transversal expansion
rule.

## 22. Architectural prohibitions

Flow code should not:

- parse command grammar itself;
- implement B/R/W/M/... semantic behavior;
- execute AST command nodes;
- mutate Buffer content while merely expanding it.

Conversely, command parsers should not reimplement FlowEngine substitutions.

The separation is:

```text
Flow decides characters/metadata
Lexer decides tokens
Parser decides syntax
Runtime decides execution
```
