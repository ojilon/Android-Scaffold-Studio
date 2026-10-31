# AGENT PROMPT: Bootstrap Android Scaffold Studio

You are implementing the first foundation of **Android Scaffold Studio** on to run on Windows. The owner (Oji) is not coding today. Your job is to (1) preserve the whole plan below as markdown in the repo, (2) get a small C++ core compiling and doing a little real work, and (3) leave clear status notes so any later session can continue.

**Read this whole prompt before doing anything.**

---

## 0. Ground rules

- **Surgical edits, not rewrites.** The repo already has a folder skeleton and a CMake build. Inspect first (`git status`, list `src/`, read the root `CMakeLists.txt`, look at what exists). Extend what is there. Keep the existing directory layout. Do not delete or reorganize wholesale. Never edit the `build/` folder by hand; make sure it is in `.gitignore`.
- **Hardware is weak** (Intel Celeron, about 4 GB RAM). No Boost, no Qt, no heavy dependencies, no template-metaprogramming-heavy code. Keep compile times low (headers include only what they use, prefer forward declarations, no giant single-header dependencies in many translation units).
- **Foundations over cleverness.** Simple, readable code. Small functions. Comments explain *why*.
- **Scope of this session is Phase 1 only** (section 7). Later phases are documented, not built.
- **Do not invent features** beyond this prompt. If something is ambiguous, pick the simplest option and record it in `docs/DECISIONS.md`.

## 1. Concept images

The owner placed concept images in `docs/concepts/` (list the folder and view every image). They show: a main window (screens-and-states tree, phone-shaped canvas, properties and constraints panel, live device log whose crashes link back to the widget that caused them), a split Design/Code view (selected widget highlights its XML lines), and a Template library (sidebar navigation, screen states, button styles, JNI bridge).

Treat them as **guidance, not spec**. Differences to remember:

- The concepts show Kotlin file names and Fragments, and a ConstraintLayout-style constraint editor. **v1 is Java + XML, Activities only, LinearLayout-style `column`/`row` containers.** Constraint layout is a later phase.
- The "24 templates" count is fictional. Real template count starts at a handful.
- Write `docs/concepts/README.md` saying exactly this: images are inspiration for the UI phase; the model and validator come first.

## 2. The problem this tool solves (write into `docs/00-vision.md`)

Hand-writing Android XML and Java is painful. Specific pains:

1. Apps build successfully and then **crash on the phone** because XML and Java disagree (missing ids, wrong view types, unregistered activities, bad themes).
2. The developer cannot visualize runtime while coding; even AI assistants write on assumptions. The feedback loop between coding and previewing is slow.
3. Many patterns repeat: JNI communication, screen states, buttons, sidebars, layouts. These should be templates/codegen, standardized.
4. Android must be targeted by **rules**, even though the design happens in a PC app.

The goal: a **standalone native desktop tool** where the developer designs UI/UX (eventually by drag and drop) on top of a single source of truth (an **app model**), the tool **validates it against Android rules**, **generates** correct XML/Java/manifest/Gradle (and JNI plumbing), and eventually gives fast feedback (instant approximate preview, then run-on-device with crashes mapped to model elements).

Also document the later ambition: **first-class round-trip editing** (import existing XML+Java, edit visually, write back). Decision: round-trip remains the goal but **v1 is model to generate only**. Importer order later: XML first (easy), Java last (hard, tree-sitter-java). Generated files carry marker comments so an importer has anchors.

## 3. Core design (write into `docs/01-architecture.md` and `docs/02-model-schema.md`)

### Pipeline

```
model.json -> load -> VALIDATE (Android rules) -> codegen (templates) -> files -> gradle build -> adb install
```

### Dependency rule (enforce in CMake with separate targets)

`core` and the model know nothing about UI. Layers: `core` <- `model/parser` <- `validator` <- `codegen/scaffolder`. `designer`, `rendering`, `ui` sit on top later and only edit the model. Nothing in `core`, `parser`, `scaffolder` or `codegen` may depend on `ui`, `rendering`, or `designer`.

Use the existing folders (`src/core`, `src/parser`, `src/scaffolder`, `src/codegen`, ...). If a validator folder does not exist, create `src/validator`. **Fix the typo folder `src/scaffolder/resource`** (use `git mv`; check CMake references).

### Generated vs user-owned split (key idea)

For each screen the generator emits:

- `res/layout/<screen>.xml` (generated, never hand-edited)
- `<Screen>ScreenBase.java` (**generated, always overwritten**): inflates the layout, calls `findViewById` once, exposes typed fields, wires click handlers to abstract methods (`onRetry()` etc.), and has `setState(State)` for screen states
- `<Screen>Activity.java` (**user-owned, generated only if missing, never overwritten**): `extends <Screen>ScreenBase`, with empty handler bodies
- `AndroidManifest.xml` entries for every screen (generated)

