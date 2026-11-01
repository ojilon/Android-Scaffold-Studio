/*
============================================================

Module:
    parser

File:
    Xml.cpp

Purpose:
    Android layout XML <-> parser::Layout.

Status:
    Phase 1 stub.

Why a stub:
    Phase 1 goes model -> files only. Reading XML back is the
    importer (Phase 4) and it must preserve attributes it does
    not understand, which needs a different in-memory shape
    than the generator's model. Writing it now would be dead
    code with the wrong design.

    Both entry points report a Result instead of throwing, so
    the CLI and tests already handle them.

============================================================
*/

#include <parser/Xml.h>

namespace parser {

core::Result<Layout> parse_xml(const core::fs::Path& xml_file) {
    return std::unexpected("XML parsing is not implemented yet (Phase 4 importer): " +
                           xml_file.string());
}

core::VoidResult write_xml(const Layout& /*layout*/, const core::fs::Path& output_file) {
    return std::unexpected("XML writing is not implemented yet (codegen owns it in Phase 1): " +
                           output_file.string());
}

} // namespace parser
