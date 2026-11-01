#pragma once

/*
============================================================

Module:
    codegen

File:
    Template.h

Purpose:
    The project's own template renderer: {{dotted.path}}
    substitution from a JSON value.

Why ours:
    inja is nice but heavy for a Celeron with 4 GB of RAM and
    we only need variable substitution. Loops and fragments
    are built in C++ and passed in as strings (the plan's
    explicit choice), so the renderer stays ~80 lines.

Behaviour:
    - "{{app.package}}" reads values["app"]["package"].
    - Whitespace inside the braces is ignored: "{{ app.name }}".
    - Text inserted by a substitution is never re-scanned, so a
      value containing "{{" cannot loop.
    - An unknown or non-string value is an error, reported with
      the template name and the offending placeholder. Silent
      leftovers ("{{screen.class}}") would end up in generated
      Java and are exactly the kind of bug this tool exists to
      prevent.

============================================================
*/

#include <core/Result.h>

#include <nlohmann/json_fwd.hpp>

#include <string>
#include <string_view>

namespace codegen {

// `template_name` is only used in error messages (e.g. "ScreenBase.java.in").
core::Result<std::string> render(std::string_view template_text,
                                 const nlohmann::json& values,
                                 std::string_view template_name);

} // namespace codegen
