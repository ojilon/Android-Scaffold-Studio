#pragma once

#include <core/Result.h>

#include <filesystem>
#include <vector>

namespace filesystem {
    using Path = std::filesystem::path;

    expected::Result<bool>  exists(const Path& path);

    expected::VoidResult create_directory(const Path& path);

    expected::VoidResult remove(const Path& path);

    expected::VoidResult copy_file( const Path& from, const Path& to );

    expected::Result<std::vector<Path>> list_directory(const Path& path);

    expected::Result<Path> canonical(const Path& path);

    /*
    join() cannot genuinyl fail, Path is returned,
    not expected::Result,Path>
    */
    Path join(
        const Path& left,
        const Path& right
    );

}