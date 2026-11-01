# designer (Phase 3)

Empty placeholder for the ImGui editor shell: model tree, property panel, canvas
preview, code view. It only edits `parser::AppModel` and re-runs
validate/generate; it never owns state. See docs/07-ui-plan.md.

Layering rule: nothing in `core`, `parser`, `validator`, `codegen` or `scaffolder`
may depend on this folder.
