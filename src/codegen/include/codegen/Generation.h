#pragma once

/*
============================================================

Module:
    codegen

File:
    Generation.h

Purpose:
    App model -> the text of every generated artefact.

What this module does NOT do:
    Touch the disk. generation is a pure function of
    (model, templates), which makes the golden test trivial
    and lets the future UI preview generated text before it
    is written. Writing the tree is scaffolder's job.

Artefacts per screen (see docs/01-architecture.md):
    res/layout/<screen>.xml                    generated
    java/<package path>/<Screen>ScreenBase.java generated
    java/<package path>/<Screen>Activity.java   user-owned
    AndroidManifest.xml                         generated

============================================================
*/

#include <core/FileSystem.h>
#include <core/Result.h>
#include <parser/AppModel.h>

#include <string>
#include <vector>

namespace codegen {

// Template text, loaded from templates/. Keeping them outside the module
// means the template library (Phase 2) can add flavours without a rebuild.
struct TemplateSet {
    std::string layout;        // templates/xml/layout.xml.in
    std::string layout_states; // templates/xml/layout_states.xml.in
    std::string screen_base;   // templates/java/ScreenBase.java.in
    std::string activity;      // templates/java/Activity.java.in
    std::string manifest;      // templates/android/AndroidManifest.xml.in
};

struct GeneratedFile {
    core::fs::Path relative_path; // relative to the output directory
    std::string content;
    // true  => user-owned: written only when the file does not exist yet.
    // false => generated: always rewritten.
    bool user_owned = false;
};

struct GenerationOutput {
    std::vector<GeneratedFile> files;
};

core::Result<GenerationOutput> generate(const parser::AppModel& model,
                                       const TemplateSet& templates);

} // namespace codegen
