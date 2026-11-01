#pragma once

/*
============================================================

Module:
    core

File:
    FileSystem.h

Purpose:
    Thin, non-throwing wrappers around <filesystem>.

Why:
    std::filesystem throws on failure and has overloads that
    silently ignore errors. Both are easy to misuse. Every
    operation here reports through core::Result so callers
    stay linear and checkable.

Naming:
    The namespace is core::fs, not `filesystem`, so that
    std::filesystem can be used alongside it without the two
    names reading the same.

============================================================
*/

#include <core/Result.h>

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace core::fs {

using Path = std::filesystem::path;

// Cannot fail.
Path join(const Path& left, const Path& right);

Result<bool> exists(const Path& path);

VoidResult create_directories(const Path& path);

VoidResult remove_all(const Path& path);

VoidResult copy_file(const Path& from, const Path& to);

Result<std::vector<Path>> list_directory(const Path& path);

Result<Path> canonical(const Path& path);

// Reads the whole file. Bytes are returned untouched (no line-ending or
// encoding conversion), so generated output stays byte-for-byte stable.
Result<std::string> read_text_file(const Path& path);

// Writes the whole file, creating missing parent directories first.
VoidResult write_text_file(const Path& path, std::string_view content);

} // namespace core::fs
