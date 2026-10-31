cmake_minimum_required(VERSION 4.0)

project(
    AndroidScaffoldStudio
    VERSION 0.1.0
    DESCRIPTION "Android application scaffolding tool"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/cmake"
)

include(CompilerOptions)
include(Warnings)
include(Dependencies)
include(Utils)

#add static libraries
add_subdirectory(src/core)
add_subdirectory(src/validator)

add_executable(
    AndroidScaffoldStudio
    src/app/main.cpp
)

#link the static libraries
target_link_libraries(AndroidScaffoldStudio
    PRIVATE
    core
    validator
)

#target_apply_compiler_options(AndroidScaffoldStudio)
target_apply_warnings(AndroidScaffoldStudio)

target_compile_features(
    AndroidScaffoldStudio
    PRIVATE
        cxx_std_23
)