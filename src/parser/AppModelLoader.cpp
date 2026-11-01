#include <parser/AppModelLoader.h>

#include <nlohmann/json.hpp>

#include <string>
#include <utility>

namespace parser {

namespace {

using json = nlohmann::json;

void report(std::vector<ParseError>& errors, std::string id, std::string path, std::string message) {
    errors.push_back(ParseError{std::move(id), std::move(path), std::move(message)});
}

// Reads an optional string field. Missing key keeps `out` untouched; a
// present-but-wrong-type value is an error so typos surface instead of
// silently falling back to defaults.
bool read_optional_string(const json& object,
                          const char* key,
                          const std::string& path,
                          std::string& out,
                          std::vector<ParseError>& errors) {
    if (!object.contains(key))
        return true;

    const json& value = object.at(key);
    if (!value.is_string()) {
        report(errors, "INVALID_TYPE", path + "." + key,
               std::string(key) + " must be a string, got " + value.type_name());
        return false;
    }

    out = value.get<std::string>();
    return true;
}

bool read_required_string(const json& object,
                          const char* key,
                          const std::string& path,
                          std::string& out,
                          std::vector<ParseError>& errors) {
    if (!object.contains(key)) {
        report(errors, "MISSING_FIELD", path + "." + key,
               std::string(key) + " is required");
        return false;
    }
    return read_optional_string(object, key, path, out, errors);
}

bool parse_widget(const json& node,
                  const std::string& path,
                  Widget& out,
                  std::vector<ParseError>& errors);

bool parse_children(const json& node,
                    const std::string& path,
                    std::vector<Widget>& out,
                    std::vector<ParseError>& errors) {
    if (!node.contains("children"))
        return true;

    const json& children = node.at("children");
    if (!children.is_array()) {
        report(errors, "INVALID_TYPE", path + ".children",
               std::string("children must be an array, got ") + children.type_name());
        return false;
    }

    bool ok = true;
    for (std::size_t i = 0; i < children.size(); ++i) {
        Widget widget;
        if (!parse_widget(children.at(i), path + ".children[" + std::to_string(i) + "]", widget, errors))
            ok = false;
        else
            out.push_back(std::move(widget));
    }
    return ok;
}

bool parse_widget(const json& node,
                  const std::string& path,
                  Widget& out,
                  std::vector<ParseError>& errors) {
    if (!node.is_object()) {
        report(errors, "INVALID_TYPE", path, std::string("widget must be an object, got ") + node.type_name());
        return false;
    }

    bool ok = read_required_string(node, "type", path, out.type, errors);
    ok = read_optional_string(node, "id", path, out.id, errors) && ok;
    ok = read_optional_string(node, "width", path, out.width, errors) && ok;
    ok = read_optional_string(node, "height", path, out.height, errors) && ok;
    ok = read_optional_string(node, "margin", path, out.margin, errors) && ok;
    ok = read_optional_string(node, "padding", path, out.padding, errors) && ok;
    ok = read_optional_string(node, "text", path, out.text, errors) && ok;
    ok = read_optional_string(node, "onClick", path, out.onClick, errors) && ok;
    ok = parse_children(node, path, out.children, errors) && ok;

    return ok;
}

bool parse_layout(const json& node,
                  const std::string& path,
                  Layout& out,
                  std::vector<ParseError>& errors) {
    if (!node.is_object()) {
        report(errors, "INVALID_TYPE", path, std::string("layout must be an object, got ") + node.type_name());
        return false;
    }

    bool ok = read_required_string(node, "type", path, out.type, errors);
    ok = parse_children(node, path, out.children, errors) && ok;
    return ok;
}

bool parse_screen(const json& node,
                  const std::string& path,
                  Screen& out,
                  std::vector<ParseError>& errors) {
    if (!node.is_object()) {
        report(errors, "INVALID_TYPE", path, std::string("screen must be an object, got ") + node.type_name());
        return false;
    }

    bool ok = read_required_string(node, "id", path, out.id, errors);
    ok = read_optional_string(node, "backend", path, out.backend, errors) && ok;

    if (node.contains("states")) {
        out.states_declared = true;
        const json& states = node.at("states");
        if (!states.is_array()) {
            report(errors, "INVALID_TYPE", path + ".states",
                   std::string("states must be an array, got ") + states.type_name());
            ok = false;
        } else {
            for (std::size_t i = 0; i < states.size(); ++i) {
                const json& state = states.at(i);
                if (!state.is_string()) {
                    report(errors, "INVALID_TYPE",
                           path + ".states[" + std::to_string(i) + "]",
                           "state must be a string, got " + std::string(state.type_name()));
                    ok = false;
                    continue;
                }
                out.states.push_back(state.get<std::string>());
            }
        }
    }

    if (node.contains("layout")) {
        if (!parse_layout(node.at("layout"), path + ".layout", out.layout, errors))
            ok = false;
    }

    return ok;
}

} // namespace

core::Result<AppModel, std::vector<ParseError>> parse_model(const std::string& json_text) {
    std::vector<ParseError> errors;

    json document;
    try {
        document = json::parse(json_text);
    } catch (const json::parse_error& error) {
        report(errors, "JSON_PARSE_ERROR", "$", error.what());
        return std::unexpected(std::move(errors));
    }

    if (!document.is_object()) {
        report(errors, "INVALID_TYPE", "$",
               std::string("model must be a JSON object, got ") + document.type_name());
        return std::unexpected(std::move(errors));
    }

    AppModel model;

    if (!document.contains("schemaVersion")) {
        report(errors, "MISSING_FIELD", "$.schemaVersion", "schemaVersion is required");
    } else if (!document.at("schemaVersion").is_number_integer()) {
        report(errors, "INVALID_TYPE", "$.schemaVersion",
               "schemaVersion must be an integer, got " + std::string(document.at("schemaVersion").type_name()));
    } else {
        model.schemaVersion = document.at("schemaVersion").get<int>();
    }

    if (!document.contains("app")) {
        report(errors, "MISSING_FIELD", "$.app", "app object is required");
    } else if (!document.at("app").is_object()) {
        report(errors, "INVALID_TYPE", "$.app",
               std::string("app must be an object, got ") + document.at("app").type_name());
    } else {
        const json& app = document.at("app");
        read_required_string(app, "name", "$.app", model.app.name, errors);
        read_required_string(app, "package", "$.app", model.app.package, errors);

        if (app.contains("minSdk")) {
            const json& min_sdk = app.at("minSdk");
            if (!min_sdk.is_number_integer()) {
                report(errors, "INVALID_TYPE", "$.app.minSdk",
                       std::string("minSdk must be an integer, got ") + min_sdk.type_name());
            } else {
                model.app.minSdk = min_sdk.get<int>();
            }
        }
    }

    if (!document.contains("screens")) {
        report(errors, "MISSING_FIELD", "$.screens", "screens array is required");
    } else if (!document.at("screens").is_array()) {
        report(errors, "INVALID_TYPE", "$.screens",
               std::string("screens must be an array, got ") + document.at("screens").type_name());
    } else {
        const json& screens = document.at("screens");
        for (std::size_t i = 0; i < screens.size(); ++i) {
            Screen screen;
            const std::string path = "$.screens[" + std::to_string(i) + "]";
            if (parse_screen(screens.at(i), path, screen, errors))
                model.screens.push_back(std::move(screen));
        }
    }

    if (!errors.empty())
        return std::unexpected(std::move(errors));

    return model;
}

core::Result<AppModel, std::vector<ParseError>> load_model(const core::fs::Path& path) {
    auto text = core::fs::read_text_file(path);
    if (!text) {
        return std::unexpected(std::vector<ParseError>{
            ParseError{"FILE_NOT_READABLE", "$", text.error()}});
    }
    return parse_model(*text);
}

} // namespace parser
