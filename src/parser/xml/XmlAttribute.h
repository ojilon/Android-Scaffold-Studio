#pragma once

/*
============================================================

Module:
    Parser

File:
    XmlAttribute.h

Purpose:
    XML attribute helper utilities.

============================================================
*/

#include <string>

namespace parser::xml
{

struct Attribute
{
    std::string name;

    std::string value;
};

bool is_android_attribute(
    const std::string& name
);

bool is_tools_attribute(
    const std::string& name
);

bool is_app_attribute(
    const std::string& name
);

}