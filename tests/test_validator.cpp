#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <core/Result.h>
#include <parser/AppModelLoader.h>
#include <validator/Validator.h>

#include <algorithm>
#include <string>
#include <vector>

namespace {

parser::AppModel load(const std::string& path) {
    auto loaded = parser::load_model(path);
    REQUIRE(loaded.has_value());
    return *loaded;
}

// Validates the sample and returns its violations (empty when valid).
std::vector<validator::RuleViolation> violations_for(const std::string& path) {
    const parser::AppModel model = load(path);
    auto result = validator::validate(model);
    if (result.has_value())
        return {};
    return result.error();
}

bool has(const std::vector<validator::RuleViolation>& violations,
         const std::string& id,
         const std::string& path) {
    return std::any_of(violations.begin(), violations.end(),
                       [&](const validator::RuleViolation& violation) {
                           return violation.id == id && violation.path == path;
                       });
}

bool has_id(const std::vector<validator::RuleViolation>& violations, const std::string& id) {
    return std::any_of(violations.begin(), violations.end(),
                       [&](const validator::RuleViolation& violation) {
                           return violation.id == id;
                       });
}

} // namespace

TEST_CASE("the golden sample is valid") {
    const std::vector<validator::RuleViolation> violations = violations_for("samples/hello.json");
    for (const validator::RuleViolation& violation : violations)
        INFO(violation.id << " " << violation.path << ": " << violation.message);
    CHECK(violations.empty());
}

TEST_CASE("V001 duplicate ids, including nested widgets") {
    const auto flat = violations_for("samples/invalid/v001_duplicate_id.json");
    CHECK(has(flat, "V001", "$.screens[0].layout.children[1].id"));

    const auto nested = violations_for("samples/invalid/v001_nested_duplicate_id.json");
    CHECK(has(nested, "V001", "$.screens[0].layout.children[1].id"));
}

TEST_CASE("V002 onClick must be a unique Java identifier") {
    const auto invalid = violations_for("samples/invalid/v002_invalid_onclick.json");
    CHECK(has(invalid, "V002", "$.screens[0].layout.children[1].onClick"));

    // "retry" and "Retry" both generate onRetry(), so the second one clashes
    // even though the strings differ.
    const auto duplicate = violations_for("samples/invalid/v002_duplicate_onclick.json");
    CHECK(has(duplicate, "V002", "$.screens[0].layout.children[1].onClick"));
}

TEST_CASE("V003 widget and layout types") {
    const auto unknown = violations_for("samples/invalid/v003_invalid_type.json");
    CHECK(has(unknown, "V003", "$.screens[0].layout.type"));
}

TEST_CASE("V004 package name and app name") {
    const auto invalid_package = violations_for("samples/invalid/v004_invalid_package.json");
    CHECK(has(invalid_package, "V004", "$.app.package"));

    auto no_name = parser::parse_model(R"({
        "schemaVersion": 1,
        "app": { "name": "", "package": "com.oji.hello", "minSdk": 21 },
        "screens": []
    })");
    REQUIRE(no_name.has_value());
    auto result = validator::validate(*no_name);
    REQUIRE_FALSE(result.has_value());
    CHECK(has(result.error(), "V004", "$.app.name"));
}

TEST_CASE("V005 screen ids are unique") {
    const auto duplicate = violations_for("samples/invalid/v005_duplicate_screen_id.json");
    CHECK(has(duplicate, "V005", "$.screens[1].id"));
}

TEST_CASE("V006 states are known and not empty") {
    const auto unknown = violations_for("samples/invalid/v006_unknown_state.json");
    CHECK(has(unknown, "V006", "$.screens[0].states[1]"));

    auto empty_states = parser::parse_model(R"({
        "schemaVersion": 1,
        "app": { "name": "A", "package": "com.oji.hello", "minSdk": 21 },
        "screens": [{ "id": "home", "states": [], "layout": { "type": "column", "children": [] } }]
    })");
    REQUIRE(empty_states.has_value());
    auto result = validator::validate(*empty_states);
    REQUIRE_FALSE(result.has_value());
    CHECK(has(result.error(), "V006", "$.screens[0].states"));
}

TEST_CASE("V001 ids may not shadow generated state views") {
    const auto violations = violations_for("samples/invalid/v001_reserved_state_id.json");
    CHECK(has(violations, "V001", "$.screens[0].layout.children[0].id"));
}

TEST_CASE("V006 states must include content") {
    const auto violations = violations_for("samples/invalid/v006_missing_content_state.json");
    CHECK(has(violations, "V006", "$.screens[0].states"));
}

TEST_CASE("V007 minSdk range") {
    const auto violations = violations_for("samples/invalid/v007_min_sdk_out_of_range.json");
    CHECK(has(violations, "V007", "$.app.minSdk"));
}

TEST_CASE("V008 Android resource names") {
    const auto violations = violations_for("samples/invalid/v008_invalid_resource_id.json");
    CHECK(has(violations, "V008", "$.screens[0].layout.children[0].id"));
}

TEST_CASE("V009 ids that are Java keywords") {
    const auto violations = violations_for("samples/invalid/v009_java_keyword_id.json");
    CHECK(has(violations, "V009", "$.screens[0].layout.children[0].id"));
}

TEST_CASE("violations carry rule id, path, message and severity") {
    const auto violations = violations_for("samples/invalid/v001_duplicate_id.json");
    REQUIRE_FALSE(violations.empty());
    CHECK_EQ(violations.front().severity, "Error");
    CHECK(has_id(violations, "V001"));
    CHECK_FALSE(violations.front().message.empty());
}
