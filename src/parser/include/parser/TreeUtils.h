#pragma once

/*
============================================================

Module:
    Parser

File:
    TreeUtils.h

Purpose:
    Common WidgetTree operations.

============================================================
*/

#include <parser/WidgetTree.h>

#include <functional>
#include <vector>

namespace parser
{

using WidgetVisitor =
    std::function<void(Widget&)>;

using ConstWidgetVisitor =
    std::function<void(const Widget&)>;

void visit(
    Widget& root,
    WidgetVisitor visitor
);

void visit(
    const Widget& root,
    ConstWidgetVisitor visitor
);

Widget* find_by_id(
    Widget& root,
    const std::string& id
);

const Widget* find_by_id(
    const Widget& root,
    const std::string& id
);

std::vector<Widget*>
find_by_type(
    Widget& root,
    const std::string& type
);

std::size_t
count_widgets(
    const Widget& root
);

}