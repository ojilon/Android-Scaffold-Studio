# Architecture Overview

## Pipeline
```
model.json -> load -> VALIDATE (Android rules) -> codegen (templates) -> files -> gradle build -> adb install
```

## Dependency Rule (Enforced in CMake)
`core` and the model know nothing about UI. Layers:
`core` <- `model/parser` <- `validator` <- `codegen/scaffolder`.
`designer`, `rendering`, `ui` sit on top later and only edit the model.
Nothing in `core`, `parser`, `scaffolder` or `codegen` may depend on `ui`, `rendering`, or `designer`.

Use existing folders (`src/core`, `src/parser`, `src/scaffolder`, `src/codegen`, ...).
Create `src/validator` if missing.
Fix the typo folder `src/scaffolder/reoursce` (use `git mv`; check CMake references).

## Generated vs User-Owned Split (Key Idea)
For each screen the generator emits:
- `res/layout/<screen>.xml` (generated, never hand-edited)
- `<Screen>ScreenBase.java` (**generated, always overwritten**): inflates the layout, calls `findViewById` once, exposes typed fields, wires click handlers to abstract methods (`onRetry()` etc.), and has `setState(State)` for screen states
- `<Screen>Activity.java` (**user-owned, generated only if missing, never overwritten**): `extends <Screen>ScreenBase`, with empty handler bodies
- `AndroidManifest.xml` entries for every screen (generated)

Because ids in XML and Java come from the same model line, id mismatches become impossible. Generated files start with a marker comment, e.g. `<!-- asstudio:generated v1 DO NOT EDIT -->` / `// asstudio:generated v1 DO NOT EDIT`.
