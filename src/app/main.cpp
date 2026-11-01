/*
============================================================

Module:
    app

File:
    main.cpp

Purpose:
    Command line entry point.

Commands:
    AndroidScaffoldStudio validate <model.json>
    AndroidScaffoldStudio generate <model.json> [-o <dir>] [--templates <dir>]
    AndroidScaffoldStudio --help
    AndroidScaffoldStudio --version

Exit codes:
    0  success / model valid
    1  model invalid, or generation failed
    2  bad command line

Exceptions are caught only here; the libraries report through
core::Result.

============================================================
*/

#include <codegen/Generation.h>
#include <core/Logger.h>
#include <parser/AppModelLoader.h>
#include <scaffolder/ProjectWriter.h>
#include <scaffolder/Templates.h>
#include <validator/Validator.h>

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#ifndef ASSTUDIO_VERSION
#define ASSTUDIO_VERSION "0.0.0"
#endif

namespace {

constexpr int exit_ok = 0;
constexpr int exit_failure = 1;
constexpr int exit_usage = 2;

struct Options {
    std::string command;
    std::string model_path;
    std::string output_directory = "generated";
    std::string template_directory; // empty => scaffolder default
};

void print_usage(std::string_view program) {
    core::logger::info(
        std::string("Usage:\n") +
        "  " + std::string(program) + " validate <model.json>\n" +
        "  " + std::string(program) + " generate <model.json> [-o <outdir>] [--templates <dir>]\n" +
        "  " + std::string(program) + " --help\n" +
        "  " + std::string(program) + " --version\n" +
        "\n" +
        "Commands:\n" +
        "  validate   check the model against the Android rules (exit 1 on errors)\n" +
        "  generate   validate, then write layout XML, Java and the manifest\n" +
        "\n" +
        "Options:\n" +
        "  -o <outdir>        output directory for generate (default: generated)\n" +
        "  --templates <dir>  template directory (default: the repo's templates/)\n");
}

// Returns an empty string on success, otherwise the message to report.
std::string parse_options(const std::vector<std::string>& arguments, Options& options) {
    if (arguments.empty())
        return "no command given; try --help";

    options.command = arguments.front();

    for (std::size_t i = 1; i < arguments.size(); ++i) {
        const std::string& argument = arguments.at(i);

        if (argument == "-o" || argument == "--output") {
            if (i + 1 >= arguments.size())
                return argument + " needs a directory";
            options.output_directory = arguments.at(++i);
        } else if (argument == "--templates") {
            if (i + 1 >= arguments.size())
                return argument + " needs a directory";
            options.template_directory = arguments.at(++i);
        } else if (!argument.empty() && argument.front() == '-') {
            return "unknown option: " + argument;
        } else if (options.model_path.empty()) {
            options.model_path = argument;
        } else {
            return "unexpected extra argument: " + argument;
        }
    }

    return {};
}

void report_parse_errors(const std::vector<parser::ParseError>& errors) {
    for (const parser::ParseError& error : errors)
        core::logger::error("[" + error.id + "] " + error.path + ": " + error.message);
}

void report_violations(const std::vector<validator::RuleViolation>& violations) {
    for (const validator::RuleViolation& violation : violations)
        core::logger::error("[" + violation.id + "] " + violation.path + ": " + violation.message);
}

int load_and_validate(const std::string& model_path, parser::AppModel& model) {
    auto loaded = parser::load_model(model_path);
    if (!loaded) {
        report_parse_errors(loaded.error());
        return exit_failure;
    }

    auto valid = validator::validate(*loaded);
    if (!valid) {
        report_violations(valid.error());
        return exit_failure;
    }

    model = std::move(*loaded);
    return exit_ok;
}

int run_validate(const Options& options) {
    if (options.model_path.empty()) {
        core::logger::error("validate needs a model file");
        return exit_usage;
    }

    parser::AppModel model;
    if (const int result = load_and_validate(options.model_path, model); result != exit_ok)
        return result;

    core::logger::info(options.model_path + ": valid (" +
                       std::to_string(model.screens.size()) + " screen(s))");
    return exit_ok;
}

int run_generate(const Options& options) {
    if (options.model_path.empty()) {
        core::logger::error("generate needs a model file");
        return exit_usage;
    }

    parser::AppModel model;
    if (const int result = load_and_validate(options.model_path, model); result != exit_ok) {
        core::logger::error("generation aborted: the model has errors");
        return result;
    }

    const core::fs::Path template_directory = options.template_directory.empty()
                                                  ? scaffolder::default_template_directory()
                                                  : core::fs::Path(options.template_directory);

    auto templates = scaffolder::load_templates(template_directory);
    if (!templates) {
        core::logger::error(templates.error());
        return exit_failure;
    }

    auto generated = codegen::generate(model, *templates);
    if (!generated) {
        core::logger::error("generation failed: " + generated.error());
        return exit_failure;
    }

    auto written = scaffolder::write_output(*generated, core::fs::Path(options.output_directory));
    if (!written) {
        core::logger::error("writing output failed: " + written.error());
        return exit_failure;
    }

    for (const core::fs::Path& path : written->written)
        core::logger::info("wrote " + path.generic_string());
    for (const core::fs::Path& path : written->skipped)
        core::logger::info("kept  " + path.generic_string() + " (user-owned, already exists)");

    core::logger::info("generation finished in " + options.output_directory);
    return exit_ok;
}

} // namespace

int main(int argc, char* argv[]) {
    std::vector<std::string> arguments;
    arguments.reserve(static_cast<std::size_t>(argc > 0 ? argc - 1 : 0));
    for (int i = 1; i < argc; ++i)
        arguments.emplace_back(argv[i]);

    const std::string program = (argc > 0) ? argv[0] : "AndroidScaffoldStudio";

    if (!arguments.empty()) {
        const std::string& first = arguments.front();
        if (first == "--help" || first == "-h" || first == "help") {
            print_usage(program);
            return exit_ok;
        }
        if (first == "--version" || first == "-v") {
            core::logger::info(std::string("Android Scaffold Studio ") + ASSTUDIO_VERSION);
            return exit_ok;
        }
    }

    Options options;
    const std::string parse_error = parse_options(arguments, options);
    if (!parse_error.empty()) {
        core::logger::error(parse_error);
        print_usage(program);
        return exit_usage;
    }

    if (options.command == "validate")
        return run_validate(options);
    if (options.command == "generate")
        return run_generate(options);

    core::logger::error("unknown command: " + options.command);
    print_usage(program);
    return exit_usage;
}
