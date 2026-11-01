#pragma once

/*
============================================================

Module:
    parser

File:
    ParseContext.h

Purpose:
    Shared state while reading a source file (error list plus
    a few preservation flags for the future importer).

Design:
    Data only. add_error()/has_errors() are free functions in
    the parser namespace, so the struct stays a plain value
    that can be copied, returned and inspected in tests.

============================================================
*/

#include <parser/ParseError.h>

#include <core/FileSystem.h>

#include <vector>

namespace parser {

struct ParseContext {
    core::fs::Path file;
    std::vector<ParseError> errors;

    bool stop_on_first_error = false;
    bool preserve_unknown_nodes = true;
    bool preserve_comments = false;
    bool preserve_formatting = false;
};

void add_error(ParseContext& context, ParseError error);
bool has_errors(const ParseContext& context);

} // namespace parser
