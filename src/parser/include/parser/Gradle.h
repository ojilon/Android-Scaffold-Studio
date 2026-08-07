#pragma once

/*
============================================================

Module:
    Parser

Purpose:
    Gradle parser.

============================================================
*/

#include <core/Core.h>

namespace parser
{

result::VoidResult
parse_gradle(
    const filesystem::Path& file
);

}