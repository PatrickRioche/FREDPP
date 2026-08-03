# SPEC-010 - Print and List execution

## Scope

Step 7.2 makes the documented FRED `P` and `L` commands executable.

## P - Print addressed lines

Syntax from the FRED command summary:

- `(.,.)P`
- `(.,.)` (implicit print form is reserved for a later step)

The default address is the current line. A single address prints one line. A
range prints every addressed line in order. The last printed line becomes the
current line.

## L - List file on terminal

Syntax from the FRED command summary:

- `L [filename]`

`L filename` writes the named file to the runtime output without changing the
current editor buffer. `L` without a filename is parsed, but execution reports
that current-file support is not implemented yet. Line addresses are rejected
for `L`; `1L` is not valid historical FRED syntax.

## Exclusions

`D` remains parsed but not executable. Current-file state, implicit print,
patterns as addresses, and bootstrap execution are outside this step.
