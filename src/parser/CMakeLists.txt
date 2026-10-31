add_library(parser STATIC)
target_include_directories(parser PUBLIC include)
target_sources(parser PRIVATE
    context/ParseContext.cpp
    error/ParseError.cpp
    gradle/Gradle.cpp
    java/Java.cpp
    tree/TreeUtils.cpp
    xml/Xml.cpp
    xml/XmlAttribute.cpp
    xml/XmlInternal.cpp
    xml/XmlReader.cpp
    xml/XmlWriter.cpp
    AppModelLoader.cpp
)
target_link_libraries(parser PRIVATE core)
