#pragma once

/*
============================================================

Module:
    core

File:
    Settings.h

Purpose:
    User settings (SDK/NDK locations, last project folder,
    theme). Values only; behaviour lives in free functions.

Status:
    Phase 1 stores nothing. save()/load() report "not
    implemented yet" so the CLI can degrade instead of
    crashing once a settings file is introduced.

============================================================
*/

#include <core/FileSystem.h>
#include <core/Result.h>

namespace core::settings {

struct Settings {
    fs::Path sdk_directory;
    fs::Path ndk_directory;
    fs::Path projects_directory;
    bool dark_theme = true;
};

Settings defaults();

VoidResult save(const Settings& settings, const fs::Path& path);

Result<Settings> load(const fs::Path& path);

} // namespace core::settings
