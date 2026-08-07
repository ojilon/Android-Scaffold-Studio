#pragma once

/*
============================================================

Module:
    Parser

File:
    WidgetTree.h

Purpose:
    Internal representation of an Android layout.

============================================================
*/

#include <string>
#include <unordered_map>
#include <vector>

namespace parser
{

using PropertyMap =
    std::unordered_map<std::string, std::string>;

struct Widget
{
    std::string type;

    std::string id;

    PropertyMap properties;

    std::vector<Widget> children;
};

struct Layout
{
    std::string name;

    Widget root;
};

}