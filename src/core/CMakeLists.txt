add_library(core STATIC)

target_sources(core PRIVATE
    filesystem/FileSystem.cpp
    logger/Logger.cpp
    settings/Settings.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_apply_warnings(core)
