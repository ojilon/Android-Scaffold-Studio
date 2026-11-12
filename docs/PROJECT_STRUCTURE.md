# Project structure (Odin + Java/XML hybrid)

## Scaffold tool (this repo)

```
src/
  app/main.odin          # CLI entry (package main)
  parser/                # model + JSON loader
  validator/             # V001–V009
  codegen/               # generate XML/Java (+ later Odin/JNI)
  core/                  # Result, logger
templates/
  xml/ java/ jni/ odin/  # screen + engine templates
  gradle/                # APK project skeleton
samples/                 # model.json examples
docs/golden/             # expected generate output
tools/build-cli.sh
```

Build CLI:

```sh
./tools/build-cli.sh
# or
odin build src/app -out:build/asstudio -collection:src=src
```

## Generated Android app (after `asstudio generate`)

Phase 1 emit (flat, matches golden):

```
AndroidManifest.xml
res/layout/<screen>.xml
java/<pkg path>/<Screen>ScreenBase.java   # generated, overwrite OK
java/<pkg path>/<Screen>Activity.java     # user-owned, skip if exists
```

When scaffolding a full Gradle project (next step), map into:

```
app/
  src/main/
    AndroidManifest.xml
    java/<pkg>/...
    res/layout/...
    jniLibs/arm64-v8a/libengine.so   # from Odin
  build.gradle
settings.gradle
gradle.properties
```

## Java ↔ XML ↔ Odin

1. **XML** – layouts + state FrameLayouts (`loading` / `content` / `error`).
2. **Java ScreenBase** – `findViewById`, click listeners, `setState`.
3. **Java Activity** – user fills handler bodies; never overwritten once present.
4. **Odin .so** – domain logic; JNI uses **file-backed results** (WayDirect):
   Java passes small request + out path; native writes payload under cache;
   returns `{status, path}`.

Gradle owns APK packaging. Odin owns the `.so`.
