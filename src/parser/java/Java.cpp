/*
============================================================

Module:
    Parser

File:
    Java.cpp

Purpose:
    Java source parser.

Responsibilities

    Java Source

        ↓

    Tokenizer

        ↓

    AST

        ↓

    Internal Representation

============================================================
*/

#include <parser/Java.h>

namespace parser
{

result::VoidResult
parse_java(
    const filesystem::Path& file)
{
    /*
    --------------------------------------------------------

    Future pipeline

    Read File

        ↓

    Tokenize

        ↓

    Parse Classes

        ↓

    Parse Methods

        ↓

    Parse Fields

        ↓

    Parse Imports

        ↓

    Build AST

    --------------------------------------------------------
    */

    (void)file;

    return std::unexpected(
        "Java parser not implemented.");
}

}

/*
============================================================

TODO

[ ] Java tokenizer
[ ] AST
[ ] Imports
[ ] Packages
[ ] Classes
[ ] Interfaces
[ ] Enums
[ ] Fields
[ ] Constructors
[ ] Methods
[ ] Lambdas
[ ] Generics
[ ] Annotations

============================================================
*/