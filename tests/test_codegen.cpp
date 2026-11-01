#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <codegen/Generation.h>
#include <codegen/Template.h>
#include <core/FileSystem.h>
#include <core/Result.h>
#include <parser/AppModelLoader.h>
#include <scaffolder/ProjectWriter.h>
#include <scaffolder/Templates.h>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

namespace {

using json = nlohmann::json;

// CRLF and LF checkouts must compare equal: the generator always writes LF.
std::string normalize(std::string text) {
    text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
    return text;
}

codegen::TemplateSet load_templates() {
    auto templates = scaffolder::load_templates(scaffolder::default_template_directory());
    REQUIRE(templates.has_value());
    return *templates;
}

parser::AppModel load_model(const std::string& path) {
    auto loaded = parser::load_model(path);
    REQUIRE(loaded.has_value());
    return *loaded;
}

} // namespace

TEST_CASE("template renderer substitutes dotted paths") {
    const json values = {{"app", {{"package", "com.oji.hello"}}}, {"name", "Home"}};

    auto rendered = codegen::render("package {{app.package}}; class {{ name }}", values, "t");
    REQUIRE(rendered.has_value());
    CHECK_EQ(*rendered, "package com.oji.hello; class Home");
}

TEST_CASE("template renderer reports unknown variables") {
    const json values = json::object();
    auto rendered = codegen::render("{{app.missing}}", values, "broken.java.in");
    REQUIRE_FALSE(rendered.has_value());
    CHECK(rendered.error().find("{{app.missing}}") != std::string::npos);
}

TEST_CASE("template renderer never rescans substituted text") {
    const json values = {{"a", "{{b}}"}, {"b", "boom"}};
    auto rendered = codegen::render("<{{a}}>", values, "t");
    REQUIRE(rendered.has_value());
    CHECK_EQ(*rendered, "<{{b}}>");
}

TEST_CASE("generated output matches docs/golden/hello") {
    const parser::AppModel model = load_model("samples/hello.json");

    auto generated = codegen::generate(model, load_templates());
    REQUIRE(generated.has_value());
    REQUIRE_EQ(generated->files.size(), 4u);

    for (const codegen::GeneratedFile& file : generated->files) {
        const std::filesystem::path golden =
            std::filesystem::path("docs/golden/hello") / file.relative_path;

        auto expected = core::fs::read_text_file(golden);
        REQUIRE_MESSAGE(expected.has_value(), "missing golden file: " << golden.string());

        INFO("comparing " << file.relative_path.generic_string());
        CHECK_EQ(normalize(file.content), normalize(*expected));
    }
}

TEST_CASE("the generated activity is user-owned, the rest is not") {
    const parser::AppModel model = load_model("samples/hello.json");

    auto generated = codegen::generate(model, load_templates());
    REQUIRE(generated.has_value());

    for (const codegen::GeneratedFile& file : generated->files) {
        const std::string name = file.relative_path.filename().string();
        const bool is_activity = name == "HomeActivity.java";
        CHECK_EQ(file.user_owned, is_activity);
    }
}

TEST_CASE("generation refuses a model without screens") {
    auto empty = parser::parse_model(R"({
        "schemaVersion": 1,
        "app": { "name": "A", "package": "com.oji.hello", "minSdk": 21 },
        "screens": []
    })");
    REQUIRE(empty.has_value());

    auto generated = codegen::generate(*empty, load_templates());
    CHECK_FALSE(generated.has_value());
}

TEST_CASE("a screen without states gets a plain layout and no setState") {
    auto model = parser::parse_model(R"({
        "schemaVersion": 1,
        "app": { "name": "Plain", "package": "com.oji.plain", "minSdk": 21 },
        "screens": [{
            "id": "main",
            "layout": {
                "type": "row",
                "children": [
                    { "type": "input", "id": "email", "text": "Email", "width": "match_parent" }
                ]
            }
        }]
    })");
    REQUIRE(model.has_value());

    auto generated = codegen::generate(*model, load_templates());
    REQUIRE(generated.has_value());

    const codegen::GeneratedFile* layout = nullptr;
    const codegen::GeneratedFile* base = nullptr;
    for (const codegen::GeneratedFile& file : generated->files) {
        const std::string name = file.relative_path.filename().string();
        if (name == "main.xml") layout = &file;
        if (name == "MainScreenBase.java") base = &file;
    }

    REQUIRE(layout != nullptr);
    CHECK(layout->content.find("<LinearLayout") != std::string::npos);
    CHECK(layout->content.find("android:orientation=\"horizontal\"") != std::string::npos);
    CHECK(layout->content.find("android:hint=\"Email\"") != std::string::npos);
    CHECK(layout->content.find("FrameLayout") == std::string::npos);
    CHECK(layout->content.find("ProgressBar") == std::string::npos);

    REQUIRE(base != nullptr);
    CHECK(base->content.find("EditText email;") != std::string::npos);
    CHECK(base->content.find("setState") == std::string::npos);
    // No handlers => no abstract members => a concrete base class.
    CHECK(base->content.find("public class MainScreenBase") != std::string::npos);
}

TEST_CASE("widget text is XML escaped") {
    auto model = parser::parse_model(R"({
        "schemaVersion": 1,
        "app": { "name": "Escape", "package": "com.oji.escape", "minSdk": 21 },
        "screens": [{
            "id": "home",
            "layout": {
                "type": "column",
                "children": [
                    { "type": "text", "id": "label", "text": "Fish & Chips <3 \"quoted\"" }
                ]
            }
        }]
    })");
    REQUIRE(model.has_value());

    auto generated = codegen::generate(*model, load_templates());
    REQUIRE(generated.has_value());

    const codegen::GeneratedFile& layout = generated->files.front();
    CHECK(layout.content.find("Fish &amp; Chips &lt;3 &quot;quoted&quot;") != std::string::npos);
}

TEST_CASE("write_output keeps an existing user-owned activity") {
    const std::filesystem::path output =
        std::filesystem::temp_directory_path() / "asstudio-writer-test";

    std::error_code ignored;
    std::filesystem::remove_all(output, ignored);

    const parser::AppModel model = load_model("samples/hello.json");
    auto generated = codegen::generate(model, load_templates());
    REQUIRE(generated.has_value());

    auto first = scaffolder::write_output(*generated, output);
    REQUIRE(first.has_value());
    CHECK(first->skipped.empty());
    CHECK_EQ(first->written.size(), 4u);

    // The developer edits the activity; a second run must not touch it.
    const std::filesystem::path activity =
        output / "java/com/oji/hello/HomeActivity.java";
    auto written_activity = core::fs::write_text_file(activity, "// hand edited\n");
    REQUIRE(written_activity.has_value());

    auto second = scaffolder::write_output(*generated, output);
    REQUIRE(second.has_value());
    REQUIRE_EQ(second->skipped.size(), 1u);
    CHECK_EQ(second->skipped.front().generic_string(), "java/com/oji/hello/HomeActivity.java");

    auto content = core::fs::read_text_file(activity);
    REQUIRE(content.has_value());
    CHECK_EQ(*content, "// hand edited\n");

    std::filesystem::remove_all(output, ignored);
}
