#include "XmlReader.h"

namespace parser::xml
{

result::Result<Layout>
XmlReader::read(
    const filesystem::Path& file)
{
    /*
    TODO

    Load XML

    Validate

    Read root

    Return Layout
    */

    return std::unexpected(
        "Not implemented");
}

Widget
XmlReader::read_widget(
    const pugi::xml_node& node)
{
    Widget widget;

    widget.type = node.name();

    read_properties(
        node,
        widget);

    for (const auto& child :
         node.children())
    {
        if (child.type() !=
            pugi::node_element)
        {
            continue;
        }

        widget.children.push_back(
            read_widget(child));
    }

    return widget;
}

void
XmlReader::read_properties(
    const pugi::xml_node& node,
    Widget& widget)
{
    for (const auto& attribute :
         node.attributes())
    {
        widget.properties
            [attribute.name()]
            = attribute.value();

        if (attribute.name() ==
            std::string("android:id"))
        {
            widget.id =
                attribute.value();
        }
    }
}

}