# Status Report - Android Scaffold Studio

## Date: 2026-11-01

## Branch: `arena/055b103b-android-scaffold-studio`

Phase 1 is implemented and verified **in the sandbox**, with CMake 4.4.4 and GCC
12.2.0. The owner's machine runs GCC 15.2.0; nothing in the tree depends on a
compiler newer than GCC 12.

## What this session changed

The previous hand-off left the tree in a state that would not compile: the
third-party headers were 0-byte placeholders, code used `<format>` (absent on
GCC 12), half the classes had been removed, and `codegen`'s header contained a
second copy of the whole generator. This session finished the job and brought the
code back in line with the plan.

**Classes removed (owner rule: structs + free functions only)**

- `parser::xml::XmlReader` / `XmlWriter` classes -> `parse_xml` / `write_xml`
  free functions; duplicate `parser::Widget` in `WidgetTree.h` (ODR clash with
  `AppModel.h`) deleted; tree walking lives in `parser/TreeUtils.h`.
- `ParseContext` member functions -> `add_error` / `has_errors` free functions.
- `scaffolder`'s six classes (`ProjectValidator`, `DirectoryBuilder`,
  `ResourceGenerator`, `ProjectWriter`, `Scaffolder`, `TemplateEngine`) replaced
  by `Templates.h` and `ProjectWriter.h` (structs + free functions).
- `codegen/Scaffolder.h` (which contained a full generator implementation in a
  header, with no include guards for its dependencies) replaced by
  `codegen/Generation.h` + `codegen/Template.h`.
- `core`'s `filesystem` namespace became `core::fs` (`fs::Path`) so it can no
  longer be confused with `std::filesystem`.

**Deviations from the plan that were corrected**

1. **Templates were unused and empty.** `templates/*.in` were 0 bytes and codegen
   built every file with `ostringstream`. There is now a real `{{dotted.path}}`
   renderer (`codegen/Template.*`), five real templates, and the generator
   renders through them.
2. **Wrong Android file conventions.** Output was flat
   (`res_layout_home.xml`) with `R.layout.res_layout_home`. Now
   `res/layout/home.xml`, `R.layout.home`, `java/com/oji/hello/...`, and
   `docs/golden/hello/` mirrors that tree.
3. **Screens with states generated no state views.** Now a `FrameLayout` with
   `loading` (centred `ProgressBar`), `content` (the model's layout) and `error`
   (message + retry); `setState(String)` shows exactly one and `onCreate` calls it
   with the first declared state.
4. **Only the first screen was generated.** All screens are generated; the first
   one is the launcher, the rest get `android:exported="false"`.
5. **`width`/`height`/`margin`/`padding` were parsed and ignored.** They are
   rendered now, along with nested `column`/`row` children (the loader previously
   read only one level).
6. **The user-owned Activity was always overwritten.** `scaffolder` now skips
   existing user-owned files and reports them; covered by a test.
7. **The validator read raw JSON a second time and its tests called
   `Validator::validate`, which did not exist.** `validator::validate` takes the
   loaded `AppModel`; tests are rewritten against it.
8. **Rules only looked at top-level children.** V001/V002 now walk nested
   widgets.
9. **Docs claimed a full pass while `cmake` was never available.** This file now
   lists only commands that were really run.
10. **Empty/duplicate scaffolding:** removed the second template tree under
    `src/scaffolder/templates/`, filled the 0-byte `CMakePresets.json`, added
    `.clang-format` and `.editorconfig`, vendored the real third-party headers.

**Rules added while fixing codegen holes** (see docs/DECISIONS.md 14-16):

- V006 also requires `content` in `states`; V001 reserves `<screen>_state_*` ids.
- V009: ids must not be Java keywords (they become Java fields).

## Verified in this session (exact commands and results)

```
cmake -B build -S . -G Ninja                  -> Configured, "Generating done"
cmake --build build                           -> 17 targets, 0 warnings
ctest --test-dir build --output-on-failure    -> 3/3 tests passed (test_parser,
                                                 test_validator, test_codegen)
cmake -B /tmp/build-werror -S . -DASSTUDIO_WERROR=ON
cmake --build /tmp/build-werror               -> builds clean with -Werror
ctest --test-dir /tmp/build-werror            -> 3/3 passed
cmake -B /tmp/build-asan -S . -DASSTUDIO_SANITIZE=ON
cmake --build /tmp/build-asan                 -> builds clean
ctest --test-dir /tmp/build-asan              -> 3/3 passed under ASan+UBSan
./build/AndroidScaffoldStudio --help          -> usage, exit 0
./build/AndroidScaffoldStudio validate samples/hello.json
                                              -> "valid (1 screen(s))", exit 0
./build/AndroidScaffoldStudio validate samples/invalid/*.json
                                              -> each prints its rule id + JSON
                                                 path, exit 1 (V001..V009)
./build/AndroidScaffoldStudio generate samples/hello.json -o /tmp/gen
                                              -> wrote 4 files
diff -r /tmp/gen docs/golden/hello            -> no differences
cmake --list-presets                          -> default, release, asan
```

Not verified: a real Gradle build or install on a phone (no Android SDK in the
sandbox), and compilation with GCC 15.2 specifically.

## Not done (by design, see docs/08-roadmap.md)

- Gradle project skeleton (settings/build.gradle, themes, strings) - Phase 2.
- `jni_json_bridge` implementation - Phase 2 (stub + README exist).
- XML/Java/Gradle importers - Phase 4 (`parser` stubs return "not implemented").
- Dear ImGui UI, canvas preview, code view, logcat mapping - Phase 3.
- Settings persistence (`core::settings::save/load` return "not implemented").
- pugixml is vendored but not compiled; the first user is the XML importer.

## Known problems / rough edges

- The sandbox needed `pip install cmake ninja` to build at all; the owner's
  machine has its own toolchain. `docs/06-stack-and-libs.md` records the floor
  (CMake 3.25, C++23 with `<expected>`).
- The logger's level is a mutable global. Fine for a single-threaded CLI; the
  Phase 3 UI thread will need to own the sink.
- `parser`'s `Attribute` struct (`XmlAttribute.h`) is unused until the importer
  lands.
- Generated Java is plausible but has never been compiled by `javac`; the golden
  files are reviewed by hand. A real Gradle smoke test is Phase 2 work.
- Template errors are reported by file name, not by template offset.
- `tests/test_codegen.cpp` writes a scratch folder under the system temp
  directory and removes it afterwards.
- `LICENSE` is a 0-byte file in the repo; choosing a licence is the owner's call,
  so it was left untouched.

## Next three suggested steps

1. **Phase 2 start:** generate the Gradle skeleton from `templates/gradle/`
   (`settings.gradle`, `build.gradle`, `gradle.properties`, `res/values/`) so a
   generated folder is a buildable project.
2. **Smoke-test on the owner's device:** run `javac`/Gradle against
   `generated/hello`, install with `adb`, confirm the state views and
   `android:exported` behaviour, and log the first real-phone crash in
   `docs/CRASH_LOG.md` (it becomes the next rule).
3. **Guard the marker comments:** before overwriting a generated file, check it
   still starts with `asstudio:generated v1`; if it does not, report that it was
   hand-edited instead of silently replacing it. That is the first half of the
   Phase 4 round-trip anchor and needs no importer.
