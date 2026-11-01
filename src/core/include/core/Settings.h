#pragma once

#include <core/FileSystem.h>
#include <core/Result.h>

namespace settings {

struct Settings {
    filesystem::Path sdk_directory;
    filesystem::Path ndk_directory;
    filesystem::Path projects_directory;
    bool dark_theme = true;
};

Settings defaults();

core::VoidResult save(const Settings& s, const filesystem::Path& path);
core::Result<Settings> load(const filesystem::Path& path);

} // namespace settings