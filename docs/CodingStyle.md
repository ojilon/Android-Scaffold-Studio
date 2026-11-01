# Coding Style

The authoritative version is in [../AGENTS.md](../AGENTS.md) ("C++ Style:
Functional, Struct-only"). Short form:

- **No classes.** Data is `struct`s with public members and no member functions;
  behaviour is free functions in the module's namespace. A type that needs state
  between calls is a struct the caller owns and passes in.
- `snake_case` functions/variables, `PascalCase` types, `enum class`, `const` by
  default, no `using namespace std;` in headers.
- `core::Result` (`std::expected`) across module boundaries; exceptions only in
  `main` and around third-party calls that throw.
- Values, `std::unique_ptr`, `std::string_view`, `std::span`, `std::filesystem`;
  no owning raw pointers; rule of zero.
- Include only what you use: the heavy headers (`nlohmann/json.hpp`,
  `<doctest/doctest.h>`) appear in a couple of `.cpp` files only.
- Formatting: `.clang-format` (4 spaces, 100 columns) and `.editorconfig`.
- Warnings: `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wnon-virtual-dtor
  -Wold-style-cast -Woverloaded-virtual`; `-DASSTUDIO_WERROR=ON` must stay green.
- Comments explain *why*, not *what*.
