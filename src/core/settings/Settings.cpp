#include <core/Settings.h>

namespace core::settings {

Settings defaults() {
    // Paths stay empty: the setup wizard (Phase 3) fills them in.
    return Settings{};
}

VoidResult save(const Settings& /*settings*/, const fs::Path& /*path*/) {
    return std::unexpected("settings::save is not implemented in Phase 1");
}

Result<Settings> load(const fs::Path& /*path*/) {
    return std::unexpected("settings::load is not implemented in Phase 1");
}

} // namespace core::settings
