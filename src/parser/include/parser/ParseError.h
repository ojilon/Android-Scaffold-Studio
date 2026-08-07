#pragma once

/*
============================================================

Module:
    Parser

File:
    ParseError.h

Purpose:
    Common parser error representation.

============================================================
*/

#include <string>

namespace parser
{

enum class ParseErrorCode
{
    None,

    FileNotFound,

    CannotOpen,

    InvalidFormat,

    InvalidSyntax,

    MissingRoot,

    InvalidAttribute,

    DuplicateId,

    UnsupportedVersion,

    Unknown
};

struct ParseError
{
    ParseErrorCode code =
        ParseErrorCode::None;

    std::string message;

    std::string file;

    std::size_t line = 0;

    std::size_t column = 0;
};

}