# Windows development test: historical T condition and procedure jumps

The historical `T` command does **not** print a message. It searches an
addressed line/range and sets `ExecutionContext.condition()`. Inversion with
`T~` reverses that Boolean result without modifying the buffer.

`ProcedureRunner` already recognizes `J(label)T` (jump on true) and
`J(label)F` (jump on false). This test connects those independently tested
features: `T` sets the condition, a conditional jump selects a branch, and
`JM` makes the selected branch visible. No additional T syntax or debug
output is introduced.

## Windows 11 manual run

From the FREDPP checkout in PowerShell:

```powershell
.\scripts\build.bat
$env:FREDPP_INIT = ""
.\out\build\x64-Debug\Debug\fredpp.exe .\tests\fixtures\cli_test_condition_t.fredpp
```

Expected output (the test procedure exits on completion):

```text
FREDPP-T-TRUE-1
FREDPP-T-FALSE-2
FREDPP-T-RANGE-3
FREDPP-T-INVERT-4
FREDPP-T-INVERT-FALSE-5
FREDPP-T-BUFFER-BEGIN
alpha
beta
gamma
FREDPP-T-BUFFER-END
```

Every `FREDPP-T-FAIL-*` marker indicates a regression in the condition or
branch behavior. The printed buffer checks that T left the text unchanged.

## Automated focused regression

```powershell
ctest --test-dir .\out\build\x64-Debug -C Debug --output-on-failure -R "^cli_test_condition_t$"
```

CTest runs `tests/test_cli_condition_t.cmake`, which executes
`tests/fixtures/cli_test_condition_t.fredpp` with user init disabled. The
resulting transcript must match the expected text **exactly**.

This focused engineering test is also run in the `dev` Windows/MSVC workflow.
It does **not** constitute complete historical FRED compatibility certification.

## Source responsibilities

- `src/runtime/CommandExecutor.cpp`: evaluate T and update condition.
- `src/runtime/ProcedureRunner.cpp`: parse/execute conditional label jumps.
- `src/runtime/ExecutionContext.cpp`: hold the shared Boolean condition.
- `tests/test_test_command.cpp`: isolated T assertions (including silence).
- `tests/test_procedure_jump.cpp`: isolated jump/label assertions.
- `tests/test_cli_condition_t.cmake`: end-to-end visible output assertion.

The command implementation is intentionally unchanged by this test addition.
