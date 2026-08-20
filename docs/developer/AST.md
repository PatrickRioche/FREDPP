# AST source architecture

## 1. Purpose

The FREDPP AST is the ownership boundary between parsing and later execution.

```text
TokenStream
    ↓
parsers
    ↓
AstNode
 ├── AddressNode
 ├── PatternNode
 └── CommandNode
    ↓
runtime/evaluators
```

AST nodes retain syntax and operands. They do **not** read or mutate a Buffer.

## 2. Root type: `AstNode`

Every node exposes:

```cpp
AstNodeKind kind() const noexcept;
SourceLocation location() const noexcept;
```

`kind()` is a stable syntactic discriminator. `location()` is the starting
source location captured during parsing.

`AstNodeKind` currently covers address nodes, pattern nodes and all command-node
types produced by the parser.

## 3. Address hierarchy

```text
AstNode
└── AddressNode
    ├── AbsoluteAddressNode
    ├── CurrentAddressNode
    ├── LastAddressNode
    ├── RelativeAddressNode
    └── RangeAddressNode
```

### Absolute address

Input represented:

```text
0
1
125
```

Stored value:

```cpp
std::size_t line_;
```

Important: `0` is preserved by the parser. The AST does not decide whether a
line exists.

### Current address

`.` stores no runtime line number. It records only its source location.

### Last address

`$` similarly records syntax only.

### Relative address

```text
+3 -> Forward, distance 3
-2 -> Backward, distance 2
```

No Buffer boundary calculation occurs in the node.

### Range address

```text
1,$
.,+4
```

Ownership:

```text
RangeAddressNode
 ├── unique_ptr<AddressNode> first_
 └── unique_ptr<AddressNode> last_
```

The constructor does not check pointers for null, but the getters dereference
them. Parser-produced ranges therefore rely on the invariant that both children
are non-null.

## 4. Pattern hierarchy

```text
PatternNode
├── LiteralPatternNode
├── AnyCharacterPatternNode
├── AnchorPatternNode
├── SequencePatternNode
├── AlternationPatternNode
├── RepetitionPatternNode
├── CharacterClassPatternNode
└── GroupPatternNode
```

### Ownership

Composite pattern nodes own their children:

- sequence -> vector of `unique_ptr<PatternNode>`;
- alternation -> vector of `unique_ptr<PatternNode>`;
- repetition -> one `unique_ptr<PatternNode>`;
- group -> one `unique_ptr<PatternNode>`.

No shared ownership is introduced.

### Character classes

A class entry uses:

```cpp
struct CharacterClassRange {
    char first;
    char last;
};
```

One literal character is represented by `first == last`.

The AST itself does not reorder, normalize or validate range direction.

## 5. Command hierarchy

`CommandNode` owns an optional address:

```text
CommandNode
 ├── unique_ptr<AddressNode> address_   (optional)
 └── SourceLocation location_
```

`address()` returns a non-owning pointer. `has_address()` is the explicit null
test.

### Simple command nodes

These currently carry no operand beyond the optional base address:

- `PrintCommandNode`
- `DeleteCommandNode`
- `AppendCommandNode`
- `InsertCommandNode`
- `ChangeCommandNode`
- `ZapCommandNode`

### Operand-owning nodes

| Node | Additional owned/value data |
|---|---|
| `ListCommandNode` | optional filename |
| `MoveCommandNode` | destination buffer name |
| `TransferCommandNode` | destination AddressNode |
| `BufferCommandNode` | buffer name + short-form flag |
| `GlobalCommandNode` | pattern + inverted flag + nested command |
| `ZapGatherCommandNode` | buffer name + nested command |
| `SystemCommandNode` | system command text |
| `SubstituteCommandNode` | pattern + replacement + print-after flag |
| `ReadCommandNode` | optional filename |
| `WriteCommandNode` | optional filename + FileWriteMode |
| `CommentCommandNode` | text |
| `MessageCommandNode` | message + newline flag |
| `FactsCommandNode` | FactsKind |
| `OptionCommandNode` | OptionKind + enabled flag |
| `QuitCommandNode` | immediate flag |

## 6. Non-null child invariants

Several getters dereference `unique_ptr` members:

- `RangeAddressNode::first()/last()`;
- `RepetitionPatternNode::operand()`;
- `GroupPatternNode::expression()`;
- `GlobalCommandNode::pattern()/nested_command()`;
- `ZapGatherCommandNode::nested_command()`;
- `SubstituteCommandNode::pattern()`.

Constructors currently do not throw on null. Correct parser/runtime construction
therefore relies on a non-null invariant.

This is an **implementation invariant**, not a runtime validation API.

## 7. Value enums carried by command AST

### `FileWriteMode`

```text
Preserve
Ascii
Utf8
BcdUnsupported
```

`BcdUnsupported` is deliberately representable in the AST: syntax recognition
and execution support are separate questions.

### `FactsKind`

```text
Buffers
Options
```

### `OptionKind`

```text
InputParenthesis
Monitor
```

## 8. Architectural limits

AST classes do not:

- parse source text;
- query CommandRegistry;
- evaluate addresses;
- inspect the active Buffer;
- execute commands;
- perform file I/O;
- match patterns.

This separation is central to the FREDPP pipeline.