Because ids in XML and Java come from the same model line, id mismatches become impossible. Generated files start with a marker comment, e.g. `<!-- asstudio:generated v1 DO NOT EDIT -->` / `// asstudio:generated v1 DO NOT EDIT`.

### App model v1 (JSON; document the schema, then implement)

```json
{
  "app": { "name": "Hello", "package": "com.oji.hello", "minSdk": 21 },
  "screens": [{
    "id": "home",
    "states": ["loading", "content", "error"],
    "layout": { "type": "column", "children": [
      { "type": "text",   "id": "title", "text": "Hello" },
      { "type": "button", "id": "retry", "text": "Retry", "onClick": "retry" }
    ]}
  }]
}
```

Allowed widget types in v1: `column`, `row`, `text`, `button`, `input`, `image`, `spacer`. Common attributes: `id`, `width`/`height` (`match_parent`, `wrap_content`, or `NNdp`), `margin`, `padding`, `text`, `onClick`. Add a `"schemaVersion": 1` field. A screen with `states` gets a root container holding three state views (`loading` = centered ProgressBar, `content` = the screen layout, `error` = message + retry); `setState` shows exactly one. Keep this convention simple and document it. Reserve (but do not implement) `"backend": "jni_json_bridge"` on a screen.

### Validator (`docs/03-validator-rules.md`)

Every rule has a stable id, a severity, and reports a **JSON path** to the offending element plus a human message. Implement at least:

| Id | Rule |
|---|---|
| V001 | ids are unique within a screen |
| V002 | every `onClick` name is a valid Java identifier and unique per screen |
| V003 | widget `type` is in the allowed set |
| V004 | package name is a valid Java package; app name non-empty |
| V005 | screen ids are unique and valid as file/class names |
| V006 | `states` only contains known state names; non-empty if present |
| V007 | `minSdk` is within a supported range (21 to 35) |
| V008 | ids are valid Android resource names (lowercase letters, digits, underscores; not starting with a digit) |

Document the principle: **every crash found on a real phone becomes a new rule.** Keep a `docs/CRASH_LOG.md` (empty table: date, symptom, cause, rule id).

### Codegen and templates (`docs/04-codegen-and-templates.md`)

Templates live in `templates/` (existing subfolders `android`, `java`, `xml`, `gradle`, `cpp`). Phase 1 uses a tiny **own** renderer: `{{dotted.path}}` substitution from a JSON value, no loops (loops/fragments are built in C++ and passed in as strings). Decide later whether to adopt `inja`; record it as an open decision. Template ideas to document: sidebar navigation, screen states, button styles, layouts, JNI bridge.

### JNI JSON bridge (`docs/05-jni-json-bridge.md`, design only in Phase 1)

Pattern: Java sends only a **request string plus an output path** across JNI; native code does the work and writes a **JSON file** to that path; Java reads the file. Benefits: tiny JNI surface, easy to template, easy to test on desktop. The JNI glue is a thin C ABI (`jni_bridge.c` plus a C header). Backend logic sits behind that header and can be **C first, then Odin or Zig** as optional "flavor" templates (Odin/Zig struggle with bionic on Android 10 and below, so keep the C ABI boundary). Add a template stub folder `templates/cpp/jni_json_bridge/` with a README describing the planned files; do not implement.

## 4. Stack and libraries (`docs/06-stack-and-libs.md`)

| Job | Choice | Notes |
|---|---|---|
| Core, validator, codegen | **C++** | C++23 if the installed toolchain supports `std::expected`; otherwise C++20 plus a small `Result<T,E>` in `src/core/expected/`. Never go below C++17. |
| JSON | nlohmann/json (single header) | if compile times hurt, consider yyjson later |
| XML | pugixml | tiny; vendored (3 files) |
| Templates | own `{{path}}` renderer | `inja` is a later option |
| Java parsing | tree-sitter + tree-sitter-java | **later** (round-trip importer) |
| Tests | doctest (single header) | |
| Build | CMake plus Ninja if available | |
| Later UI | Dear ImGui (docking branch) | not in Phase 1 |

Explicitly **not** in the tool itself: Go/Wails/TS, Rust, Python. (Python is allowed as a throwaway for prototyping templates.) Odin and Zig appear only as optional JNI backend template flavors.

### C++ safety and style standards (put in `AGENTS.md` and enforce in CMake)

- Warnings on: `/W4` (MSVC) or `-Wall -Wextra -Wpedantic -Wshadow -Wconversion` (GCC/Clang). Treat as errors only for new code you add; do not fail the build on third-party headers (mark them `SYSTEM`).
- No owning raw pointers; use values, `std::unique_ptr`, `std::string_view`, `std::span`, `std::filesystem`. Rule of zero. `enum class`. `const` by default. No `using namespace std;` in headers.
- Errors: return `std::expected` / `Result` across module boundaries; reserve exceptions for truly exceptional failure in `main`. No exceptions thrown through the public API of core libraries.
- Each module is a static library target with a clear public header directory (`include/<module>/...`).
- Add `.clang-format` (simple, 4-space indent) and `.editorconfig`.
- Optional: AddressSanitizer/UBSan via a CMake option on GCC/Clang.

