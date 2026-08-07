#pragma once

/*
============================================================

Purpose

Writes WidgetTree into Android XML.

============================================================
*/

#include <core/Core.h>

#include <parser/WidgetTree.h>

#include <pugixml.hpp>

namespace parser::xml
{

class XmlWriter
{
public:

    result::VoidResult
    write(
        const Layout& layout,
        const filesystem::Path& file
    );

private:

    void
    write_widget(
        const Widget& widget,
        pugi::xml_node& node
    );

    void
    write_properties(
        const Widget& widget,
        pugi::xml_node& node
    );

};

}