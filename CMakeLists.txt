cmake_minimum_required(VERSION 3.25)

project(
    AndroidScaffoldStudio
    VERSION 0.1.0
    DESCRIPTION "Android UI/UX design tool that validates a model and generates Android files"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH
    "${PROJECT_SOURCE_DIR}/cmake"
)

include(CompilerOptions)
include(Warnings)
include(Dependencies)
include(Utils)

# CMake 4.4+ reserves the "codegen" target name (policy CMP0171). The codegen
# module is named that on purpose, so keep the previous behaviour when the
# policy exists; older CMake does not have it and needs no guard.
if(POLICY CMP0171)
    cmake_policy(SET CMP0171 OLD)
endif()

# Layering (enforced by target_link_libraries, see docs/01-architecture.md):
#   core <- parser <- validator <- codegen <- scaffolder
add_subdirectory(src/core)
add_subdirectory(src/parser)
add_subdirectory(src/validator)
add_subdirectory(src/codegen)
add_subdirectory(src/scaffolder)

add_executable(AndroidScaffoldStudio
    src/app/main.cpp
)

target_link_libraries(AndroidScaffoldStudio
    PRIVATE
    scaffolder
    codegen
    validator
    parser
    core
)

target_compile_definitions(AndroidScaffoldStudio
    PRIVATE ASSTUDIO_VERSION="${PROJECT_VERSION}"
)

target_apply_warnings(AndroidScaffoldStudio)

# ---- Tests ----
# One executable per test file: each defines its own doctest main.
include(CTest)

function(add_scaffold_test NAME)
    add_executable(${NAME} tests/${NAME}.cpp)
    target_link_libraries(${NAME} PRIVATE ${ARGN})
    target_apply_warnings(${NAME})
    add_test(NAME ${NAME} COMMAND ${NAME})
    # Tests read samples/ and docs/golden/ relative to the source root.
    set_tests_properties(${NAME} PROPERTIES WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endfunction()

add_scaffold_test(test_parser    parser core)
add_scaffold_test(test_validator validator parser core)
add_scaffold_test(test_codegen   scaffolder codegen validator parser core)
