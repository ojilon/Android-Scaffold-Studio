#pragma once

/*
============================================================

Module:
    Scaffolder

Purpose:
    Generates Android project structures.

============================================================
*/

#include <core/Core.h>

namespace scaffolder
{

result::VoidResult
create_project(
    const project::Project& project
);

}