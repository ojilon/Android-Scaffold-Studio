# AGENTS.md - Android Scaffold Studio Conventions

Read this before touching the code. It summarises the plan in
`AGENT_PROMPT.md` and the module docs in `docs/`.

## Build and Test

- **Build system:** CMake (>= 3.25) + Ninja when available.
- **Targets:** one static library per module with a public include dir
  (`src/<module>/include/<module>/...`), plus the `AndroidScaffoldStudio` CLI.
- **Tests:** `ctest`, doctest single header (`third_party/doctest`). One
  executable per test file because each defines its own doctest main.
- **Commands:**
  - Configure: `cmake -B build -S .` (or `cmake --preset default`)
  - Build: `cmake --build build`
  - Test: `ctest --test-dir build --output-on-failure`
  - Warnings as errors: `cmake -B build -S . -DASSTUDIO_WERROR=ON`
  - Sanitizers: `cmake -B build -S . -DASSTUDIO_SANITIZE=ON`
- **Toolchain floor:** C++23 with `<expected>`. Verified on GCC 12.2; the
  owner's machine runs GCC 15.2. Clang 16+ and MSVC 19.33+ also work. Never go
  below C++20, never rely on `<format>` (GCC 12 lacks it), and do not silently
  fall back to C++17.

## C++ Style: Functional, Struct-only

- **No classes.** This is a hard rule from the owner. Model data and results
  are `struct`s with public members and no member functions; behaviour lives in
  free functions in the module's namespace.
  - Bad: `class Validator { public: Result validate(); };`
  - Good: `struct RuleViolation {...}; core::Result<void, std::vector<RuleViolation>> validate(const parser::AppModel&);`
  - A type that needs state between calls is a `struct` the caller owns and
    passes in (`ParseContext`), not a class with hidden fields.
- **Names:** `snake_case` for functions, variables and files' contents;
  `PascalCase` for types; `enum class` for enumerations. Model field names keep
  the JSON spelling (`minSdk`, `onClick`) so the schema and the code match.
- **Ownership:** values, `std::unique_ptr`, `std::string_view`, `std::span`,
  `std::filesystem`. No owning raw pointers. Rule of zero. `const` by default.
  No `using namespace std;` in headers.
- **Errors:** return `core::Result` (= `std::expected`) across module
  boundaries; reserve exceptions for `main` and third-party calls that throw
  (`nlohmann::json`). No exceptions through a public API.
- **Headers:** include only what you use; no heavyweight dependency in many
  translation units (that is why `nlohmann/json.hpp` appears in exactly two
  `.cpp` files and the template header uses `json_fwd.hpp`).
- **Warnings:** `-Wall -Wextra -Wpedantic -Wconversion -Wshadow
  -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual` (GCC/Clang) or
  `/W4 /permissive- /utf-8` (MSVC). Third-party include dirs are `SYSTEM`.
  `-DASSTUDIO_WERROR=ON` is the release gate; keep the tree warning-clean.
- **Formatting:** `.clang-format` (4 spaces, 100 columns) and `.editorconfig`.

## Layering Rule

```
core  <-  parser  <-  validator  <-  codegen  <-  scaffolder  <-  app
```

- `core`: `Result`, logger, filesystem wrappers, settings, project types.
  Knows nothing about Android.
- `parser`: app model structs, JSON loader, tree helpers. Owns the only
  `nlohmann/json` usage for reading.
- `validator`: Android rules over the loaded model. Never parses JSON, never
  generates.
- `codegen`: pure functions that turn (model, templates) into file text. Never
  touches the disk.
- `scaffolder`: loads templates from `templates/` and writes the generated tree
  (the generated / user-owned overwrite policy lives here).
- `app`: the CLI. Nothing else links everything.
- `designer`, `rendering`, `ui` arrive later and only edit the model. Nothing in
  `core`, `parser`, `validator`, `codegen` or `scaffolder` may depend on them.

Dependencies are declared per target in CMake; add a module by adding its
`target_link_libraries`, never by an include path trick.

## Templates and Golden Files

- Templates live in `templates/` (`xml/`, `java/`, `android/`, `gradle/`,
  `cpp/`). `{{dotted.path}}` placeholders only; loops and fragments are built
  in C++ and passed in as pre-rendered strings.
- `docs/golden/<sample>/` holds the hand-written expected output, mirroring the
  output tree. `tests/test_codegen.cpp` compares generated text against it
  (line endings normalised). Change the golden and the generator together, and
  say why in the commit.
- Generated files start with `<!-- asstudio:generated v1 DO NOT EDIT -->` or
  `// asstudio:generated v1 DO NOT EDIT`. User-owned files never get the marker.

## Crash Policy

Every crash found on a real phone becomes a new validator rule with a stable id,
documented in `docs/03-validator-rules.md` and logged in `docs/CRASH_LOG.md`.
Rules are why this tool exists: the model gets fixed, not the generated file.

## Plan Location

The full plan is preserved in `AGENT_PROMPT.md`; the distributed version lives in
`docs/00-vision.md` .. `docs/08-roadmap.md`. Decisions and their reasons go in
`docs/DECISIONS.md`. Current status and known problems go in `docs/STATUS.md`.
