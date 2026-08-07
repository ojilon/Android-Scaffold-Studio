#pragma once

/*
============================================================

Module:
    Scaffolder

Purpose:
    Shared generation state.

============================================================
*/

#include <filesystem>
#include <string>
#include <unordered_map>

namespace scaffolder
{

using Variables =
    std::unordered_map<
        std::string,
        std::string>;

struct GenerationContext
{
    std::filesystem::path
        template_directory;

    std::filesystem::path
        output_directory;

    Variables variables;

    bool overwrite_existing = true;

    bool create_missing_directories = true;
};

}