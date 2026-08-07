/*
============================================================

Purpose

Writes generated files.

============================================================
*/

#include <filesystem>
#include <fstream>

namespace scaffolder
{

bool
write_file(
    const std::filesystem::path& file,
    const std::string& text)
{
    /*
    TODO

    UTF-8

    CRLF/LF option

    permissions

    */

    std::ofstream stream(file);

    if (!stream)
    {
        return false;
    }

    stream << text;

    return true;
}

}