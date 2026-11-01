#pragma once

/*
============================================================

Module:
    parser

File:
    AppModel.h

Purpose:
    App model v1: plain value types that mirror model.json.

Design:
    Structs only. There is no Widget class with behaviour:
    the model is data, and everything that walks it is a free
    function (see TreeUtils.h). That keeps the model cheap to
    copy, trivial to compare in tests, and free of the
    virtual dispatch / ownership questions that classes bring.

    Unknown JSON keys are ignored by the loader on purpose so
    a newer model file still loads in an older tool.

Schema notes (full version in docs/02-model-schema.md):
    - Widget.type is one of: column, row, text, button, input,
      image, spacer.
    - width/height accept "match_parent", "wrap_content" or a
      dp/size literal such as "16dp".
    - A screen with `states` gets generated state views; see
      docs/02-model-schema.md for the naming convention.

============================================================
*/

#include <string>
#include <vector>

namespace parser {

struct Widget {
    std::string type;
    std::string id;
    std::string width;   // empty => generator default
    std::string height;  // empty => generator default
    std::string margin;
    std::string padding;
    std::string text;
    std::string onClick; // handler name without the "on" prefix
    std::vector<Widget> children;
};

struct Layout {
    std::string type; // "column" or "row"
    std::vector<Widget> children;
};

struct Screen {
    std::string id;
    // Subset of loading/content/error. `states_declared` separates
    // "no states key" (no state views generated) from `"states": []`
    // (V006: declared but empty), which an empty vector cannot express.
    std::vector<std::string> states;
    bool states_declared = false;
    Layout layout;
    // Reserved: "jni_json_bridge". Parsed, never generated in v1.
    std::string backend;
};

struct App {
    std::string name;
    std::string package;
    int minSdk = 21;
};

struct AppModel {
    int schemaVersion = 1;
    App app;
    std::vector<Screen> screens;
};

} // namespace parser
