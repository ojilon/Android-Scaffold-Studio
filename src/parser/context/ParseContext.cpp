#include <parser/ParseContext.h>

namespace parser
{

void ParseContext::add_error(
    ParseError error)
{
    errors.push_back(
        std::move(error));
}

bool ParseContext::has_errors() const
{
    return !errors.empty();
}

}