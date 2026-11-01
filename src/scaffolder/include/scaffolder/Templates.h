#pragma once

/*
============================================================

Module:
    scaffolder

File:
    Templates.h

Purpose:
    Loads the template set that codegen renders.

Where templates live:
    templates/ at the repository root (xml/, java/, android/).
    They are files, not strings compiled into the binary, so
    the Phase 2 template library can add variants without a
    rebuild.

Default location:
    default_template_directory() returns the templates folder
    recorded at configure time, so the CLI and tests work from
    any working directory. The CLI still accepts --templates
    to point somewhere else.

============================================================
*/

#include <codegen/Generation.h>

#include <core/FileSystem.h>
#include <core/Result.h>

namespace scaffolder {

core::fs::Path default_template_directory();

// Reads every template referenced by codegen::TemplateSet.
// Fails with the offending file name when one is missing.
core::Result<codegen::TemplateSet> load_templates(const core::fs::Path& directory);

} // namespace scaffolder
