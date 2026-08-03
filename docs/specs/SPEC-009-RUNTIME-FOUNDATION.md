# SPEC-009 — Runtime foundation (Step 7.2A)

## Status

Implemented by FRED++ Step 7.2A.

## Purpose

This specification establishes the execution boundary used by all future FRED
commands. It deliberately defines infrastructure only; it does not define the
semantics of `P`, `L`, `D`, or any other command.

## Separation of responsibilities

```text
CommandParser -> Command AST -> CommandExecutor -> ExecutionContext
                                                  |-> BufferManager
                                                  `-> Output
```

- The parser builds an AST and does not access editor buffers.
- The executor receives an already parsed AST and does not parse source text.
- `ExecutionContext` exposes runtime services without owning them.
- Runtime output goes through `Output`; command code must not use `std::cout`.

## Output abstraction

`Output::write()` is the single primitive required by the runtime.
`write_line()` is a convenience built on that primitive.

Step 7.2A supplies:

- `StringOutput`, for deterministic tests and embedding;
- `ConsoleOutput`, for a later REPL integration.

## Execution context

The minimal context exposes:

- the `BufferManager`;
- the current `Buffer` through the manager;
- the selected `Output` destination.

Its dependencies are non-owning references. Their lifetime must exceed that of
the context.

## Command executor

`CommandExecutor::execute()` is the unique runtime entry point for command ASTs.
In this milestone every recognized command raises
`CommandNotImplementedError`. This is intentional: Print and List semantics are
reserved for Step 7.2B.

## Excluded from Step 7.2A

- address evaluation;
- Print execution;
- List execution;
- REPL routing of native FRED command lines;
- file and bootstrap execution;
- direct console output from the runtime.
