# Stack and Libraries

## Language and Toolchain

- **C++23** is required. `core::Result` is `std::expected`, so the compiler must
  ship `<expected>`: GCC 12+, Clang 16+, MSVC 19.33+.
  - Verified here on **GCC 12.2.0** (`std::expected` present, builds and tests
    clean).
  - The owner's machine runs **GCC 15.2.0**, where `std::expected` and the other
    C++23 features used here are available.
  - If a machine's toolchain lacks `<expected>`, the documented fallback is
    C++20 plus a small `Result<T, E>`; that branch is intentionally *not* in the
    tree, so there is one behaviour to test.
- **No `<format>`.** GCC 12 does not ship it, and the logger does not need it.
  `std::ostringstream` + `std::strftime` keep the build portable and fast to
  compile.
- **Warnings:** see AGENTS.md. `-DASSTUDIO_WERROR=ON` for release/CI.
  `-DASSTUDIO_SANITIZE=ON` enables ASan+UBSan on GCC/Clang (verified clean).

## Build

- **CMake** >= 3.25, **Ninja** when available (`CMakePresets.json` has
  `default`, `release`, `asan`).
- One static library per module, one `ctest` executable per test file.

## Vendored Dependencies (`third_party/`, pinned)

| Library | Version | Files | Used by |
|---|---|---|---|
| nlohmann/json | 3.11.3 | `json.hpp`, `json_fwd.hpp`, LICENSE.MIT | `parser` (loader), `codegen` (renderer) |
| doctest | 2.4.11 | `doctest.h`, LICENSE.txt | `tests/` |
| pugixml | 1.14 | `pugixml.{hpp,cpp}`, `pugiconfig.hpp`, LICENSE.md | Phase 4 XML importer - not compiled yet |

All headers are included as `SYSTEM` so their warnings never reach our build.
`third_party/README.md` is the canonical version/license record.

Headers are heavy, so the tree limits their reach:

- `<nlohmann/json.hpp>` appears in `parser/AppModelLoader.cpp` and
  `codegen/Template.cpp` only; `codegen/Template.h` uses `json_fwd.hpp`.
- `<doctest/doctest.h>` appears in test translation units only.
- `pugixml` is not built; nothing includes it in Phase 1.

## Explicitly Not Used

- Go/Wails/TS, Rust, Python in the tool itself (Python is fine for throwaway
  template prototyping, not for shipped code).
- Qt, Boost, vcpkg, Android Studio, full Gradle/Android SDK.
- Dear ImGui until Phase 3.
- `inja` template engine (open decision, see docs/DECISIONS.md).
- `std::regex`: libstdc++'s implementation is slow to compile and the validator's
  character rules are a few range checks each.

## Deliberately Deferred

- tree-sitter + tree-sitter-java for the Java importer (Phase 4).
- Gradle dependency resolution: we generate files; the user's Gradle builds them.
