#include "parser/AppModel.h"
#include "parser/ParseError.h"
#include <core/Result.h>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>

using json = nlohmann::json;
using namespace parser;

namespace parser {

core::Result<AppModel, std::vector<ParseError>> loadModel(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return std::unexpected<std::vector<ParseError>>(
            {ParseError{"FILE_NOT_FOUND", "$.", "Could not open file: " + filepath}});
    }

    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        return std::unexpected<std::vector<ParseError>>(
            {ParseError{"JSON_PARSE_ERROR", "$.", e.what()}});
    }

    AppModel model;

    // schemaVersion
    if (j.contains("schemaVersion")) {
        model.schemaVersion = j["schemaVersion"].get<int>();
    } else {
        return std::unexpected<std::vector<ParseError>>(
            {ParseError{"MISSING_FIELD", "$.schemaVersion", "schemaVersion is required"}});
    }

    // app
    if (j.contains("app")) {
        const auto& app_j = j["app"];
        model.app.name = app_j.value("name", "");
        model.app.package = app_j.value("package", "");
        model.app.minSdk = app_j.value("minSdk", 0);

        if (model.app.name.empty()) {
            return std::unexpected<std::vector<ParseError>>(
                {ParseError{"INVALID_VALUE", "$.app.name", "app name cannot be empty"}});
        }
        if (model.app.package.empty()) {
            return std::unexpected<std::vector<ParseError>>(
                {ParseError{"INVALID_VALUE", "$.app.package", "package name cannot be empty"}});
        }
    } else {
        return std::unexpected<std::vector<ParseError>>(
            {ParseError{"MISSING_FIELD", "$.app", "app object is required"}});
    }

    // screens
    if (j.contains("screens") && j["screens"].is_array()) {
        for (size_t i = 0; i < j["screens"].size(); ++i) {
            const auto& screen_j = j["screens"][i];
            Screen screen;

            screen.id = screen_j.value("id", "");
            if (screen.id.empty()) {
                return std::unexpected<std::vector<ParseError>>(
                    {ParseError{"MISSING_FIELD", "$.screens[" + std::to_string(i) + "].id", "screen id is required"}});
            }

            // states
            if (screen_j.contains("states") && screen_j["states"].is_array()) {
                for (const auto& state_j : screen_j["states"]) {
                    screen.states.push_back(state_j.get<std::string>());
                }
            }

            // layout
            if (screen_j.contains("layout")) {
                const auto& layout_j = screen_j["layout"];
                screen.layout.type = layout_j.value("type", "");

                if (screen.layout.type.empty()) {
                    return std::unexpected<std::vector<ParseError>>(
                        {ParseError{"MISSING_FIELD", "$.screens[" + std::to_string(i) + "].layout.type",
                                    "layout type is required"}});
                }

                if (layout_j.contains("children") && layout_j["children"].is_array()) {
                    for (const auto& child_j : layout_j["children"]) {
                        Widget w;
                        w.type = child_j.value("type", "");
                        w.id = child_j.value("id", "");
                        w.width = child_j.value("width", "");
                        w.height = child_j.value("height", "");
                        w.margin = child_j.value("margin", "");
                        w.padding = child_j.value("padding", "");
                        w.text = child_j.value("text", "");
                        w.onClick = child_j.value("onClick", "");
                        screen.layout.children.push_back(w);
                    }
                }
            }
            model.screens.push_back(screen);
        }
    }

    return model;
}

} // namespace parser
