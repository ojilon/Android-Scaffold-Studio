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

# CMake 4.4 reserves "codegen" as a target name (policy CMP0171); the
# codegen module intentionally keeps that name, so allow it.
cmake_policy(SET CMP0171 OLD)

# Layering: core <- parser <- validator <- codegen/scaffolder
add_subdirectory(src/core)
add_subdirectory(src/parser)
add_subdirectory(src/validator)
add_subdirectory(src/codegen)

add_executable(
    AndroidScaffoldStudio
    src/app/main.cpp
)

# link the static libraries
target_link_libraries(AndroidScaffoldStudio
    PRIVATE
    core
    parser
    validator
    codegen
)

#target_apply_compiler_options(AndroidScaffoldStudio)
target_apply_warnings(AndroidScaffoldStudio)

target_compile_features(
    AndroidScaffoldStudio
    PRIVATE
        cxx_std_23
)

# ---- Tests (doctest, one executable per test file since each defines its own main) ----
enable_testing()

function(add_scaffold_test NAME)
    add_executable(${NAME} tests/${NAME}.cpp)
    target_compile_features(${NAME} PRIVATE cxx_std_23)
    target_apply_warnings(${NAME})
    target_link_libraries(${NAME} PRIVATE ${ARGN})
    add_test(NAME ${NAME} COMMAND ${NAME})
    # Tests resolve samples/ and docs/golden/ relative to the source root
    set_tests_properties(${NAME} PROPERTIES WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endfunction()

add_scaffold_test(test_parser    parser core)
add_scaffold_test(test_validator validator core)
add_scaffold_test(test_codegen   codegen parser core)
