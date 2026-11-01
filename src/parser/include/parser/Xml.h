#pragma once

/*
============================================================

Module:
    parser

File:
    Xml.h

Purpose:
    Android layout XML <-> parser::Layout.

Status:
    Phase 1 stub. The XML importer is Phase 4 (round-trip);
    the generator writes XML itself, so nothing here is on the
    Phase 1 path.

Design note:
    Free functions, no XmlReader/XmlWriter classes: the reader
    and writer have no state to carry between calls, and the
    pugixml document owns the memory anyway.

============================================================
*/

#include <core/FileSystem.h>
#include <core/Result.h>

#include <parser/AppModel.h>

namespace parser {

core::Result<Layout> parse_xml(const core::fs::Path& xml_file);

core::VoidResult write_xml(const Layout& layout, const core::fs::Path& output_file);

} // namespace parser
