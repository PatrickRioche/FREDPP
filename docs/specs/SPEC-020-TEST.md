# SPEC-020 — Historical pattern test (`T`)

**Status:** Implemented candidate; historical homologation deferred

**Command:** `T` / `T~`

**Primary reference:** `docs/fr/reference/commandes/t.md`

## Purpose

`T` tests whether a FRED pattern exists in at least one line of the addressed
range, setting the shared condition register used by conditional procedure
jumps. `T~` inverts that range-wide existence test.

The former FREDPP implementation of `T` as an in-buffer transfer/copy command
was not the historical FRED behavior and has been replaced. The old
`tests/homologation/transfer.fred` fixture is obsolete and is not part of the
active test suite.

## Supported syntax

```fred
[address[,address]]T[~]/pattern/
[address[,address]]T[~]?pattern?
```

The omitted address means the current line. Pattern syntax and protected
character metadata use the existing central Flow/Lexer/PatternParser pipeline;
no command-specific pattern parser is introduced.

## Runtime semantics

1. Evaluate the source range using `AddressEvaluator`.
2. Search the complete pattern independently against each addressed line.
3. Set `ExecutionContext::condition()` to whether any line matched.
4. If `~` is present, negate that final Boolean result.

The Buffer text, modified flag and current-line cursor are unchanged. The
numeric counter and Output are unchanged. An invalid address raises
`CommandExecutionError` before the prior condition is overwritten.

## Current scope

The existing `PatternMatcher` and supported `/` or `?` delimiters are reused.
Complex historical options, broader pattern syntax and detailed procedure
compatibility will be covered in the later historical homologation phase.

## Traceability

- AST: `TestCommandNode` and `AstNodeKind::TestCommand`;
- Parser: `CommandParser::parse_delimited_pattern()`;
- Runtime: `execute_test()` in `src/runtime/CommandExecutor.cpp`;
- focused unit tests: `tests/test_test_command.cpp`,
  `tests/test_command_parser.cpp`;
- historical reference: `docs/fr/reference/commandes/t.md` (unchanged).
