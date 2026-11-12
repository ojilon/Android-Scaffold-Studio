# Android Scaffold Studio (Odin Rewrite)

**Branch:** `odin-rewrite`

A native desktop CLI tool that turns one JSON app model into correct Android
XML, Java, Gradle and Odin/JNI scaffolding — and refuses the model when it
would produce code that crashes on the phone.

This branch rewrites the original C++ core in **Odin** while keeping the same
architecture and the same model → validate → generate pipeline. It also
captures the proven patterns from **WayDirect** (Material 3 Java/XML UI + Odin
native engine + file-backed JNI results) into reusable templates.

## Why Odin

- Easier to read and modify than C++ for developers who learn by reading AI
  code and then making changes themselves.
- Official Android target (`-subtarget:android`, `odin bundle android`).
- Clean FFI / JNI story; the same language can own both the scaffold tool and
  the generated native engine.
- Good enough tooling, fast compile times, simple project structure.

## Architecture (unchanged)

```
model.json → load → VALIDATE (Android rules) → codegen (templates) → files
                                                              ↓
                                                    Gradle builds APK
                                                    Odin builds .so
```

Layers (same dependency rule as the C++ version):

```
core  ←  parser  ←  validator  ←  codegen  ←  scaffolder  ←  app
```

- **core** – Result, logger, filesystem, settings
- **parser** – AppModel structs + JSON loader
- **validator** – V001… rules over the loaded model
- **codegen** – pure functions that turn (model, templates) into file text
- **scaffolder** – loads templates, writes the output tree (generated vs
  user-owned overwrite policy)
- **app** – CLI

## Quick Start

```sh
# Requires Odin (dev-2026-11 or compatible) on PATH
odin version

# Build the CLI
odin build src/app -out:build/asstudio -collection:src=src

# Validate
./build/asstudio validate samples/hello.json

# Generate
./build/asstudio generate samples/hello.json -o generated/hello
```

Generated layout (mirrors the original golden):

```
AndroidManifest.xml
res/layout/home.xml
java/com/oji/hello/HomeScreenBase.java   (generated, always overwritten)
java/com/oji/hello/HomeActivity.java     (user-owned, never overwritten)
```

When a screen declares `"backend": "odin_jni"` the generator also emits:

```
native/
  jni/…_jni.odin          # thin JNI_OnLoad + Java_* exports
  odin/…_core.odin        # action router + C ABI
  build-android.ps1 / .sh # produces lib….so → jniLibs/
```

## WayDirect patterns captured as templates

| Pattern | Location | Notes |
|---------|----------|-------|
| Screen states (loading / content / error) | `templates/xml/screen_states.xml.in` | FrameLayout + three state views |
| Material 3 Java shell | `templates/java/ScreenBase.java.in` | ViewBinding-ready, abstract handlers |
| File-backed JNI results | `templates/jni/file_result_bridge.odin.in` | Request path + output path; native writes JSON/file, Java reads it |
| Odin native engine skeleton | `templates/odin/engine_core.odin.in` | Action router, private app-home paths |
| Gradle + ABI filter | `templates/gradle/` | arm64-v8a default, Material 3 deps |
| Theme / chrome tokens | `templates/android/themes.xml.in` | Starting point for Material 3 |

## Generated vs user-owned

- Files that start with `<!-- asstudio:generated … -->` or
  `// asstudio:generated …` are always overwritten.
- User-owned Activity / Fragment files are written **only if they do not
  already exist**.

## Project layout

```
src/
  core/          Result, logger, fs, settings
  parser/        AppModel + JSON loader
  validator/     Android rules
  codegen/       template renderer + generation
  scaffolder/    disk writer + overwrite policy
  app/           CLI (main)
  jni/           shared helpers for generated JNI

templates/
  xml/  java/  android/  gradle/  odin/  jni/

samples/         hello.json + invalid models
docs/            vision, schema, rules, WayDirect notes, roadmap
tools/           build scripts, odin helpers
tests/           odin test packages
```

## Relationship to the C++ tree

The original C++ Phase-1 implementation remains on `main`. This branch is the
experimental rewrite. Once the Odin CLI reaches feature parity (validate +
generate matching the golden files) the plan is to make `odin-rewrite` the
default development line and keep the C++ tree only as historical reference.

## Status

See [docs/STATUS.md](docs/STATUS.md).
