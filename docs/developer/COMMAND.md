# Command descriptors and registry

## 1. Purpose

The command module provides the registry boundary between a command mnemonic
and the metadata/construction callback associated with it.

```text
source text
   ↓
Lexer
   ↓
TokenStream
   ↓
CommandParser ────────────────┐
   │                          │
   │ lookup                   ▼
   └────────────────> CommandRegistry
                              │
                              ▼
                       CommandDescriptor
                              │
                              ▼
                    CommandParseFunction
```

The registry does **not** replace `CommandParser`. It identifies registered
commands and provides a common construction hook.

## 2. `CommandParseFunction`

Type:

```cpp
using CommandParseFunction =
    std::function<std::unique_ptr<CommandNode>(
        std::unique_ptr<AddressNode>,
        SourceLocation)>;
```

### Inputs

`std::unique_ptr<AddressNode> address`

- optional parsed line address;
- ownership is transferred into the callback;
- may be null.

`SourceLocation location`

- location of the command mnemonic/symbol;
- passed by value.

### Output

```cpp
std::unique_ptr<CommandNode>
```

The caller receives exclusive ownership of the returned AST.

### Important current limitation

The callback type originated when command construction was simpler. The current
`CommandParser` now parses many command-specific operands itself.

Therefore a descriptor callback is **not guaranteed to be a full parser or to
construct a fully populated node when invoked directly**.

For real command source text, use:

```text
Lexer -> TokenStream -> CommandParser
```

rather than:

```text
registry.find(mnemonic)->parse(...)
```

as a substitute for parsing.

## 3. `CommandDescriptor`

Fields:

| Field | Type | Meaning |
|---|---|---|
| `mnemonic` | `char` | exact registry key |
| `name` | `std::string` | descriptive metadata |
| `parse` | `CommandParseFunction` | AST construction hook |

### Validation boundary

`CommandDescriptor` itself is a passive struct. Validation occurs only when it
is passed to `CommandRegistry::register_command()`.

The registry rejects:

- NUL mnemonic;
- empty name;
- empty `std::function`;
- duplicate exact mnemonic.

## 4. `CommandRegistry`

Internal storage:

```cpp
std::unordered_map<char, CommandDescriptor> commands_;
```

The registry owns its descriptors.

### `register_command()`

Input:

```cpp
CommandDescriptor descriptor
```

Behavior:

1. validates the descriptor;
2. stores it under its exact `char` key;
3. rejects duplicate insertion rather than replacing the previous command.

Failures use `std::invalid_argument`.

### `find()`

```cpp
const CommandDescriptor* find(char mnemonic) const noexcept;
```

Returns:

- pointer to owned descriptor;
- `nullptr` when absent.

The returned pointer is non-owning and tied to the lifetime of the registry.

### `contains()`

Equivalent to testing:

```cpp
find(mnemonic) != nullptr
```

### `size()`

Returns the number of registered exact keys.

## 5. Case handling

`CommandRegistry` does **no case normalization**.

The core table registers:

```text
P L D A I B C M T G Z R W S J F O Q
```

all in uppercase.

`CommandParser` converts the source mnemonic to uppercase before calling
`registry.find()`.

Consequently, with the standard registry:

```cpp
registry.contains('P')  // true
registry.contains('p')  // false
```

unless a lowercase descriptor was separately registered.

This is a registry API fact, not a statement that the FRED source language is
case-sensitive.

## 6. Core registry

`make_core_command_registry()` currently creates 18 descriptors:

| Mnemonic | Registry name | Descriptor callback role |
|---|---|---|
| P | Print | complete simple construction |
| L | List | no-address check; default no-filename node |
| D | Delete | complete simple construction |
| A | Append | rejects range; complete simple construction |
| I | Insert | rejects range; complete simple construction |
| B | Buffer | no-address check; placeholder empty buffer name |
| C | Change | complete simple construction |
| M | Move | placeholder empty destination buffer |
| T | Transfer | placeholder null destination |
| G | Global | placeholder null pattern/nested command |
| Z | Zap | basic node; CommandParser applies current range restriction |
| R | Read | default no-filename construction |
| W | Write | default Preserve/no-filename construction |
| S | Substitute | placeholder null pattern |
| J | Jump | registry metadata name retained; parser handles current JM/JP forms |
| F | Facts | default Buffers placeholder |
| O | Option | default InputParenthesis/enabled placeholder |
| Q | Quit | default non-immediate Q |

