#include <scaffolder/TemplateEngine.h>

namespace scaffolder
{

std::string
TemplateEngine::render(
    const std::string& text,
    const Variables& variables)
{
    std::string output = text;

    /*
    Very simple implementation.

    Future:

    loops

    conditions

    escaping

    includes

    */

    for (const auto& variable : variables)
    {
        std::string key =
            "${" +
            variable.first +
            "}";

        std::size_t position = 0;

        while ((position =
            output.find(
                key,
                position))
            != std::string::npos)
        {
            output.replace(
                position,
                key.size(),
                variable.second);

            position +=
                variable.second.size();
        }
    }

    return output;
}

}