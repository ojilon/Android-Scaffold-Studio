#pragma once

/*
============================================================

Module:
    parser

File:
    AppModelLoader.h

Purpose:
    model.json -> parser::AppModel.

Contract:
    Structural problems (missing file, bad JSON, wrong value
    type, missing required field) are reported here as
    ParseError values with a JSON path. Semantic Android
    problems (bad package name, duplicate ids, unknown widget
    type) belong to the validator, not the loader; the loader
    stays permissive so that `validate` can report them with
    rule ids instead of a parse failure.

============================================================
*/

#include <core/FileSystem.h>
#include <core/Result.h>
#include <parser/AppModel.h>
#include <parser/ParseError.h>

#include <vector>

namespace parser {

core::Result<AppModel, std::vector<ParseError>> load_model(const core::fs::Path& path);

// Same loader for a model that is already in memory (tests, later the UI).
core::Result<AppModel, std::vector<ParseError>> parse_model(const std::string& json_text);

} // namespace parser
