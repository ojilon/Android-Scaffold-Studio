# Architecture Overview

## Pipeline

```
model.json -> load -> VALIDATE (Android rules) -> codegen (templates) -> files -> gradle build -> adb install
```

Phase 1 implements the first four steps and the CLI that drives them. Gradle and
`adb` are Phase 2.

## Modules and Dependency Rule (enforced in CMake)

```
core  <-  parser  <-  validator  <-  codegen  <-  scaffolder  <-  app
```

| Module | Owns | Never does |
|---|---|---|
| `core` | `Result` (`std::expected`), logger, non-throwing filesystem wrappers, settings, project types | know anything about Android |
| `parser` | app model structs, `model.json` loader, widget tree helpers, (later) XML/Java/Gradle readers | validate Android rules, generate |
| `validator` | rules V001-V009 over the loaded model, with JSON paths | parse JSON, generate |
| `codegen` | `(model, templates) -> file text`; template renderer | touch the disk |
| `scaffolder` | template loading, writing the output tree, generated/user-owned policy | decide what the content is |
| `app` | CLI argument handling and exit codes | own logic |

Dependencies are declared with `target_link_libraries` and are one-directional:

- `parser` links `core` (PUBLIC, because its headers expose `core::Result`)
- `validator` links `parser` + `core`
- `codegen` links `parser` + `core`
- `scaffolder` links `codegen` + `parser` + `core`
- the CLI links everything

`designer`, `rendering`, `ui` sit on top later and only edit the model. Nothing
in `core`, `parser`, `validator`, `codegen` or `scaffolder` may depend on
`ui`, `rendering` or `designer`.

## File Layout on Disk

```
src/core/        include/core/{Result,FileSystem,Logger,Settings,Project}.h
src/parser/      include/parser/{AppModel,AppModelLoader,TreeUtils,ParseContext,ParseError,Xml,Java,Gradle,Parser}.h
src/validator/   include/validator/Validator.h
src/codegen/     include/codegen/{Generation,Template}.h
src/scaffolder/  include/scaffolder/{Templates,ProjectWriter}.h, generated Config.h
src/app/         main.cpp (the CLI)
templates/       xml/, java/, android/, gradle/, cpp/  ({{dotted.path}} templates)
docs/golden/     hand-written expected output, one folder per sample
samples/         hello.json plus samples/invalid/*.json (one per rule family)
tests/           doctest executables, one per module area
third_party/     vendored single-header libraries (see third_party/README.md)
```

## Generated vs User-Owned Split (Key Idea)

For every screen the generator emits:

- `res/layout/<screen>.xml` - **generated**, always overwritten
- `java/<package path>/<Screen>ScreenBase.java` - **generated**, always
  overwritten: `setContentView`, one `findViewById` per id exposed as a typed
  `protected` field, click listeners wired to abstract handlers (`onRetry()`),
  and `setState(String)` when the screen declares states
- `java/<package path>/<Screen>Activity.java` - **user-owned**, written only if
  missing, never overwritten: `extends <Screen>ScreenBase` with empty handler
  bodies
- `AndroidManifest.xml` - **generated**, one `<activity>` per screen; the first
  screen is the launcher (`exported="true"` + intent filter, every other
  activity `exported="false"`)

Because ids in XML and Java come from the same model entry, id mismatches become
impossible. Generated files start with a marker comment
(`<!-- asstudio:generated v1 DO NOT EDIT -->` / `// asstudio:generated v1 DO NOT EDIT`)
so that the Phase 4 importer has anchors, while user-owned files never carry it.

## Why the validator reads the model, not the JSON

`app` loads the file once, validates the loaded `AppModel`, then generates from
that same in-memory model. Two consequences worth keeping:

1. `generate` cannot validate one revision of a file and generate from another.
2. `nlohmann/json` stays inside `parser`, so the validator is cheap to compile
   and easy to unit test with constructed models.

JSON paths in violations are reconstructed from the model's indices
(`$.screens[0].layout.children[1].id`), so the CLI output still points at the
line a developer would edit.
