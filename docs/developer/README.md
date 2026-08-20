# FREDPP source documentation

This directory is the developer-oriented companion to the normative
specifications under `docs/specs/`.

The goal is to document the implementation as it exists in the source tree:
class responsibilities, inputs and outputs, ownership, side effects, invariants,
limits, and the relationships between layers.

## Source pipeline

```text
FlowEngine
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
AddressEvaluator
    ↓
Runtime
```

## Module documentation

- [Lexer and character/token infrastructure](LEXER.md)

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
   - functionality not yet implemented.
4. Documentation-only lots must not change runtime behavior.
5. Statements about behavior should be cross-checked against implementation and
   tests before being written as a contract.
