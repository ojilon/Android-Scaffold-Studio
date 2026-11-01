#pragma once

/*
============================================================

Module:
    parser

File:
    Java.h

Purpose:
    Java source reader for the future round-trip importer.

Status:
    Phase 1 stub. Java is the last importer to build
    (tree-sitter-java); v1 is model -> generate only.

============================================================
*/

#include <core/FileSystem.h>
#include <core/Result.h>

namespace parser {

core::VoidResult parse_java(const core::fs::Path& file);

} // namespace parser
