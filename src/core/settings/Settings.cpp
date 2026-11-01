#include <core/Settings.h>

namespace settings {

Settings defaults() {
    Settings s;
    s.dark_theme = true;
    // leave paths empty for now
    return s;
}

core::VoidResult save(const Settings& /*s*/, const filesystem::Path& /*path*/) {
    return std::unexpected("settings::save not implemented yet");
}

core::Result<Settings> load(const filesystem::Path& /*path*/) {
    return std::unexpected("settings::load not implemented yet");
}

} // namespace settings