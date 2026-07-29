# SPEC-008 — Command parser infrastructure

Status: Step 7.1 implemented.

## 1. Purpose

The command layer combines an optional address expression with one registered
FRED command. It builds an AST only. It does not inspect or modify a Buffer and
it performs no command execution.

## 2. Input form

The infrastructure implemented in Step 7.1 accepts:

```text
[ address ] command [ newline ] end
```

An address is parsed by `AddressParser::parse_prefix()`. The command mnemonic
is one uppercase command token. Horizontal whitespace continues to follow the
lexer rules established in SPEC-003.

## 3. Registry

`CommandRegistry` maps one mnemonic to one `CommandDescriptor`. A descriptor
contains:

- the mnemonic;
- a descriptive name;
- the AST construction callback.

Duplicate mnemonics, empty names, NUL mnemonics and empty callbacks are
rejected. The parser contains no command-specific `switch` and obtains command
behavior from the registry.

## 4. AST

`CommandNode` owns an optional `AddressNode` and records the command source
location. Step 7.1 supplies structural AST nodes for the first registered
mnemonics:

- `P` → `PrintCommandNode`;
- `L` → `ListCommandNode`;
- `D` → `DeleteCommandNode`.

These nodes identify syntax only. Their semantics are intentionally deferred to
later steps.

## 5. Diagnostics

Required diagnostics include:

- `expected a command` when input or the post-address input ends;
- `expected a command, got '…'` for a non-command token;
- `unknown command 'X'` for an unregistered uppercase mnemonic;
- `unexpected token after command: '…'` for trailing input.

On failure, `CommandParser` restores the TokenStream position at which parsing
began.

## 6. Non-goals

Step 7.1 does not define:

- default-address semantics;
- command execution;
- command operands or command bodies;
- pattern addresses;
- bootstrap execution;
- file or batch processing.

Those behaviors must be added from the FRED manuals in later specifications.
