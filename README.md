# Android Scaffold Studio

A native desktop tool (Phase 1: CLI) that turns one JSON app model into correct
Android XML, Java and manifest files - and refuses the model when it would
produce code that crashes on the phone.

Read [AGENT_PROMPT.md](AGENT_PROMPT.md) for the full plan, [AGENTS.md](AGENTS.md)
for the conventions, and [docs/STATUS.md](docs/STATUS.md) for where things stand.

## Quick Start

```sh
cmake -B build -S .            # or: cmake --preset default
cmake --build build
ctest --test-dir build --output-on-failure

./build/AndroidScaffoldStudio --help
./build/AndroidScaffoldStudio validate samples/hello.json
./build/AndroidScaffoldStudio generate samples/hello.json -o generated/hello
```

Requires CMake >= 3.25 and a C++23 compiler with `<expected>` (GCC 12+,
Clang 16+, MSVC 19.33+). Ninja is used when present.

`generate` writes exactly the files under `docs/golden/hello/`:

```
AndroidManifest.xml
res/layout/home.xml
java/com/oji/hello/HomeScreenBase.java   (generated, always overwritten)
java/com/oji/hello/HomeActivity.java     (user-owned, never overwritten)
```

## Why

Hand-writing Android XML and Java crashes apps: missing ids, wrong view types,
unregistered activities, forgotten `android:exported`. Here the model is the only
source of truth, the validator enforces the Android rules with stable rule ids,
and the generator emits XML and Java from the same model entry so they cannot
disagree.

## Project Layout

```
src/core/        Result (std::expected), logger, filesystem, settings, project
src/parser/      app model + model.json loader + widget tree helpers
src/validator/   V001..V009 Android rules over the loaded model
src/codegen/     (model, templates) -> file text, {{dotted.path}} renderer
src/scaffolder/  template loading + writing the output tree
src/app/         CLI
templates/       xml/, java/, android/, gradle/, cpp/ templates
docs/            vision, architecture, schema, rules, codegen, JNI, UI, roadmap,
                 golden output, decisions, status, crash log
samples/         hello.json and samples/invalid/*.json
tests/           doctest suites
third_party/     pinned vendored libraries
```

All C++ is functional: structs for data, free functions for behaviour, no
classes. See [AGENTS.md](AGENTS.md).
