#pragma once

/*
============================================================

Module:
    parser

File:
    ParseError.h

Purpose:
    One problem found while reading a file. The same struct is
    used for model loading errors and (later) XML/Java import
    diagnostics, so the CLI has exactly one error format:

        [CODE] $.screens[0].id: message

============================================================
*/

#include <string>

namespace parser {

struct ParseError {
    std::string id;      // stable, greppable code, e.g. "JSON_PARSE_ERROR"
    std::string path;    // JSON path, e.g. "$.screens[0].layout.type"
    std::string message; // human readable detail
};

} // namespace parser
