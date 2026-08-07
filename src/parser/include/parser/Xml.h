#pragma once

/*
============================================================

Module:
    Parser

Purpose:
    Android XML parser interface.

============================================================
*/

#include <core/Core.h>

#include <parser/WidgetTree.h>

namespace parser
{

result::Result<Layout>
parse_xml(
    const filesystem::Path& xml_file
);

result::VoidResult
write_xml(
    const Layout& layout,
    const filesystem::Path& output_file
);

}