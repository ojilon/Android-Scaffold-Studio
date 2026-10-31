#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "codegen/Scaffolder.h"
#include "parser/AppModelLoader.h"
#include <core/Result.h>
#include <fstream>

using namespace codegen;
using namespace parser;

TEST_CASE("Codegen matches golden sample") {
    auto model_res = loadModel("samples/hello.json");
    CHECK(model_res.has_value());
    const auto& model = model_res.value();

    auto gen_res = generate(model);
    CHECK(gen_res.has_value());
    const auto& gen = gen_res.value();

    // Compare with docs/golden/hello/
    auto read_file = [](const std::string& path) {
        std::ifstream f(path);
        return std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    };

    CHECK(gen.layout_xml == read_file("docs/golden/hello/res_layout_home.xml"));
    CHECK(gen.screen_base_java == read_file("docs/golden/hello/HomeScreenBase.java"));
    CHECK(gen.activity_java == read_file("docs/golden/hello/HomeActivity.java"));
    CHECK(gen.manifest_xml == read_file("docs/golden/hello/AndroidManifest.xml"));
}
