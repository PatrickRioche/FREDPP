# Command parser

Step 7.1 introduces the integration layer above `AddressParser`.

```text
Lexer → TokenStream → AddressParser → CommandParser → Command AST
```

Example:

```text
1,$P
```

produces a `PrintCommandNode` that owns a `RangeAddressNode`. No Buffer is
available to the parser and no output is produced.

The command table is created by `make_core_command_registry()`. Later command
steps extend this table and may provide command-specific operand parsers while
preserving the same registry boundary.
