#include <iostream>
#include <string>
#include <core/Logger.h>
#include <parser/AppModelLoader.h>
#include <validator/Validator.h>
#include <codegen/Scaffolder.h>
#include <fstream>

int main(int argc, char* argv[]) {
    core::Logger::info("Android Scaffold Studio starting...");
    
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [validate|generate] <model.json> [-o <outdir>]" << std::endl;
        return 1;
    }

    std::string command = argv[1];
    if (command == "--help") {
        std::cout << "Android Scaffold Studio v0.1.0" << std::endl;
        std::cout << "Usage: " << argv[0] << " [validate|generate] <model.json> [-o <outdir>]" << std::endl;
        return 0;
    }

    if (argc < 3) {
        std::cerr << "Missing <model.json> argument" << std::endl;
        return 1;
    }

    std::string model_path = argv[2];
    
    // 1. Load
    auto load_res = parser::loadModel(model_path);
    if (!load_res.has_value()) {
        for (const auto& err : load_res.error()) {
            std::cerr << "[" << err.id << "] " << err.path << ": " << err.message << std::endl;
        }
        return 1;
    }
    const auto& model = load_res.value();

    if (command == "validate") {
        // 2. Validate
        auto val_res = validator::Validator::validate(model_path); // Note: validator takes string content in my impl, let's fix main to read file
        // Wait, I implemented Validator::validate(const std::string& json_content)
        std::ifstream f(model_path);
        std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        val_res = validator::Validator::validate(content);

        if (!val_res.has_value()) {
            for (const auto& err : val_res.error()) {
                std::cerr << "[" << err.id << "] " << err.path << ": " << err.message << " (" << err.severity << ")\n";
            }
            return 1;
        }
        core::Logger::info("Validation successful");
        return 0;

    } else if (command == "generate") {
        // Validate first
        std::ifstream f(model_path);
        std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        auto val_res = validator::Validator::validate(content);
        if (!val_res.has_value()) {
            std::cerr << "Validation failed. Generation aborted." << std::endl;
            for (const auto& err : val_res.error()) {
                std::cerr << "[" << err.id << "] " << err.path << ": " << err.message << std::endl;
            }
            return 1;
        }

        // Output directory
        std::string outdir = "generated";
        if (argc >= 5 && std::string(argv[3]) == "-o") {
            outdir = argv[4];
        }

        // 3. Generate
        auto gen_res = codegen::generate(model);
        if (!gen_res.has_value()) {
            std::cerr << "Generation failed: " << gen_res.error() << std::endl;
            return 1;
        }
        const auto& gen = gen_res.value();

        // Write files
        auto write_file = [](const std::string& path, const std::string& content) {
            std::ofstream f(path);
            f << content;
        };

        // Ensure directories exist (simplified for v1)
        write_file(outdir + "/res_layout_" + model.screens[0].id + ".xml", gen.layout_xml);
        write_file(outdir + "/" + model.screens[0].id + "ScreenBase.java", gen.screen_base_java);
        write_file(outdir + "/" + model.screens[0].id + "Activity.java", gen.activity_java);
        write_file(outdir + "/AndroidManifest.xml", gen.manifest_xml);

        core::Logger::info("Generation successful. Files written to " + outdir);
        return 0;
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        return 1;
    }
}
