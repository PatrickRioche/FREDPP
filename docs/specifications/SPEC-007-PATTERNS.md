# SPEC-007 - FRED Pattern Language (Step 6 core)

Status: implemented core syntax, parser only.

## Normative source

This step follows `PATTERNS - the FRED pattern matcher`, especially the summary and examples on pages 1-3.
No PCRE or C++ regular-expression syntax is introduced.

## Accepted delimiters

A complete debug input is delimited by `/.../` or `?...?`.
The delimiter is not part of the Pattern AST.

## Implemented grammar

```text
pattern      := alternation
alternation  := sequence ("|" sequence)*
sequence     := repetition*
repetition   := atom ("*" | "+")?
atom         := literal | "." | "^" | "$" | group | class
 group       := "(" alternation ")"
class        := "[" "^"? class-item+ "]"
class-item   := character | character "-" character
```

Meaning preserved from the FRED documentation:

- `.`: any single character except new-line;
- `^`: start of line;
- `$`: end of line;
- `P*`: zero or more occurrences of P;
- `P+`: one or more occurrences of P;
- `P|Q`: P or Q;
- `(P)`: grouping;
- `[XYZ]`: one character in the class;
- `[^XYZ]`: one character outside the class;
- `[c1-c2]`: inclusive ASCII range.

## Parser scope

Step 6 constructs an AST only. It does not match text and therefore does not yet implement:

- longest-match execution;
- case option `O+SD` / `O-SD`;
- column assertions `@(N)`, `@(-N)`, `@(N-)`, `@(N+)`;
- tags `{P}T` and `@T`;
- word boundaries `<` and `>`;
- defined patterns `\E(name)`;
- fence `#`;
- FRED class-case modifiers `\C` and `\O`.

These documented forms are reserved for later compatibility milestones and are not silently interpreted as PCRE.

## Errors

The parser rejects unterminated patterns, groups and classes, empty groups/classes,
missing operands around `|`, leading/repeated quantifiers, dangling escapes and trailing input.
