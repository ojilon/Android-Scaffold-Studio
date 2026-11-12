# Status – odin-rewrite branch

**Date:** 2026-11-12  
**Branch:** `odin-rewrite`

## Goal of this branch

1. Rewrite the Phase-1 C++ core (model → validate → generate) in **Odin**.
2. Capture the reusable patterns that WayDirect already proved work on a real
   phone (Material 3 Java/XML shell, Odin native engine, file-backed JNI
   results, screen-state discipline).
3. Keep Gradle responsible for XML/Java/APK; Odin builds the `.so`.
4. Leave a clean foundation so more screens, charts, themes and states can be
   added as templates later.

## What exists in this first commit

### Core Odin packages
- `src/core` – Result, logger
- `src/parser` – AppModel structs + JSON loader (same schema as C++)
- `src/validator` – V001–V009 rules ported
- `src/codegen` – `{{dotted.path}}` template renderer + full generator
- `src/app` – CLI (`validate` / `generate` / `--help`)

### Templates captured from WayDirect principles
- `templates/xml/screen_with_states.xml.in` – FrameLayout + state views
- `templates/java/ScreenBase.java.in` / `Activity.java.in` – generated vs user-owned split
- `templates/jni/file_result_bridge.odin.in` – the “request + out-path → file result” pattern
- `templates/odin/engine_core.odin.in` – action-router skeleton for the native side

### Docs
- README describing the new direction
- AGENTS.md, STATUS, WAYDIRECT_PATTERNS

## Not done yet (next concrete steps)

1. **Golden parity** – make `generate samples/hello.json` produce files
   identical to the original `docs/golden/hello/` (line-ending normalised).
2. **Gradle skeleton templates** (`settings.gradle`, `build.gradle`,
   `gradle.properties`, Material 3 deps, arm64-v8a filter).
3. **Odin JNI glue template** that matches the WayDirect `waydirect_jni.odin`
   shape (JNI_OnLoad + Java_* exports that call the engine).
4. **Tests** – Odin test packages that exercise loader, each rule, and golden
   comparison.
5. **Build scripts** under `tools/` (`build-cli.sh`, `build-android-so.ps1`).
6. **More templates** – themes, navigation shell, list/detail, charts.

## Design decisions for this branch

- Keep the exact same model schema and rule ids so existing samples and the
  golden files stay valid.
- Prefer Odin for both the scaffold tool *and* the generated native engine;
  the C ABI boundary is only needed where bionic forces a tiny C helper.
- Gradle stays; we do not try to replace the Android build system.
- Templates stay simple (`{{path}}` only). Loops stay in Odin code.

## How to continue

```sh
git checkout odin-rewrite
# (optional) remove or ignore the old C++ tree while iterating
odin build src/app -out:build/asstudio -collection:src=src
./build/asstudio validate samples/hello.json
./build/asstudio generate samples/hello.json -o generated/hello
```
