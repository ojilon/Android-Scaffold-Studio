#pragma once

/*
============================================================

Module:
    scaffolder

File:
    ProjectWriter.h

Purpose:
    Writes a codegen::GenerationOutput tree to disk.

The generated / user-owned split:
    Generated files (layout XML, <Screen>ScreenBase.java,
    AndroidManifest.xml) are always rewritten; they carry the
    asstudio marker comment.

    User-owned files (<Screen>Activity.java) are written only
    when missing. That is the whole promise of the split: the
    developer's handler bodies are never clobbered, and they
    are reported as skipped so the CLI can say so.

Why here and not in codegen:
    codegen is a pure function of (model, templates) so the
    golden test can compare text without touching the disk.
    Only this module knows about directories and overwrite
    policy.

============================================================
*/

#include <codegen/Generation.h>

#include <core/FileSystem.h>
#include <core/Result.h>

#include <vector>

namespace scaffolder {

struct WriteReport {
    std::vector<core::fs::Path> written; // relative paths
    std::vector<core::fs::Path> skipped; // existing user-owned files
};

core::Result<WriteReport> write_output(const codegen::GenerationOutput& output,
                                       const core::fs::Path& output_directory);

} // namespace scaffolder
