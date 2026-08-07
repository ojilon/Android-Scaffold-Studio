#pragma once

/*
============================================================

Module:
    Scaffolder

Purpose:
    Validates a project before generation.

============================================================
*/

#include <core/Core.h>

namespace scaffolder
{

class ProjectValidator
{
public:

    result::VoidResult
    validate(
        const project::Project& project
    );

private:

    result::VoidResult
    validate_name(
        const project::Project& project
    );

    result::VoidResult
    validate_package(
        const project::Project& project
    );

    result::VoidResult
    validate_sdk(
        const project::Project& project
    );
};

}