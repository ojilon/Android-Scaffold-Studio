#pragma once

/*
============================================================

Purpose

Creates directory hierarchy.

============================================================
*/

#include <core/Core.h>

namespace scaffolder
{

class DirectoryBuilder
{
public:

    result::VoidResult
    build(
        const project::Project& project
    );

};

}