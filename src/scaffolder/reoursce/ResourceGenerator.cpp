#include <scaffolder/ResourceGenerator.h>

namespace scaffolder
{

result::VoidResult
ResourceGenerator::generate(
    const project::Project& project)
{
    (void)project;

    /*
    Generation order

    layouts

    values

    drawable

    mipmap

    menu

    navigation

    */

    return {};
}

result::VoidResult
ResourceGenerator::generate_layouts(
    const project::Project&)
{
    return {};
}

result::VoidResult
ResourceGenerator::generate_values(
    const project::Project&)
{
    return {};
}

result::VoidResult
ResourceGenerator::generate_drawables(
    const project::Project&)
{
    return {};
}

result::VoidResult
ResourceGenerator::generate_mipmap(
    const project::Project&)
{
    return {};
}

}