#pragma once

#include <string>
#include <vector>
#include <core/Filesystem.h>

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