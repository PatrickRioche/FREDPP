# C++ implementation documentation standard and completeness

## 1. Status

This document records the stricter in-source documentation standard adopted
after the seven-lot architecture audit.

FREDPP source documentation is mandatory and written in **English**.

The rule applies to useful technical documentation in:

```text
.hpp
.h
.cpp
Android/Kotlin source when documenting FREDPP implementation behavior
build files when an architectural rule needs explanation
```

French remains appropriate for user-facing French documentation and runtime
messages where French is the intended UI language.

## 2. What “complete” means

Documentation completeness does **not** mean putting a Doxygen block before
every trivial getter.

A source file is complete when:

- public contracts are documented in headers;
- the implementation file explains its role when that role is not obvious;
- complex/private helpers explain their contract where useful;
- ownership and lifetime constraints are explicit;
- non-obvious invariants and side effects are explained;
- historical behavior is distinguished from FREDPP extensions and current
  implementation limits;
- comments explain *why/contract/invariant*, not obvious individual statements.

## 3. `.cpp` audit result

The product tree currently contains 26 maintained `.cpp` files.

The post-Lot-7 audit classified:

```text
8  already compliant / quasi-compliant
11 adequately covered by header contracts plus targeted implementation comments
7  requiring implementation-comment normalization
```

Lot 8 normalizes the seven files with the largest remaining documentation gap:

```text
src/cli/main.cpp
src/HelpManager.cpp
src/Terminal.cpp
src/parser/PatternParser.cpp
src/parser/CommandParser.cpp
src/runtime/CommandExecutor.cpp
src/runtime/ProcedureRunner.cpp
```

The other `.cpp` files are intentionally not inflated with repetitive comments.

## 4. Lot 8 invariants

Lot 8 is documentation-only.

It must not:

- change C++ executable tokens;
- remove the currently unused `starts_with_ci()` helper;
- alter Runtime diagnostics containing old milestone wording;
- fix known implementation limitations;
- refactor functions;
- alter command semantics;
- alter public API.

The application helper verifies every target against the audited Git blob ID
and compares pre/post source after removing comments and whitespace.

## 5. Known issues deliberately left for separate work

Examples include:

```text
ProcedureRunner starts_with_ci() unused warning
old "Lot 4"/"this lot" runtime diagnostic wording
AddressEvaluator forward relative size_t overflow guard
FlowEngine reuse after an exceptional active stack
front-end whitespace asymmetries
Android hard-coded version fallback
```

These are not documentation changes.

## 6. Maintenance rule

Whenever a `.cpp` gains significant new logic, the same change must review:

```text
file/module responsibility
helper contracts
ownership/lifetime
preconditions/postconditions
side effects
error behavior
historical compatibility
current limits
source comments
tests
```

If a new `.hpp`, `.h` or `.cpp` product file is introduced, add it to
`files/SOURCE-FILES.md` and document it at the same time.

## 7. Language rule

For FREDPP:

```text
in-source technical documentation -> English
```

This includes existing comments touched by a change.

Do not translate user-visible French strings merely to satisfy this rule:
runtime/UI text and source documentation are different concerns.
