# SPEC-005 — Grammar foundation

Status: implemented in FRED++ 0.5.0.

## 1. Scope

This milestone introduces the common abstract syntax tree (AST) foundation and
parses one complete address expression. It does not parse commands or patterns.

## 2. Layering

```text
CharacterStream -> Lexer -> TokenStream -> AddressParser -> Address AST
```

The lexer remains syntax-neutral. Meaning is assigned by `AddressParser`.

## 3. Milestone grammar

```ebnf
AddressExpression = SingleAddress [ "," SingleAddress ] ;
SingleAddress     = Number | "." | "$" | RelativeAddress ;
RelativeAddress   = ( "+" | "-" ) Number ;
```

Horizontal whitespace is discarded by the lexer. A single trailing newline is
accepted before end-of-input.

## 4. Error model

A syntax error raises `fred::ParseError` with the source location of the token
that made parsing fail. `AddressParser::parse()` is transactional: on failure,
the `TokenStream` is restored to its initial position.

## 5. Deferred syntax

Search addresses, pattern syntax, omitted range endpoints, chained arithmetic,
and command parsing are outside this milestone. Their behavior must be derived
from the FRED manuals before implementation.
