# Contributing

## Permanent branches

- `dev`: all ongoing C++20 development, regression fixes, in-source English
  documentation, tests and development commits.
- `main`: stable sources only. Do not develop or tag experimental work directly
  here. Promote `dev` to `main` only after the release review and full required
  validation are complete.

The development lane compiles all C++ targets with Windows/MSVC on changes to
`dev` and runs focused engineering regressions. Historical compatibility
homologation is a separate pre-release activity; a green development build
does not confer release approval.

## Optional development tags

Intermediate tags may be created on `dev` to mark a checkpoint. Use names such
as `dev/2026-11-07-flow` rather than stable `vX.Y.Z` release tags. Development
tags are not releases and do not trigger the publication workflow.

## Before a stable release

1. Keep implementation and source documentation together in `dev`.
2. Build successfully and run focused technical regressions during development.
3. Complete the broader compatibility/regression campaign and user validation
   before declaring the version ready.
4. Fast-forward `main` to the approved `dev` commit, preserving the same tree.
5. Create the official `vX.Y.Z` tag on `main` after the version in CMake has
   been finalized; only then publish GitHub Releases.

The release workflow refuses a stable tag unless it points to the current
`main` commit, even if the tag name matches the project version.

## Development quality rules

1. Preserve the Core → Flow → Lexer → Parser → Runtime architecture.
2. Add focused tests for new behavior and run the applicable checks.
3. Document affected C++ contracts, invariants, errors and limitations in
   English at the same time as the implementation.
4. Keep commits focused.

Commit style examples:

Sprint 2.5 - Implement Move command
Sprint 2.5 - Fix Move regression
