#include <core/FileSystem.h>

#include <fstream>
#include <iterator>
#include <system_error>

// Named std_fs, not fs: inside core::fs the name fs would resolve to the
// namespace we are defining, not to std::filesystem.
namespace std_fs = std::filesystem;

namespace core::fs {

Path join(const Path& left, const Path& right) {
    return left / right;
}

Result<bool> exists(const Path& path) {
    std::error_code ec;
    const bool value = std_fs::exists(path, ec);
    if (ec)
        return std::unexpected(ec.message());
    return value;
}

VoidResult create_directories(const Path& path) {
    std::error_code ec;
    std_fs::create_directories(path, ec);
    if (ec)
        return std::unexpected(ec.message());
    return {};
}

VoidResult remove_all(const Path& path) {
    std::error_code ec;
    std_fs::remove_all(path, ec);
    if (ec)
        return std::unexpected(ec.message());
    return {};
}

VoidResult copy_file(const Path& from, const Path& to) {
    std::error_code ec;
    std_fs::copy_file(from, to, std_fs::copy_options::overwrite_existing, ec);
    if (ec)
        return std::unexpected(ec.message());
    return {};
}

Result<std::vector<Path>> list_directory(const Path& path) {
    std::error_code ec;
    std::vector<Path> entries;

    for (const auto& entry : std_fs::directory_iterator(path, ec))
        entries.push_back(entry.path());

    if (ec)
        return std::unexpected(ec.message());
    return entries;
}

Result<Path> canonical(const Path& path) {
    std::error_code ec;
    const Path resolved = std_fs::canonical(path, ec);
    if (ec)
        return std::unexpected(ec.message());
    return resolved;
}

Result<std::string> read_text_file(const Path& path) {
    // Binary mode on purpose: no CRLF translation, so what is on disk is what
    // the parser, the golden tests and the generator all agree on.
    std::ifstream stream(path, std::ios::binary);
    if (!stream)
        return std::unexpected("cannot open file: " + path.string());

    std::string content((std::istreambuf_iterator<char>(stream)),
                        std::istreambuf_iterator<char>());

    if (stream.bad())
        return std::unexpected("cannot read file: " + path.string());

    return content;
}

VoidResult write_text_file(const Path& path, std::string_view content) {
    const Path parent = path.parent_path();
    if (!parent.empty()) {
        // Qualified: std::filesystem::create_directories is also findable
        // through ADL on std::filesystem::path, which makes the unqualified
        // call ambiguous.
        if (auto created = core::fs::create_directories(parent); !created)
            return std::unexpected(created.error());
    }

    std::ofstream stream(path, std::ios::binary | std::ios::trunc);
    if (!stream)
        return std::unexpected("cannot open file for writing: " + path.string());

    stream.write(content.data(), static_cast<std::streamsize>(content.size()));
    if (!stream)
        return std::unexpected("cannot write file: " + path.string());

    return {};
}

} // namespace core::fs
