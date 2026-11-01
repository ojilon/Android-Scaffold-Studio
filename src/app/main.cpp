#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include <core/Logger.h>
#include <parser/AppModelLoader.h>
#include <validator/Validator.h>
#include <codegen/Scaffolder.h>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    logger::info("Android Scaffold Studio starting...");

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0]
                  << " [validate|generate] <model.json> [-o <outdir>]\n";
        return 1;
    }

    std::string command = argv[1];
    if (command == "--help" || command == "-h") {
        std::cout << "Android Scaffold Studio v0.1.0\n"
                  << "Usage: " << argv[0]
                  << " [validate|generate] <model.json> [-o <outdir>]\n";
        return 0;
    }

    if (argc < 3) {
        std::cerr << "Missing <model.json> argument\n";
        return 1;
    }

    std::string model_path = argv[2];

    // ---- load ----
    auto load_res = parser::loadModel(model_path);
    if (!load_res) {
        for (const auto& err : load_res.error()) {
            std::cerr << "[" << err.id << "] " << err.path << ": " << err.message << "\n";
        }
        return 1;
    }
    const auto& model = *load_res;

    // ---- read raw content for validator ----
    std::ifstream f(model_path);
    if (!f) {
        std::cerr << "Cannot re-open " << model_path << "\n";
        return 1;
    }
    std::string content((std::istreambuf_iterator<char>(f)),
                         std::istreambuf_iterator<char>());

    if (command == "validate") {
        auto val_res = validator::validate(content);
        if (!val_res) {
            for (const auto& err : val_res.error()) {
                std::cerr << "[" << err.id << "] " << err.path
                          << ": " << err.message << " (" << err.severity << ")\n";
            }
            return 1;
        }
        logger::info("Validation successful");
        return 0;
    }

    if (command == "generate") {
        // validate first
        auto val_res = validator::validate(content);
        if (!val_res) {
            std::cerr << "Validation failed – generation aborted\n";
            for (const auto& err : val_res.error()) {
                std::cerr << "[" << err.id << "] " << err.path << ": " << err.message << "\n";
            }
            return 1;
        }

        std::string outdir = "generated";
        if (argc >= 5 && std::string(argv[3]) == "-o")
            outdir = argv[4];

        auto gen_res = codegen::generate(model);
        if (!gen_res) {
            std::cerr << "Generation failed: " << gen_res.error() << "\n";
            return 1;
        }
        const auto& gen = *gen_res;

        // ensure directory
        std::error_code ec;
        fs::create_directories(outdir, ec);
        if (ec) {
            std::cerr << "Cannot create output directory: " << ec.message() << "\n";
            return 1;
        }

        auto write = [&](const std::string& name, const std::string& body) -> bool {
            fs::path p = fs::path(outdir) / name;
            std::ofstream out(p);
            if (!out) {
                std::cerr << "Failed to write " << p << "\n";
                return false;
            }
            out << body;
            return true;
        };

        if (model.screens.empty()) {
            std::cerr << "No screens to generate\n";
            return 1;
        }
        const auto& screen = model.screens[0];

        bool ok = true;
        ok &= write("res_layout_" + screen.id + ".xml", gen.layout_xml);
        ok &= write(screen.id + "ScreenBase.java",     gen.screen_base_java);
        ok &= write(screen.id + "Activity.java",       gen.activity_java);
        ok &= write("AndroidManifest.xml",             gen.manifest_xml);

        if (!ok)
            return 1;

        logger::info("Generation successful → " + outdir);
        return 0;
    }

    std::cerr << "Unknown command: " << command << "\n";
    return 1;
}