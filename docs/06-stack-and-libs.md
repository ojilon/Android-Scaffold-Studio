# Stack and Libraries

## Core, Validator, Codegen
- **Language:** C++ (C++23 if `std::expected` supported; otherwise C++20 + small `Result<T,E>` in `src/core/expected/`. Never below C++17.)

## Dependencies (Vendored in `third_party/`)
- **JSON:** nlohmann/json (single header) - pinned version with license in `third_party/README.md`
- **XML:** pugixml (tiny; vendored 3 files)
- **Templates:** own `{{path}}` renderer (inja is later option)
- **Java parsing:** tree-sitter + tree-sitter-java (**later** - round-trip importer)
- **Tests:** doctest (single header)
- **Build:** CMake plus Ninja if available

## Explicitly Excluded
- Go/Wails/TS, Rust, Python (Python allowed for prototyping templates only)
- Odin and Zig appear only as optional JNI backend template flavors
- No full Android Studio, no Qt, no vcpkg unless truly needed
- Java/Gradle/Android SDK not required this session (optional smoke test if present)

## Tool Installation (Agent Task)
Check: `cmake --version`, `ninja --version`, compiler (`cl`, `g++`, or `clang++`), `git --version`.
Install missing essentials with `winget`/`choco`/`scoop` or toolchain installer.
Record versions and licenses in `third_party/README.md`.
