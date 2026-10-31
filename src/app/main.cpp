#include <iostream>
#include <string>
#include <core/Logger.h>
#include <validator/Validator.h>

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
    
    // Read model.json (placeholder for now)
    // Validate or Generate (placeholder for now)

    if (command == "validate") {
        // validator::Validator::validate logic
        core::Logger::info("Validation successful (placeholder)");
        return 0;
    } else if (command == "generate") {
        // codegen logic
        core::Logger::info("Generation successful (placeholder)");
        return 0;
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        return 1;
    }
}