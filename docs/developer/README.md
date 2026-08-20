# FREDPP source documentation

This directory is the developer-oriented companion to the normative
specifications under `docs/specs/`.

The goal is to document the implementation as it exists in the source tree:
class responsibilities, inputs and outputs, ownership, side effects, invariants,
limits, and the relationships between layers.

## Source pipeline

```text
Buffer / BufferManager
          ↑
          │ lookup
FlowEngine / CommandInputExpansion
          ↓
CharacterStream
          ↓
Lexer
          ↓
TokenStream
          ↓
AddressParser / PatternParser / CommandParser
          ↓
AST
          ↓
AddressEvaluator / PatternMatcher
          ↓
CommandExecutor
          ↓
ExecutionContext
   ├── BufferManager
   └── Output
```

`ProcedureRunner` coordinates the same Flow -> Lexer -> Parser -> Executor
pipeline for procedure buffers/files and adds procedure-only control syntax.

For command text, flow/substitution that is valid in historical FRED syntax is
a **pre-parsing language stage**. It must remain centralized rather than being
implemented independently by each command.

## Module documentation

- [Lexer and character/token infrastructure](LEXER.md)
- [AST nodes and ownership](AST.md)
- [Address, pattern and command parsers](PARSER.md)
- [Command descriptors and registry](COMMAND.md)
- [Core buffers, manager and limits](CORE.md)
- [Flow engine and command-input expansion](FLOW.md)
- [Runtime evaluation, execution and procedures](RUNTIME.md)

Further modules are added incrementally as the source-documentation lots are
completed.

## Documentation rules

1. Public C++ interfaces use Doxygen-compatible comments in `.hpp`/`.h`.
2. Implementation comments in `.cpp` explain non-obvious behavior, invariants
   and historical compatibility rules; they do not restate obvious code.
3. Documentation distinguishes:
   - architectural constraints;
   - current implementation limits;
   - historical FRED language rules;
   - FREDPP extensions;
   - functionality not yet implemented.
4. Documentation-only lots must not change runtime behavior.
5. Statements about behavior should be cross-checked against implementation and
   tests before being written as a contract.
6. Older milestone specifications are not assumed to describe every feature of
   the current implementation; later code/tests take precedence when the
   developer documentation describes the current source tree.
7. Flow/substitution in command arguments is a transversal language capability:
   when historical syntax permits it, expansion occurs before command parsing,
   never as command-by-command patches.
8. Runtime receives parsed AST and must not reparse source command text.
