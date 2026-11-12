# AGENTS.md – Android Scaffold Studio (Odin rewrite)

Read this before touching the code.

## Language & build

- **Language:** Odin (dev-2026-11 or compatible).
- **Collections:** `-collection:src=src` so packages live under `src/`.
- **CLI binary:** `odin build src/app -out:build/asstudio -collection:src=src`
- **Tests:** `odin test tests -collection:src=src`

## Style

- Prefer clear, boring code. Small procedures. Comments explain *why*.
- Data is `struct`s; behaviour is free procedures in the package.
- Errors across package boundaries use the `core.Result` pattern or explicit
  `(value, error, ok)` triples. The CLI is the only place that calls `os.exit`.
- No hidden global mutable state except the logger level and the native
  `g_app_home` (which is set once from Java).

## Layering (same as the original C++ design)

```
core  ←  parser  ←  validator  ←  codegen  ←  scaffolder  ←  app
```

Nothing in the lower layers may import the higher ones. Designer / UI packages
(when they appear) only edit the model and re-run validate/generate.

## Templates

- Live under `templates/` (`xml/`, `java/`, `android/`, `gradle/`, `odin/`, `jni/`).
- Renderer supports only `{{dotted.path}}`. Loops and conditional fragments are
  built in Odin and passed in as pre-rendered strings.
- Generated files carry the marker `asstudio:generated v1 DO NOT EDIT`.
- User-owned files never get the marker and are never overwritten.

## Crash → rule

Every crash found on a real phone becomes a new validator rule with a stable
id, documented in `docs/03-validator-rules.md` (still to be ported) and logged
in `docs/CRASH_LOG.md`. The model is fixed; the generated file is not hand-
edited.

## WayDirect heritage

See `docs/WAYDIRECT_PATTERNS.md`. The file-backed JNI pattern, screen-state
discipline, and generated/user-owned Java split are first-class citizens of
this branch.

## Plan location

- Vision & architecture live under `docs/`.
- Current status: `docs/STATUS.md`.
- Decisions that affect the rewrite: record them in `docs/DECISIONS.md` as
  they are made.