The distinction between "complete" and "placeholder/default" is important.

## 7. Why incomplete callbacks exist

The early command-parser architecture described a generic table containing the
AST construction callback. Later implementation steps added command-specific
operands while preserving the registry boundary.

The current `CommandParser` therefore follows this pattern:

```text
1. parse optional address
2. normalize/identify mnemonic
3. registry.find(mnemonic)
4. if command has special operand syntax:
       parse operands
       construct concrete node directly
   else:
       descriptor.parse(address, location)
```

Examples handled directly by `CommandParser` include R, W, S, Q, J, F, O, G,
Z, B, M, T and L.

The generic descriptor callback remains the normal final path for simple
commands such as P, D, A, I and C.

## 8. Safety implication of direct callback use

Some placeholder callbacks can create AST objects whose child pointers are null.

Examples:

- T descriptor -> `TransferCommandNode(..., nullptr, ...)`;
- G descriptor -> null pattern and nested command;
- S descriptor -> null pattern.

Their corresponding AST getters may dereference these members.

These callbacks are safe in their intended role as registry hooks because the
real `CommandParser` intercepts those source forms before fallback. They should
**not** be invoked by external code expecting a fully usable command AST.

This is a current implementation constraint worth preserving in code review
until/unless the command descriptor API is redesigned.

## 9. Registry-level validation vs parser validation

There are two different classes of checks.

### Registry integrity

Performed by `register_command()`:

```text
descriptor key/name/callback validity
duplicate key prevention
```

### Command syntax

Performed primarily by `CommandParser` and, for a few simple callback paths, by
the descriptor callback:

```text
whether an address is allowed
whether a range is allowed
required buffer/destination/pattern/filename operands
special command forms
```

Do not move Buffer-dependent semantic validation into either layer.

## 10. Errors

`CommandRegistry` uses `std::invalid_argument` for descriptor/registration
contract violations.

Exact current messages include:

```text
command mnemonic may not be NUL
command name may not be empty
command parser may not be empty
command 'X' is already registered
```

Some descriptor callbacks also throw `std::invalid_argument` when called with an
address form they do not accept.

`CommandParser`, by contrast, reports source syntax failures as `ParseError`.

This distinction is intentional in the current implementation:

```text
bad registry/programmer input -> std::invalid_argument
bad FRED source syntax        -> ParseError
```

## 11. Lifetime and mutation

The registry owns descriptors in `std::unordered_map`.

`find()` exposes only `const CommandDescriptor*`.

There is currently:

- no erase method;
- no replace method;
- no public mutable descriptor access.

The class provides no synchronization. The expected pattern is to build the
registry, then pass it by const reference to parsers.

## 12. Architectural boundary

The command registry must not:

- tokenize source;
- parse arbitrary operand text;
- access Buffer state;
- execute commands;
- perform file/system I/O.

It connects registered command identity to AST construction metadata.

## 13. Current callers/usages

The standard registry is created by the CLI/parser paths and by tests through:

```cpp
const auto registry = make_core_command_registry();
CommandParser parser(tokens, registry);
```

The Android native layer also uses the same core registry, preserving the same
command table across platforms.

Tests currently verify, among other things, that the core registry contains 18
mnemonics and includes the expected command keys.

## 14. Relationship to milestone documentation

`docs/specs/COMMAND_PARSER.md` states that the command table is created by
`make_core_command_registry()` and anticipated that later steps could provide
command-specific operand parsers while preserving the registry boundary.

That later evolution is now visible in the source.

`SPEC-008-COMMAND-PARSER.md` describes the earlier Step 7.1 subset and should
therefore be read as milestone history rather than an exhaustive description of
the current `CommandParser`/registry division of labor.
