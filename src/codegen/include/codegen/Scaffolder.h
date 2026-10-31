#pragma once

/*
============================================================

Module:
    Codegen

File:
    Scaffolder.h

Purpose:
    Generate Android artifacts (XML, Java, Manifest) from AppModel.

Dependencies:
    <string>
    <parser/AppModel.h>

============================================================
*/

#include <string>
#include <parser/AppModel.h>
#include <core/Result.h>

namespace codegen {

struct GenerationResult {
    std::string layout_xml;
    std::string screen_base_java;
    std::string activity_java;
    std::string manifest_xml;
};

core::Result<GenerationResult, std::string> generate(const parser::AppModel& model);

} // namespace codegen