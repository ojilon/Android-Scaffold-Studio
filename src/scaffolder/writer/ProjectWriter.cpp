#include <scaffolder/ProjectWriter.h>

#include <utility>

namespace scaffolder {

core::Result<WriteReport> write_output(const codegen::GenerationOutput& output,
                                       const core::fs::Path& output_directory) {
    WriteReport report;

    for (const codegen::GeneratedFile& file : output.files) {
        const core::fs::Path target = output_directory / file.relative_path;

        if (file.user_owned) {
            auto exists = core::fs::exists(target);
            if (!exists)
                return std::unexpected(exists.error());
            if (*exists) {
                report.skipped.push_back(file.relative_path);
                continue;
            }
        }

        if (auto written = core::fs::write_text_file(target, file.content); !written)
            return std::unexpected(written.error());

        report.written.push_back(file.relative_path);
    }

    return report;
}

} // namespace scaffolder
