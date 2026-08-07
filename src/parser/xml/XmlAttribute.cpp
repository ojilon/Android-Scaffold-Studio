#include "XmlAttribute.h"

namespace parser::xml
{

bool is_android_attribute(
    const std::string& name)
{
    return name.starts_with("android:");
}

bool is_tools_attribute(
    const std::string& name)
{
    return name.starts_with("tools:");
}

bool is_app_attribute(
    const std::string& name)
{
    return name.starts_with("app:");
}

}