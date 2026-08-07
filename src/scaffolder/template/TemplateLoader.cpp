/*
============================================================

Purpose

Loads templates from disk.

Future

zip templates

embedded templates

remote templates

============================================================
*/

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace scaffolder
{

std::string
load_template(
    const std::filesystem::path& file)
{
    /*
    TODO

    Open file

    Read text

    Return

    */

    std::ifstream stream(file);

    if (!stream)
    {
        return {};
    }

    std::stringstream buffer;

    buffer << stream.rdbuf();

    return buffer.str();
}

}