### Tool installation (you do this)

Check what exists first: `cmake --version`, `ninja --version`, compiler (`cl`, `g++`, or `clang++`), `git --version`. Install missing essentials with `winget`/`choco`/`scoop` or the toolchain's installer if available; if you cannot install something, say so in `docs/STATUS.md` and use a fallback (e.g. default generator instead of Ninja). **Vendor** libraries into `third_party/` (nlohmann/json single header, doctest single header, pugixml source) at pinned versions rather than depending on a system package manager; record versions and licenses in `third_party/README.md`. Do not install anything heavy (no full Android Studio, no Qt, no vcpkg unless truly needed). Java/Gradle/Android SDK are **not required** this session; if present, you may use them for an optional smoke test.

## 5. UI plan (`docs/07-ui-plan.md`, documentation only)

- **Dear ImGui (docking)** with a custom canvas: immediate-mode, light on weak hardware, distinct tool-like look, good for the panels plus a side-by-side code view.
- The **preview is our own renderer** drawing the model with approximate Material widgets, not an Android runtime. The validator keeps the preview honest.
- Fast feedback has two layers: (1) instant approximate canvas preview; (2) a "Run on device" loop: generate, `gradlew installDebug`, `adb logcat` piped into the tool, with crashes shown next to the model element that caused them (see concept image 1).
- The UI never owns state; it edits the model and re-runs validate/generate.

## 6. Roadmap (`docs/08-roadmap.md`)

1. **Phase 1 (this session):** model, validator, codegen for the sample, CLI, tests, docs.
2. Phase 2: full Gradle project skeleton generation (settings/build files, themes, strings), `jni_json_bridge` template, template library, more widgets, build from CLI plus install via adb.
3. Phase 3: ImGui shell, model tree, canvas preview, property editing, code view with widget-to-lines mapping; run-on-device with live logcat mapping.
4. Phase 4: XML importer (round-trip, XML first), then Java via tree-sitter; ConstraintLayout; Odin/Zig backend flavors.

## 7. Phase 1 tasks (the actual coding, keep it small)

1. **Inspect and report.** Summarize existing repo state in `docs/STATUS.md` before changing anything.
2. **Golden sample first.** Hand-write the *expected* output for `samples/hello.json` in `docs/golden/hello/` (layout XML, `HomeScreenBase.java`, `HomeActivity.java`, manifest). Follow real Android conventions (AppCompatActivity, `R.id.*`, `R.layout.*`). It must be plausible Android code that would build; do not require an actual Gradle build.
3. **Build system.** Make CMake configure and build cleanly with the targets/layers from section 3. Add `ctest`.
4. **Model + loader.** Parse `samples/hello.json` into C++ structs (simple value types). Report parse errors with a JSON path.
5. **Validator.** Rules V001 to V008 with tests, including at least one invalid sample per rule family in `samples/invalid/`.
6. **Codegen.** Generate the four artifacts above for the sample into a chosen output folder; user-owned `HomeActivity.java` is skipped if it already exists. Test: generated output equals `docs/golden/hello/` (normalize line endings).
7. **CLI** (existing app target, `src/app`):
   - `AndroidScaffoldStudio validate <model.json>`: exit 0 if valid; otherwise print `[V001] $.screens[0].layout.children[1].id: duplicate id "retry"` and exit 1
   - `AndroidScaffoldStudio generate <model.json> -o <outdir>`: validate first; refuse to generate on errors
   - `AndroidScaffoldStudio --help`
8. **Docs.** Write every `docs/*.md` listed above, plus `AGENTS.md` at the repo root (conventions, how to build/test, layering rule, "every crash becomes a rule", where the plan lives) and `docs/DECISIONS.md` (dated decision log; seed it with: round-trip is the goal but v1 is generate-only; generated/user split; C++ layering; own template renderer first; ImGui later; C ABI boundary for JNI backends; Java+XML+Activities only in v1).
9. **Status.** End by updating `docs/STATUS.md`: what was done, what was verified (exact commands run and their results), what was *not* done, known problems, and the next three suggested steps.

## 8. Acceptance criteria

- Fresh configure and build succeed with the detected toolchain, no warnings in our own code.
- `ctest` passes; tests cover loader, each validator rule, and the golden comparison.
- `validate samples/hello.json` exits 0; each invalid sample exits 1 with the expected rule id and path.
- `generate samples/hello.json -o generated/hello` produces files identical to `docs/golden/hello/`.
- All docs and `AGENTS.md` exist and contain the plan above in your own organized words (nothing important from this prompt should live only in this prompt).
- Nothing outside `docs/`, `src/`, `templates/`, `samples/`, `tests/`, `third_party/`, `cmake/`, build files, and root config files was changed.

If something fails, fix it or record it honestly in `docs/STATUS.md`. Do not claim verification you did not perform.
