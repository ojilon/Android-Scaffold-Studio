#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "parser/AppModelLoader.h"
#include "parser/ParseError.h"
#include <core/Result.h>
#include <string>

using namespace parser;

TEST_CASE("Model Loader parses valid hello.json") {
    auto result = loadModel("samples/hello.json");
    CHECK(result.has_value());
    const auto& model = result.value();
    
    CHECK_EQ(model.schemaVersion, 1);
    CHECK_EQ(model.app.name, "Hello");
    CHECK_EQ(model.app.package, "com.oji.hello");
    CHECK_EQ(model.app.minSdk, 21);
    
    CHECK_EQ(model.screens.size(), 1);
    const auto& screen = model.screens[0];
    CHECK_EQ(screen.id, "home");
    CHECK_EQ(screen.states.size(), 3);
    CHECK(std::find(screen.states.begin(), screen.states.end(), "loading") != screen.states.end());
    CHECK(std::find(screen.states.begin(), screen.states.end(), "content") != screen.states.end());
    CHECK(std::find(screen.states.begin(), screen.states.end(), "error") != screen.states.end());
    
    CHECK_EQ(screen.layout.type, "column");
    CHECK_EQ(screen.layout.children.size(), 2);
    
    const auto& text_widget = screen.layout.children[0];
    CHECK_EQ(text_widget.type, "text");
    CHECK_EQ(text_widget.id, "title");
    CHECK_EQ(text_widget.text, "Hello");
    
    const auto& button_widget = screen.layout.children[1];
    CHECK_EQ(button_widget.type, "button");
    CHECK_EQ(button_widget.id, "retry");
    CHECK_EQ(button_widget.text, "Retry");
    CHECK_EQ(button_widget.onClick, "retry");
}

TEST_CASE("Model Loader fails on missing file") {
    auto result = loadModel("nonexistent.json");
    CHECK(!result.has_value());
    const auto& errors = result.error();
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].id, "FILE_NOT_FOUND");
}