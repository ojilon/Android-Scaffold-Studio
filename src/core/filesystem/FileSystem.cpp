#include <core/FileSystem.h>   // note the exact casing

#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

namespace filesystem {

core::Result<bool> exists(const Path& path) {
    std::error_code ec;
    bool value = fs::exists(path, ec);
    if (ec)
        return std::unexpected(ec.message());
    return value;
}

core::VoidResult create_directory(const Path& path) {
    std::error_code ec;
    fs::create_directories(path, ec);
    if (ec)
        return std::unexpected(ec.message());
    return {};
}

core::VoidResult remove(const Path& path) {
    std::error_code ec;
    fs::remove_all(path, ec);          // remove_all is safer
    if (ec)
        return std::unexpected(ec.message());
    return {};
}

core::VoidResult copy_file(const Path& from, const Path& to) {
    std::error_code ec;
    fs::copy_file(from, to, fs::copy_options::overwrite_existing, ec);
    if (ec)
        return std::unexpected(ec.message());
    return {};
}

core::Result<std::vector<Path>> list_directory(const Path& path) {
    std::error_code ec;
    std::vector<Path> files;
    for (const auto& entry : fs::directory_iterator(path, ec)) {
        files.push_back(entry.path());
    }
    if (ec)
        return std::unexpected(ec.message());
    return files;
}

core::Result<Path> canonical(const Path& path) {
    std::error_code ec;
    auto result = fs::canonical(path, ec);
    if (ec)
        return std::unexpected(ec.message());
    return result;
}

Path join(const Path& left, const Path& right) {
    return left / right;
}

} // namespace filesystem