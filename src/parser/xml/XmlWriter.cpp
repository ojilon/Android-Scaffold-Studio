#include "XmlWriter.h"

namespace parser::xml
{

result::VoidResult
XmlWriter::write(
    const Layout& layout,
    const filesystem::Path& file)
{
    /*
    TODO

    Create document

    Write root

    Save

    */

    (void)layout;
    (void)file;

    return std::unexpected(
        "Not implemented");
}

void
XmlWriter::write_widget(
    const Widget& widget,
    pugi::xml_node& node)
{
    write_properties(
        widget,
        node);

    for (const auto& child :
         widget.children)
    {
        auto child_node =
            node.append_child(
                child.type.c_str());

        write_widget(
            child,
            child_node);
    }
}

void
XmlWriter::write_properties(
    const Widget& widget,
    pugi::xml_node& node)
{
    for (const auto& property :
         widget.properties)
    {
        node.append_attribute(
            property.first.c_str())
            =
            property.second.c_str();
    }
}

}