#pragma once

/*
============================================================

Module:
    Parser

File:
    AppModel.h

Purpose:
    App model v1 schema structures (JSON -> C++).

Dependencies:
    <string>
    <vector>
    <unordered_map>

============================================================
*/

#include <string>
#include <vector>
#include <unordered_map>

namespace parser {

struct Widget {
    std::string type;
    std::string id;
    std::string width;
    std::string height;
    std::string margin;
    std::string padding;
    std::string text;
    std::string onClick;
    std::vector<Widget> children;
};

struct Layout {
    std::string type; // column, row
    std::vector<Widget> children;
};

struct Screen {
    std::string id;
    std::vector<std::string> states; // loading, content, error
    Layout layout;
};

struct App {
    std::string name;
    std::string package;
    int minSdk;
};

struct AppModel {
    int schemaVersion;
    App app;
    std::vector<Screen> screens;
};

} // namespace parser