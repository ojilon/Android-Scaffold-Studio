#pragma once

/*
============================================================

Module:
    Parser

Purpose:
    Java source parser.

============================================================
*/

#include <core/Core.h>

namespace parser
{

result::VoidResult
parse_java(
    const filesystem::Path& file
);

}