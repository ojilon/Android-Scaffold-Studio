#pragma once

/*
============================================================

Module:
    Scaffolder

Purpose:
    Generates Android resources.

============================================================
*/

#include <core/Core.h>

namespace scaffolder
{

class ResourceGenerator
{
public:

    result::VoidResult
    generate(
        const project::Project& project
    );

private:

    result::VoidResult
    generate_layouts(
        const project::Project& project
    );

    result::VoidResult
    generate_values(
        const project::Project& project
    );

    result::VoidResult
    generate_drawables(
        const project::Project& project
    );

    result::VoidResult
    generate_mipmap(
        const project::Project& project
    );

};

}