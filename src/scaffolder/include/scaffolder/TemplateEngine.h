#pragma once

/*
============================================================

Purpose

Loads and expands templates.

============================================================
*/

#include <string>

#include <unordered_map>

namespace scaffolder
{

using Variables =
    std::unordered_map
    <
        std::string,
        std::string
    >;

class TemplateEngine
{
public:

    std::string
    render(
        const std::string& text,
        const Variables& variables
    );

};

}

/*
Future

${NAME}

${PACKAGE}

${MIN_SDK}

${TARGET_SDK}

Future extensions

{% if %}

{% for %}

{% include %}

*/