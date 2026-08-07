#pragma once

/*
============================================================

Purpose

Writes generated files.

============================================================
*/

#include <core/Core.h>

namespace scaffolder
{

class ProjectWriter
{
public:

    result::VoidResult
    write(
        const project::Project& project
    );

};

}