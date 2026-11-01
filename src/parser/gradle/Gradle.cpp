/*
============================================================

Module:
    parser

File:
    Gradle.cpp

Purpose:
    Gradle file reader.

Status:
    Phase 1 stub. Phase 2 generates the Gradle skeleton from
    templates, so reading Gradle only becomes interesting with
    the round-trip importer.

============================================================
*/

#include <parser/Gradle.h>

namespace parser {

core::VoidResult parse_gradle(const core::fs::Path& file) {
    return std::unexpected("Gradle parsing is not implemented yet (Phase 4 importer): " +
                           file.string());
}

} // namespace parser
