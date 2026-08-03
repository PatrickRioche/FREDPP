# Runtime foundation

Step 7.2A introduces the first execution-layer classes without changing the
interactive demo's behavior.

## Main types

- `Output`: abstract text destination.
- `StringOutput`: in-memory destination for tests.
- `ConsoleOutput`: adapter around an existing `std::ostream`.
- `ExecutionContext`: access to buffers and runtime output.
- `CommandExecutor`: execution boundary for command AST nodes.
- `CommandNotImplementedError`: explicit result for commands whose semantics
  have not yet been installed.

The absence of Print/List execution is deliberate. Step 7.2B will add address
evaluation and implement those two commands on top of this stable boundary.
