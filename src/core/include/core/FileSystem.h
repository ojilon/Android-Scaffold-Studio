#pragma once

#include <core/Result.h>
#include <filesystem>
#include <vector>

namespace filesystem {

using Path = std::filesystem::path;

core::Result<bool>               exists(const Path& path);
core::VoidResult                 create_directory(const Path& path);
core::VoidResult                 remove(const Path& path);
core::VoidResult                 copy_file(const Path& from, const Path& to);
core::Result<std::vector<Path>>  list_directory(const Path& path);
core::Result<Path>               canonical(const Path& path);

// cannot fail
Path join(const Path& left, const Path& right);

} // namespace filesystem