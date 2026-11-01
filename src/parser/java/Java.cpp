/*
============================================================

Module:
    parser

File:
    Java.cpp

Purpose:
    Java source reader.

Status:
    Phase 1 stub. The importer pipeline will be:

        Java source -> tokenizer -> AST -> internal model

    tree-sitter-java is a Phase 4 dependency; nothing is
    vendored for it yet on purpose.

============================================================
*/

#include <parser/Java.h>

namespace parser {

core::VoidResult parse_java(const core::fs::Path& file) {
    return std::unexpected("Java parsing is not implemented yet (Phase 4 importer): " +
                           file.string());
}

} // namespace parser
