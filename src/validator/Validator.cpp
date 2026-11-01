#include <validator/Validator.h>

#include <nlohmann/json.hpp>
#include <regex>
#include <unordered_set>
#include <string>

using json = nlohmann::json;

namespace validator {

namespace {

std::string format_path(const std::string& base, const std::string& sub) {
    if (base == "$") return "$" + sub;
    return base + sub;
}

} // anonymous

core::Result<void, std::vector<RuleViolation>>
validate(const std::string& json_content) {

    std::vector<RuleViolation> violations;

    json j;
    try {
        j = json::parse(json_content);
    } catch (const json::parse_error& e) {
        violations.push_back({"PARSE_ERROR", "$", e.what(), "Error"});
        return std::unexpected(std::move(violations));
    }

    // ----- app -----
    if (!j.contains("app") || !j["app"].is_object()) {
        violations.push_back({"V004", "$.app", "app object missing", "Error"});
    } else {
        const auto& app = j["app"];
        std::string pkg  = app.value("package", "");
        std::string name = app.value("name", "");
        int minSdk       = app.value("minSdk", 0);

        static const std::regex pkg_re(R"(^[a-zA-Z_][a-zA-Z0-9_]*(\.[a-zA-Z_][a-zA-Z0-9_]*)*$)");
        if (pkg.empty() || !std::regex_match(pkg, pkg_re)) {
            violations.push_back({"V004", "$.app.package",
                                  "invalid Java package name: " + pkg, "Error"});
        }
        if (name.empty()) {
            violations.push_back({"V004", "$.app.name", "app name is empty", "Error"});
        }
        if (minSdk < 21 || minSdk > 35) {
            violations.push_back({"V007", "$.app.minSdk",
                                  "minSdk " + std::to_string(minSdk) + " out of range (21-35)", "Error"});
        }
    }

    // ----- screens -----
    if (!j.contains("screens") || !j["screens"].is_array()) {
        // optional for now
    } else {
        std::unordered_set<std::string> screen_ids;

        for (size_t i = 0; i < j["screens"].size(); ++i) {
            const auto& screen_j = j["screens"][i];
            std::string path_screen = format_path("$", ".screens[" + std::to_string(i) + "]");

            std::string sid = screen_j.value("id", "");
            static const std::regex id_re(R"(^[a-z][a-z0-9_]*$)");
            if (sid.empty() || !std::regex_match(sid, id_re)) {
                violations.push_back({"V005", path_screen + ".id",
                                      "invalid screen id: " + sid, "Error"});
            }
            if (screen_ids.count(sid)) {
                violations.push_back({"V005", path_screen + ".id",
                                      "duplicate screen id: " + sid, "Error"});
            }
            screen_ids.insert(sid);

            // states
            if (screen_j.contains("states") && screen_j["states"].is_array()) {
                if (screen_j["states"].empty()) {
                    violations.push_back({"V006", path_screen + ".states",
                                          "states array is empty", "Error"});
                }
                for (size_t s = 0; s < screen_j["states"].size(); ++s) {
                    std::string st = screen_j["states"][s].get<std::string>();
                    if (st != "loading" && st != "content" && st != "error") {
                        violations.push_back({"V006",
                            path_screen + ".states[" + std::to_string(s) + "]",
                            "unknown state: " + st, "Error"});
                    }
                }
            }

            // layout
            if (screen_j.contains("layout") && screen_j["layout"].is_object()) {
                const auto& layout_j = screen_j["layout"];
                std::string ltype = layout_j.value("type", "");
                if (ltype != "column" && ltype != "row") {
                    violations.push_back({"V003", path_screen + ".layout.type",
                                          "invalid layout type: " + ltype, "Error"});
                }

                std::unordered_set<std::string> widget_ids;
                std::unordered_set<std::string> onclicks;

                if (layout_j.contains("children") && layout_j["children"].is_array()) {
                    for (size_t c = 0; c < layout_j["children"].size(); ++c) {
                        const auto& child_j = layout_j["children"][c];
                        std::string path_child = path_screen + ".layout.children[" + std::to_string(c) + "]";

                        std::string type    = child_j.value("type", "");
                        std::string id      = child_j.value("id", "");
                        std::string onclick = child_j.value("onClick", "");

                        static const std::unordered_set<std::string> allowed =
                            {"column", "row", "text", "button", "input", "image", "spacer"};
                        if (allowed.find(type) == allowed.end()) {
                            violations.push_back({"V003", path_child + ".type",
                                                  "unknown widget type: " + type, "Error"});
                        }

                        static const std::regex res_id_re(R"(^[a-z][a-z0-9_]*$)");
                        if (!id.empty()) {
                            if (!std::regex_match(id, res_id_re)) {
                                violations.push_back({"V008", path_child + ".id",
                                                      "invalid Android resource id: " + id, "Error"});
                            }
                            if (widget_ids.count(id)) {
                                violations.push_back({"V001", path_child + ".id",
                                                      "duplicate id: " + id, "Error"});
                            }
                            widget_ids.insert(id);
                        }

                        if (!onclick.empty()) {
                            static const std::regex java_id(R"(^[a-zA-Z_$][a-zA-Z0-9_$]*$)");
                            if (!std::regex_match(onclick, java_id)) {
                                violations.push_back({"V002", path_child + ".onClick",
                                                      "invalid Java identifier: " + onclick, "Error"});
                            }
                            if (onclicks.count(onclick)) {
                                violations.push_back({"V002", path_child + ".onClick",
                                                      "duplicate onClick: " + onclick, "Error"});
                            }
                            onclicks.insert(onclick);
                        }
                    }
                }
            }
        }
    }

    if (violations.empty())
        return {};
    return std::unexpected(std::move(violations));
}

} // namespace validator