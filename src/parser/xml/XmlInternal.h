#pragma once

/*
============================================================

Module:
    Parser

File:
    XmlInternal.h

Purpose:
    Internal recursive XML helper functions.

Notes:
    This header is PRIVATE.
    Only Xml.cpp should include it.

============================================================
*/

#include <parser/WidgetTree.h>

#include <pugixml.hpp>

namespace parser::xml
{

// Reads one XML node recursively.
Widget read_widget(
    const pugi::xml_node& node
);

// Writes one Widget recursively.
void write_widget(
    const Widget& widget,
    pugi::xml_node& node
);

// Copies all XML attributes into PropertyMap.
void read_properties(
    const pugi::xml_node& node,
    Widget& widget
);

// Writes PropertyMap back into XML.
void write_properties(
    const Widget& widget,
    pugi::xml_node& node
);

}