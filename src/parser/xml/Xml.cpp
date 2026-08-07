#include "XmlReader.h"
#include "XmlWriter.h"

#include <parser/Xml.h>

namespace parser
{

result::Result<Layout>
parse_xml(
    const filesystem::Path& file)
{
    xml::XmlReader reader;

    return reader.read(file);
}

result::VoidResult
write_xml(
    const Layout& layout,
    const filesystem::Path& file)
{
    xml::XmlWriter writer;

    return writer.write(
        layout,
        file);
}

}