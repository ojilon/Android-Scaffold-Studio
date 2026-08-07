/*
============================================================

Module:
    Parser

File:
    Gradle.cpp

Purpose:
    Gradle parser.

Responsibilities

    build.gradle

        ↓

    Tokenizer

        ↓

    AST

        ↓

    Internal Project Model

============================================================
*/

#include <parser/Gradle.h>

namespace parser
{

result::VoidResult
parse_gradle(
    const filesystem::Path& file)
{
    /*
    --------------------------------------------------------

    Future pipeline

    Read File

        ↓

    Tokenize

        ↓

    Parse Blocks

        ↓

    Parse Plugins

        ↓

    Parse Dependencies

        ↓

    Parse Android Block

        ↓

    Internal Model

    --------------------------------------------------------
    */

    (void)file;

    return std::unexpected(
        "Gradle parser not implemented.");
}

}

/*
============================================================

TODO

[ ] plugins{}
[ ] android{}
[ ] dependencies{}
[ ] repositories{}
[ ] signingConfigs{}
[ ] buildTypes{}
[ ] flavors{}
[ ] kotlin support
[ ] version catalog
[ ] settings.gradle

============================================================
*/