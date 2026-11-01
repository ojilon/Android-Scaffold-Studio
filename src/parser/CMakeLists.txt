add_library(parser STATIC)

target_sources(parser PRIVATE
    AppModelLoader.cpp
    context/ParseContext.cpp
    error/ParseError.cpp
    gradle/Gradle.cpp
    java/Java.cpp
    tree/TreeUtils.cpp
    xml/Xml.cpp
    xml/XmlAttribute.cpp
)

target_include_directories(parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# PUBLIC: parser's headers expose core::Result / core::fs::Path, so anything
# that links parser needs core's include directory too.
target_link_libraries(parser PUBLIC core)

target_apply_warnings(parser)
