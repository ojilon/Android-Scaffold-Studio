/*
jobs:

create folders
copy files
delete files
enumerate directories
normalize paths
report errors
*/

#include <core/Filesystem.h>

namespace filesystem {
    expected::Result<bool>
    exists(const Path& path)
    {
        std::error_code ec;

        bool value = std::filesystem::exists(path, ec);

        if (ec)
            return std::unexpected(ec.message());

        return value;
    }

    expected::VoidResult create_directory(const Path& path) {
        std::error_code ec;
        std::filesystem::create_directories(path, ec);

        if (ec) {
            return std::unexpected(ec.message());
        }

        return {};
    }

    expected::Result<Path>
    canonical(const Path& path)
    {
        std::error_code ec;

        auto result =
            std::filesystem::canonical(path, ec);

        if (ec)
            return std::unexpected(ec.message());

        return result;
    }

    result::Result<std::vector<Path>>
    filesystem::list_directory(
        const Path& path)
    {
        std::error_code ec;

        std::vector<Path> files;

        for (const auto& entry :
             std::filesystem::directory_iterator(path, ec))
        {
            files.push_back(entry.path());
        }

        if (ec)
        {
            return std::unexpected(ec.message());
        }

        return files;
    }



    Path join(
        const Path& left,
        const Path& right
    )
    {
        return left / right;
    }

}