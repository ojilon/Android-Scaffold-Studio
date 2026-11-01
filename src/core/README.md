# Core Module

Foundation types that every other module is allowed to use. No Android
knowledge, no UI, no JSON.

| Header | Purpose |
|---|---|
| `core/Result.h` | `core::Result<T, E>` = `std::expected`, `core::Error`, `core::VoidResult`. The only error vocabulary across module boundaries. |
| `core/FileSystem.h` | `core::fs`: non-throwing wrappers over `std::filesystem` (`read_text_file`, `write_text_file`, `create_directories`, ...). |
| `core/Logger.h` | `core::logger::debug/info/warning/error`, one process-wide level. |
| `core/Settings.h` | user settings value type (persistence is Phase 3). |
| `core/Project.h` | reserved Android project description for Phase 2. |

Everything here is a struct plus free functions; there are no classes. `core`
depends on nothing but the standard library.
