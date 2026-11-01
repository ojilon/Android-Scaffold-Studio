#pragma once

#include <string>
#include <vector>
#include <core/FileSystem.h>

namespace project{
    struct Module{
        std::string name;
    };

    struct Project{
        std::string name;
        std::string package_name;
        filesystem::Path root_directory;
        std::vector<Module> modules;
    };

}