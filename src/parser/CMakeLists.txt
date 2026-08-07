# ============================================================
#
# Module:
#     Parser
#
# Purpose:
#     Converts Android project files into the application's
#     internal data model.
#
# Provides:
#     - XML parser
#     - Java parser
#     - Gradle parser
#
# ============================================================

add_library(parser STATIC)

target_sources(parser
PRIVATE

    xml/Xml.cpp
    java/Java.cpp
    gradle/Gradle.cpp
    tree/TreeUtils.cpp
    context/ParseContext.cpp
    error/ParseError.cpp
)

target_include_directories(parser
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(parser
PUBLIC
    core
)

target_apply_compiler_options(parser)
target_apply_warnings(parser)