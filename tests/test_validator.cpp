#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "validator/Validator.h"
#include <core/Result.h>
#include <string>

using namespace validator;

TEST_CASE("Validator V001: Unique IDs") {
    std::string json = R"({
      "schemaVersion": 1,
      "app": { "name": "A", "package": "com.a", "minSdk": 21 },
      "screens": [{
        "id": "home",
        "layout": {
          "type": "column",
          "children": [
            { "type": "text", "id": "t1" },
            { "type": "text", "id": "t1" }
          ]
        }
      }]
    })";
    auto res = Validator::validate(json);
    CHECK(!res.has_value());
    const auto& errs = res.error();
    CHECK(std::any_of(errs.begin(), errs.end(), [](const RuleViolation& v){ return v.id == "V001" && v.path == "$.screens[0].layout.children[1].id"; }));
}

TEST_CASE("Validator V002: onClick valid and unique") {
    std::string json1 = R"({
      "schemaVersion": 1,
      "app": { "name": "A", "package": "com.a", "minSdk": 21 },
      "screens": [{
        "id": "home",
        "layout": {
          "type": "column",
          "children": [
            { "type": "button", "id": "b1", "onClick": "123bad" }
          ]
        }
      }]
    })";
    auto res1 = Validator::validate(json1);
    CHECK(!res1.has_value());
    CHECK(std::any_of(res1.error().begin(), res1.error().end(), [](const RuleViolation& v){ return v.id == "V002"; }));

    std::string json2 = R"({
      "schemaVersion": 1,
      "app": { "name": "A", "package": "com.a", "minSdk": 21 },
      "screens": [{
        "id": "home",
        "layout": {
          "type": "column",
          "children": [
            { "type": "button", "id": "b1", "onClick": "retry" },
            { "type": "button", "id": "b2", "onClick": "retry" }
          ]
        }
      }]
    })";
    auto res2 = Validator::validate(json2);
    CHECK(!res2.has_value());
    CHECK(std::any_of(res2.error().begin(), res2.error().end(), [](const RuleViolation& v){ return v.id == "V002" && v.path == "$.screens[0].layout.children[1].onClick"; }));
}

TEST_CASE("Validator V003: Allowed widget types") {
    std::string json = R"({
      "schemaVersion": 1,
      "app": { "name": "A", "package": "com.a", "minSdk": 21 },
      "screens": [{
        "id": "home",
        "layout": {
          "type": "foo",
          "children": [{ "type": "bar", "id": "t1" }]
        }
      }]
    })";
    auto res = Validator::validate(json);
    CHECK(!res.has_value());
    bool found_layout = false, found_child = false;
    for (const auto& v : res.error()) {
        if (v.id == "V003") {
            if (v.path.find(".layout.type") != std::string::npos) found_layout = true;
            if (v.path.find(".children[0].type") != std::string::npos) found_child = true;
        }
    }
    CHECK(found_layout && found_child);
}

TEST_CASE("Validator V004: Package, Name, minSdk") {
    std::string json1 = R"({
      "schemaVersion": 1,
      "app": { "name": "", "package": "com.a", "minSdk": 21 },
      "screens": []
    })";
    CHECK(!Validator::validate(json1).has_value());

    std::string json2 = R"({
      "schemaVersion": 1,
      "app": { "name": "A", "package": "123bad.pkg", "minSdk": 21 },
      "screens": []
    })";
    CHECK(!Validator::validate(json2).has_value());

    std::string json3 = R"({
      "schemaVersion": 1,
      "app": { "name": "A", "package": "com.a", "minSdk": 10 },
      "screens": []
    })";
    auto res3 = Validator::validate(json3);
    CHECK(!res3.has_value());
    CHECK(std::any_of(res3.error().begin(), res3.error().end(), [](const RuleViolation& v){ return v.id == "V007"; }));
}

TEST_CASE("Validator V005: Screen IDs") {
    std::string json = R"({
      "schemaVersion": 1,
      "app": { "name": "A", "package": "com.a", "minSdk": 21 },
      "screens": [
        { "id": "home", "layout": { "type": "column", "children": [] } },
        { "id": "home", "layout": { "type": "column", "children": [] } }
      ]
    })";
    auto res = Validator::validate(json);
    CHECK(!res.has_value());
    CHECK(std::any_of(res.error().begin(), res.error().end(), [](const RuleViolation& v){ return v.id == "V005"; }));
}

TEST_CASE("Validator V006: States") {
    std::string json1 = R"({
      "schemaVersion": 1,
      "app": { "name": "A", "package": "com.a", "minSdk": 21 },
      "screens": [{ "id": "home", "states": [], "layout": { "type": "column", "children": [] } }]
    })";
    CHECK(!Validator::validate(json1).has_value());

    std::string json2 = R"({
      "schemaVersion": 1,
      "app": { "name": "A", "package": "com.a", "minSdk": 21 },
      "screens": [{ "id": "home", "states": ["unknown"], "layout": { "type": "column", "children": [] } }]
    })";
    CHECK(!Validator::validate(json2).has_value());
}

TEST_CASE("Validator V008: Android Resource Names") {
    std::string json = R"({
      "schemaVersion": 1,
      "app": { "name": "A", "package": "com.a", "minSdk": 21 },
      "screens": [{
        "id": "home",
        "layout": {
          "type": "column",
          "children": [{ "type": "text", "id": "123bad" }]
        }
      }]
    })";
    auto res = Validator::validate(json);
    CHECK(!res.has_value());
    CHECK(std::any_of(res.error().begin(), res.error().end(), [](const RuleViolation& v){ return v.id == "V008"; }));
}

TEST_CASE("Validator Success on Golden Sample") {
    std::ifstream file("samples/hello.json");
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    auto res = Validator::validate(content);
    CHECK(res.has_value());
}
