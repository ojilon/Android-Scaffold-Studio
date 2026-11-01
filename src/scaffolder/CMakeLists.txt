add_library(scaffolder STATIC)

target_sources(scaffolder PRIVATE
    template/TemplateLoader.cpp
    writer/ProjectWriter.cpp
)

target_include_directories(scaffolder PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Configured header: records where templates/ lives so the CLI and the tests
# work from any working directory.
set(ASSTUDIO_TEMPLATE_DIR "${PROJECT_SOURCE_DIR}/templates")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/scaffolder/Config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/scaffolder/Config.h
    @ONLY
)

target_include_directories(scaffolder PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_link_libraries(scaffolder PUBLIC codegen parser core)

target_apply_warnings(scaffolder)
