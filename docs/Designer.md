# Designer

Phase 3. The designer is the ImGui shell that edits the app model: model tree,
canvas preview, property panel, code view. It **only edits the model** and
re-runs validate + generate; it never owns state and never writes files itself.

Nothing is implemented in Phase 1; `src/designer/` is an empty placeholder so the
layering rule has a home.

See [07-ui-plan.md](07-ui-plan.md).
