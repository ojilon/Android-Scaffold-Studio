add_library(validator STATIC)

target_sources(validator PRIVATE
    Validator.cpp
)

target_include_directories(validator PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(validator PUBLIC parser core)

target_apply_warnings(validator)
