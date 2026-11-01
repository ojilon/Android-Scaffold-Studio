#pragma once

/*
============================================================

Module:
    core

File:
    Project.h

Purpose:
    The on-disk Android project the tool writes into.

Status:
    Reserved for Phase 2 (full Gradle skeleton). Phase 1
    generates only the artefacts listed in docs/01-architecture.md,
    so nothing here is used yet.

============================================================
*/

#include <core/FileSystem.h>

#include <string>
#include <vector>

namespace core::project {

struct Module {
    std::string name;
};

struct Project {
    std::string name;
    std::string package_name;
    fs::Path root_directory;
    std::vector<Module> modules;
};

} // namespace core::project
