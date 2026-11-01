add_library(codegen STATIC)

target_sources(codegen PRIVATE
    Generation.cpp
    Template.cpp
)

target_include_directories(codegen PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(codegen PUBLIC parser core)

target_apply_warnings(codegen)
