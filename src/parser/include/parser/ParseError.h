#pragma once

/*
============================================================

Module:
    Parser

File:
    ParseError.h

Purpose:
    Common parser error representation. Each error carries a stable
    string id, a JSON path where it occurred, and a message.

============================================================
*/

#include <string>

namespace parser {

struct ParseError
{
    std::string id;      // stable error identifier, e.g. "FILE_NOT_FOUND"
    std::string path;    // JSON path, e.g. "$.app.name"
    std::string message; // human-readable description
};

} // namespace parser
