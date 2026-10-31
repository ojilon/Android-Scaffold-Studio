# AGENTS.md - Android Scaffold Studio Conventions

## Build and Test
- **Build System:** CMake + Ninja (if available) or default generator.
- **Targets:** Static library targets with clear public header directories (`include/<module>/...`).
- **Tests:** `ctest` (doctest single header).
- **Commands:**
  - Configure: `cmake -B build -S .`
  - Build: `cmake --build build`
  - Test: `ctest --output-on-failure`

## C++ Safety and Style Standards
- **Warnings:** `/W4` (MSVC) or `-Wall -Wextra -Wpedantic -Wshadow -Wconversion` (GCC/Clang). Treat as errors for new code; mark third-party headers as `SYSTEM`.
- **Pointers:** No owning raw pointers; use values, `std::unique_ptr`, `std::string_view`, `std::span`, `std::filesystem`. Rule of zero. `enum class`. `const` by default. No `using namespace std;` in headers.
- **Errors:** return `std::expected` / `Result` across module boundaries; reserve exceptions for truly exceptional failure in `main`. No exceptions thrown through public API.
- **Formatting:** `.clang-format` (4-space indent) and `.editorconfig`.
- **Optional:** AddressSanitizer/UBSan via CMake option on GCC/Clang.

## Layering Rule
`core` <- `model/parser` <- `validator` <- `codegen/scaffolder`.
`designer`, `rendering`, `ui` sit on top later and only edit the model.
Nothing in `core`, `parser`, `scaffolder` or `codegen` may depend on `ui`, `rendering`, or `designer`.

## Crash Policy
Every crash found on a real phone becomes a new rule (documented in `docs/CRASH_LOG.md`).

## Plan Location
The full plan is preserved in `AGENT_PROMPT.md` and distributed into `docs/*.md` files.
