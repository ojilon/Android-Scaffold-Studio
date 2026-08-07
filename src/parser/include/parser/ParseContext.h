#pragma once

/*
============================================================

Module:
    Parser

Purpose:
    Shared parsing state.

============================================================
*/

#include <core/Core.h>

#include <parser/ParseError.h>

#include <vector>

namespace parser
{

struct ParseContext
{
    filesystem::Path file;

    std::vector<ParseError> errors;

    bool stop_on_first_error = false;

    bool preserve_unknown_nodes = true;

    bool preserve_comments = false;

    bool preserve_formatting = false;

    void add_error(
        ParseError error);

    bool has_errors() const;
};

}