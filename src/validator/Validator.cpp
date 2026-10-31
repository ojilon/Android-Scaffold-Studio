#include "validator/Validator.h"
#include <core/Result.h>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_set>
#include <regex>

using json = nlohmann::json;
using namespace parser;

namespace validator {

// Helper to format JSON path
static std::string formatPath(const std::string& base, const std::string& sub) {
    if (base == "$") return "$" + sub;
    return base + sub;
}

core::Result<void, std::vector<RuleViolation>> Validator::validate(const std::string& json_content) {
    std::vector<RuleViolation> violations;
    
    json j;
    try {
        j = json::parse(json_content);
    } catch (const json::parse_error& e) {
        violations.push_back({"PARSE_ERROR", "$", e.what(), "Error"});
        return core::error(violations);
    }

    // V004: package name is a valid Java package; app name non-empty
    if (j.contains("app")) {
        const auto& app = j["app"];
        std::string pkg = app.value("package", "");
        std::string name = app.value("name", "");
        int minSdk = app.value("minSdk", 0);

        // V004 Package & Name
        std::regex pkg_regex("^[a-zA-Z_][a-zA-Z0-9_]*(\\.[a-zA-Z_][a-zA-Z0-9_]*)*$");
        if (!std::regex_match(pkg, pkg_regex) || pkg.empty()) {
            violations.push_back({"V004", "$.app.package", "invalid Java package name: " + pkg, "Error"});
        }
        if (name.empty()) {
            violations.push_back({"V004", "$.app.name", "app name is empty", "Error"});
        }

        // V007: minSdk range 21-35
        if (minSdk < 21 || minSdk > 35) {
            violations.push_back({"V007", "$.app.minSdk", "minSdk " + std::to_string(minSdk) + " is out of range (21-35)", "Error"});
        }
    } else {
        violations.push_back({"V004", "$.app", "app object missing", "Error"});
    }

    // Screen level checks
    if (j.contains("screens") && j["screens"].is_array()) {
        std::unordered_set<std::string> screen_ids;
        
        for (size_t i = 0; i < j["screens"].size(); ++i) {
            const auto& screen_j = j["screens"][i];
            std::string path_screen = formatPath("$", ".screens[" + std::to_string(i) + "]");
            
            std::string sid = screen_j.value("id", "");
            // V005: screen ids unique and valid
            std::regex id_regex("^[a-z][a-z0-9_]*$");
            if (!std::regex_match(sid, id_regex) || sid.empty()) {
                violations.push_back({"V005", path_screen + ".id", "invalid screen id: " + sid, "Error"});
            }
            if (screen_ids.count(sid)) {
                violations.push_back({"V005", path_screen + ".id", "duplicate screen id: " + sid, "Error"});
            }
            screen_ids.insert(sid);

            // V006: states
            if (screen_j.contains("states") && screen_j["states"].is_array()) {
                const auto& states = screen_j["states"];
                if (states.empty()) {
                    violations.push_back({"V006", path_screen + ".states", "states array is empty", "Error"});
                }
                for (size_t s = 0; s < states.size(); ++s) {
                    std::string st = states[s].get<std::string>();
                    if (st != "loading" && st != "content" && st != "error") {
                        violations.push_back({"V006", path_screen + ".states[" + std::to_string(s) + "]", "unknown state: " + st, "Error"});
                    }
                }
            }

            // Layout & Widgets
            if (screen_j.contains("layout")) {
                const auto& layout_j = screen_j["layout"];
                std::unordered_set<std::string> widget_ids;
                std::unordered_set<std::string> onclicks;

                // V003: widget type check for root layout
                std::string ltype = layout_j.value("type", "");
                if (ltype != "column" && ltype != "row") {
                    violations.push_back({"V003", path_screen + ".layout.type", "invalid layout type: " + ltype, "Error"});
                }

                if (layout_j.contains("children") && layout_j["children"].is_array()) {
                    for (size_t c = 0; c < layout_j["children"].size(); ++c) {
                        const auto& child_j = layout_j["children"][c];
                        std::string path_child = path_screen + ".layout.children[" + std::to_string(c) + "]";

                        std::string type = child_j.value("type", "");
                        std::string id = child_j.value("id", "");
                        std::string onclick = child_j.value("onClick", "");

                        // V003: widget type
                        std::unordered_set<std::string> allowed = {"column", "row", "text", "button", "input", "image", "spacer"};
                        if (allowed.find(type) == allowed.end()) {
                            violations.push_back({"V003", path_child + ".type", "unknown widget type: " + type, "Error"});
                        }

                        // V001 & V008: ids unique and valid Android resource names
                        std::regex res_id_regex("^[a-z][a-z0-9_]*$");
                        if (!std::regex_match(id, res_id_regex) || id.empty()) {
                            violations.push_back({"V008", path_child + ".id", "invalid Android resource id: " + id, "Error"});
                        }
                        if (!id.empty()) {
                            if (widget_ids.count(id)) {
                                violations.push_back({"V001", path_child + ".id", "duplicate id: " + id, "Error"});
                            }
                            widget_ids.insert(id);
                        }

                        // V002: onClick valid identifier and unique
                        if (!onclick.empty()) {
                            std::regex java_id("^[a-zA-Z_$][a-zA-Z0-9_$]*$");
                            if (!std::regex_match(onclick, java_id)) {
                                violations.push_back({"V002", path_child + ".onClick", "invalid Java identifier: " + onclick, "Error"});
                            }
                            if (onclicks.count(onclick)) {
                                violations.push_back({"V002", path_child + ".onClick", "duplicate onClick: " + onclick, "Error"});
                            }
                            onclicks.insert(onclick);
                        }
                    }
                }
            }
        }
    }

    if (violations.empty()) {
        return core::ok();
    } else {
        return core::error(violations);
    }
}

} // namespace validator