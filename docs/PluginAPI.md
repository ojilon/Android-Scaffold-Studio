# Plugin API

Not designed yet, and deliberately not invented in Phase 1 (the plan forbids
building beyond the current phase). `src/plugins/` and `plugins/` are placeholders.

Known constraints for whoever designs it:

- The layering rule stays: a plugin may edit the model and may contribute
  templates, but cannot make `core`, `parser`, `validator` or `codegen` depend on
  `ui`/`designer`/`rendering`.
- Templates are already data (`templates/`), so a plugin that adds a template
  folder needs no new C++ interface for the simple case.
- Anything that enters the process must preserve the "no exceptions through a
  public API" rule, so plugin entry points return `core::Result`.
