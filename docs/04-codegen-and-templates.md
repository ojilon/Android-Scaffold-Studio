# Codegen and Templates

## Template Locations
Templates live in `templates/` (existing subfolders `android`, `java`, `xml`, `gradle`, `cpp`).

## Phase 1 Renderer
Phase 1 uses a tiny **own** renderer: `{{dotted.path}}` substitution from a JSON value, no loops (loops/fragments are built in C++ and passed in as strings).

## Open Decision
Decide later whether to adopt `inja`; record it as an open decision in `docs/DECISIONS.md`.

## Template Ideas
- sidebar navigation
- screen states
- button styles
- layouts
- JNI bridge

## Generated Artifacts per Screen
1. `res/layout/<screen>.xml` (generated, never hand-edited)
2. `<Screen>ScreenBase.java` (**generated, always overwritten**): inflates the layout, calls `findViewById` once, exposes typed fields, wires click handlers to abstract methods (`onRetry()` etc.), and has `setState(State)` for screen states
3. `<Screen>Activity.java` (**user-owned, generated only if missing, never overwritten**): `extends <Screen>ScreenBase`, with empty handler bodies
4. `AndroidManifest.xml` entries for every screen (generated)

Marker comments: `<!-- asstudio:generated v1 DO NOT EDIT -->` / `// asstudio:generated v1 DO NOT EDIT`.
