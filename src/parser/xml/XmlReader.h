#pragma once

/*
============================================================

Purpose

Reads Android XML into WidgetTree.

============================================================
*/

#include <core/Core.h>

#include <parser/WidgetTree.h>

#include <pugixml.hpp>

namespace parser::xml
{

class XmlReader
{
public:

    result::Result<Layout>
    read(
        const filesystem::Path& file
    );

private:

    Widget
    read_widget(
        const pugi::xml_node& node
    );

    void
    read_properties(
        const pugi::xml_node& node,
        Widget& widget
    );

};

}