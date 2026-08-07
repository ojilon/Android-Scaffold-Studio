#include <scaffolder/ProjectValidator.h>

namespace scaffolder
{

result::VoidResult
ProjectValidator::validate(
    const project::Project& project)
{
    (void)project;

    /*
    Future

    Project name

    Package name

    SDK versions

    Duplicate resources

    Invalid identifiers

    Missing templates

    */

    return {};
}

result::VoidResult
ProjectValidator::validate_name(
    const project::Project&)
{
    return {};
}

result::VoidResult
ProjectValidator::validate_package(
    const project::Project&)
{
    return {};
}

result::VoidResult
ProjectValidator::validate_sdk(
    const project::Project&)
{
    return {};
}

}