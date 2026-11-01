#include <scaffolder/Templates.h>

#include <scaffolder/Config.h>

#include <string>

namespace scaffolder {

namespace {

core::Result<std::string> read_template(const core::fs::Path& directory, const char* relative) {
    const core::fs::Path path = directory / relative;

    auto text = core::fs::read_text_file(path);
    if (!text)
        return std::unexpected("template " + path.string() + ": " + text.error());

    return text;
}

} // namespace

core::fs::Path default_template_directory() {
    return core::fs::Path(ASSTUDIO_TEMPLATE_DIR);
}

core::Result<codegen::TemplateSet> load_templates(const core::fs::Path& directory) {
    auto exists = core::fs::exists(directory);
    if (!exists)
        return std::unexpected("template directory " + directory.string() + ": " + exists.error());
    if (!*exists)
        return std::unexpected("template directory not found: " + directory.string());

    codegen::TemplateSet templates;

    auto load = [&](const char* relative, std::string& target) -> core::VoidResult {
        auto text = read_template(directory, relative);
        if (!text)
            return std::unexpected(text.error());
        target = std::move(*text);
        return {};
    };

    if (auto loaded = load("xml/layout.xml.in", templates.layout); !loaded)
        return std::unexpected(loaded.error());
    if (auto loaded = load("xml/layout_states.xml.in", templates.layout_states); !loaded)
        return std::unexpected(loaded.error());
    if (auto loaded = load("java/ScreenBase.java.in", templates.screen_base); !loaded)
        return std::unexpected(loaded.error());
    if (auto loaded = load("java/Activity.java.in", templates.activity); !loaded)
        return std::unexpected(loaded.error());
    if (auto loaded = load("android/AndroidManifest.xml.in", templates.manifest); !loaded)
        return std::unexpected(loaded.error());

    return templates;
}

} // namespace scaffolder
