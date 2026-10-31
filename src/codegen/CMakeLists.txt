add_library(codegen STATIC)
target_include_directories(codegen PUBLIC include)
target_sources(codegen PRIVATE
    Scaffolder.cpp
)
target_link_libraries(codegen PRIVATE parser core)
