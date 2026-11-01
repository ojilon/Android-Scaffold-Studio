# core/filesystem

Implementation of `core/FileSystem.h`: non-throwing wrappers over
`std::filesystem` (`read_text_file`, `write_text_file`, `create_directories`,
`list_directory`, ...) returning `core::Result` / `core::VoidResult`.

The alias inside the .cpp is `std_fs`, not `fs`, because `fs` would resolve to
`core::fs` (the namespace being defined) instead of `std::filesystem`.
