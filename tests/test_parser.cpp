#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <core/Result.h>
#include <parser/AppModelLoader.h>
#include <parser/ParseError.h>
#include <parser/TreeUtils.h>

#include <algorithm>
#include <string>
#include <vector>

namespace {

bool has_error(const std::vector<parser::ParseError>& errors, const std::string& id) {
    return std::any_of(errors.begin(), errors.end(),
                       [&](const parser::ParseError& error) { return error.id == id; });
}

bool has_error_at(const std::vector<parser::ParseError>& errors,
                  const std::string& id,
                  const std::string& path) {
    return std::any_of(errors.begin(), errors.end(), [&](const parser::ParseError& error) {
        return error.id == id && error.path == path;
    });
}

} // namespace

TEST_CASE("loads samples/hello.json") {
    auto loaded = parser::load_model("samples/hello.json");
    REQUIRE(loaded.has_value());

    const parser::AppModel& model = *loaded;
    CHECK_EQ(model.schemaVersion, 1);
    CHECK_EQ(model.app.name, "Hello");
    CHECK_EQ(model.app.package, "com.oji.hello");
    CHECK_EQ(model.app.minSdk, 21);

    REQUIRE_EQ(model.screens.size(), 1u);
    const parser::Screen& screen = model.screens.front();
    CHECK_EQ(screen.id, "home");
    CHECK(screen.states_declared);
    REQUIRE_EQ(screen.states.size(), 3u);
    CHECK_EQ(screen.states.at(0), "loading");
    CHECK_EQ(screen.states.at(1), "content");
    CHECK_EQ(screen.states.at(2), "error");
    CHECK_EQ(screen.layout.type, "column");

    REQUIRE_EQ(screen.layout.children.size(), 2u);
    CHECK_EQ(screen.layout.children.at(0).type, "text");
    CHECK_EQ(screen.layout.children.at(0).id, "title");
    CHECK_EQ(screen.layout.children.at(0).text, "Hello");
    CHECK_EQ(screen.layout.children.at(1).type, "button");
    CHECK_EQ(screen.layout.children.at(1).id, "retry");
    CHECK_EQ(screen.layout.children.at(1).onClick, "retry");
}

TEST_CASE("reports a missing file") {
    auto loaded = parser::load_model("samples/does_not_exist.json");
    REQUIRE_FALSE(loaded.has_value());
    CHECK(has_error(loaded.error(), "FILE_NOT_READABLE"));
}

TEST_CASE("reports malformed JSON with a path") {
    auto loaded = parser::parse_model("{ \"schemaVersion\": 1, ");
    REQUIRE_FALSE(loaded.has_value());
    CHECK(has_error_at(loaded.error(), "JSON_PARSE_ERROR", "$"));
}

TEST_CASE("reports a missing schemaVersion") {
    auto loaded = parser::parse_model(R"({ "app": { "name": "A", "package": "a.b" }, "screens": [] })");
    REQUIRE_FALSE(loaded.has_value());
    CHECK(has_error_at(loaded.error(), "MISSING_FIELD", "$.schemaVersion"));
}

TEST_CASE("reports wrong value types instead of guessing") {
    auto loaded = parser::parse_model(R"({
        "schemaVersion": 1,
        "app": { "name": "A", "package": "a.b" },
        "screens": [{ "id": 7, "layout": { "type": "column", "children": [] } }]
    })");
    REQUIRE_FALSE(loaded.has_value());
    CHECK(has_error_at(loaded.error(), "INVALID_TYPE", "$.screens[0].id"));
}

TEST_CASE("parses nested children, widths and margins") {
    auto loaded = parser::parse_model(R"({
        "schemaVersion": 1,
        "app": { "name": "Nested", "package": "com.oji.nested", "minSdk": 24 },
        "screens": [{
            "id": "detail",
            "layout": {
                "type": "row",
                "children": [{
                    "type": "column",
                    "id": "sidebar",
                    "width": "120dp",
                    "height": "match_parent",
                    "margin": "8dp",
                    "padding": "4dp",
                    "children": [
                        { "type": "text", "id": "label", "text": "A & B" }
                    ]
                }]
            }
        }]
    })");
    REQUIRE(loaded.has_value());

    const parser::Screen& screen = loaded->screens.at(0);
    CHECK_EQ(screen.layout.type, "row");

    const parser::Widget& sidebar = screen.layout.children.at(0);
    CHECK_EQ(sidebar.type, "column");
    CHECK_EQ(sidebar.id, "sidebar");
    CHECK_EQ(sidebar.width, "120dp");
    CHECK_EQ(sidebar.height, "match_parent");
    CHECK_EQ(sidebar.margin, "8dp");
    CHECK_EQ(sidebar.padding, "4dp");
    REQUIRE_EQ(sidebar.children.size(), 1u);
    CHECK_EQ(sidebar.children.at(0).text, "A & B");

    CHECK_EQ(parser::count_widgets(sidebar), 2u);
    CHECK_EQ(parser::find_by_id(sidebar, "label")->text, "A & B");
    CHECK(parser::find_by_id(sidebar, "nope") == nullptr);

    const std::vector<std::string> ids = parser::all_ids(sidebar);
    REQUIRE_EQ(ids.size(), 2u);
    CHECK_EQ(ids.at(0), "sidebar");
    CHECK_EQ(ids.at(1), "label");
}

TEST_CASE("ignores unknown keys so newer models still load") {
    auto loaded = parser::parse_model(R"({
        "schemaVersion": 1,
        "futureField": { "anything": true },
        "app": { "name": "A", "package": "a.b", "minSdk": 21 },
        "screens": []
    })");
    CHECK(loaded.has_value());
}
