# Development Method

**Understand → Restore → Document → Test**

## Branch and release discipline

- Work on `dev` during implementation, source documentation and focused
  Windows 11 validation.
- Keep `main` stable. Do not merge `dev` into `main` or publish a stable
  tag until a validated release decision has been made.
- A `vX.Y.Z` tag at the current `dev` tip may publish a **pre-release**;
  stable GitHub releases remain restricted to the current `main` tip.
- Optional development tags may mark reproducible checkpoints but do not
  authorize a release.
- Updating a development commit does not automatically change the software
  version declared in `CMakeLists.txt`.

## Traceability of each development lot

1. Identify the historical reference, current implementation and open limits.
2. Implement the smallest coherent change through the shared parser/runtime
   architecture and document non-obvious C++ behavior in English.
3. Add focused automated regressions, including an observable Windows CLI
   scenario when the command's effects can be demonstrated meaningfully.
4. Run Windows/MSVC CI on `dev` and record failures as well as corrections.
5. Request user validation on their Windows 11 checkout when appropriate.
6. Update [the dated development log](DEVELOPMENT_LOG.md) with commits, CI
   evidence, local-test status, known limits and the release state.
7. Maintain [CHANGELOG](../../CHANGELOG.md) for version-oriented summaries and
   [ROADMAP](../../ROADMAP.md) for planned work; do not use either as the
   sole record of individual development operations.

The [Git commit history](https://github.com/PatrickRioche/FREDPP/commits/dev/)
remains authoritative for file-level differences and timestamps. A successful
automated test is not a substitute for local Windows 11 validation or the
later complete historical compatibility campaign.
