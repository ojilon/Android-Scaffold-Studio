add_library(validator STATIC)
target_include_directories(validator PUBLIC include)
target_sources(validator PRIVATE
    Validator.cpp
)
target_link_libraries(validator PRIVATE core)