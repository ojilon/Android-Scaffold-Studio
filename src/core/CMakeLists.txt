add_library(core STATIC)

target_sources(core PRIVATE
    logger/Logger.cpp
    filesystem/FileSystem.cpp
    settings/Settings.cpp
    #project/Project.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#target_apply_compiler_options(core)
target_apply_warnings(core)