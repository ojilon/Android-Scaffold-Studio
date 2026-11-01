#pragma once

/*
============================================================

Module:
    validator

File:
    Validator.h

Purpose:
    Android rules that a loaded app model must satisfy before
    anything is generated.

Input:
    parser::AppModel (already loaded). Validating the model
    instead of re-parsing the JSON means `generate` never
    validates one file and generates from another, and the
    loader stays the only place that knows nlohmann/json.

Output:
    core::Result<void, std::vector<RuleViolation>>:
    success = no errors, failure = every violation found, in
    document order.

Rule ids are stable. Every crash found on a real phone
becomes a new rule (docs/CRASH_LOG.md).

============================================================
*/

#include <core/Result.h>
#include <parser/AppModel.h>

#include <string>
#include <vector>

namespace validator {

struct RuleViolation {
    std::string id;       // "V001" ... "V009"
    std::string path;     // JSON path: "$.screens[0].layout.children[1].id"
    std::string message;  // human readable detail
    std::string severity; // "Error" (warnings are planned, none emitted yet)
};

// Empty result => the model is valid.
core::Result<void, std::vector<RuleViolation>> validate(const parser::AppModel& model);

} // namespace validator
