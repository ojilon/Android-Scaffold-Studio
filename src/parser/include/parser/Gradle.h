#pragma once

/*
============================================================

Module:
    parser

File:
    Gradle.h

Purpose:
    Gradle file reader for the future project importer.

Status:
    Phase 1 stub. Full Gradle skeleton generation is Phase 2.

============================================================
*/

#include <core/FileSystem.h>
#include <core/Result.h>

namespace parser {

core::VoidResult parse_gradle(const core::fs::Path& file);

} // namespace parser
