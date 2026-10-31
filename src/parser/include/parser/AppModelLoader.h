#pragma once

/*
============================================================

Module:
    Parser

File:
    AppModelLoader.h

Purpose:
    Declares loadModel(): loads a JSON model file into an
    AppModel, returning core::Result on success or a vector
    of ParseError on failure.

Dependencies:
    <string>
    <parser/AppModel.h>
    <parser/ParseError.h>
    <core/Result.h>

============================================================
*/

#include <string>

#include <core/Result.h>
#include <parser/AppModel.h>
#include <parser/ParseError.h>

namespace parser {

core::Result<AppModel, std::vector<ParseError>> loadModel(const std::string& filepath);

} // namespace parser
