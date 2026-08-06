#pragma once

#include <core/Filesystem.h>
#include <core/Result.h>

namespace settings {
    struct Settings {
        filesystem::Path sdk_directory;
        filesystem::Path ndk_directory;
        filesystem::Path projects_directory;
        bool dark_theme = true;
    };

    Settings defaults();

    result::VoidResult save(
        const Settings&,
        const filesystem::Path&
    );

    result::Result<Settings> load(
        const filesystem::Path&
    );

}