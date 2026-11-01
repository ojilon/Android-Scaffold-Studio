#include <core/Logger.h>

#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

namespace {

core::logger::Level g_level = core::logger::Level::Info;

const char* level_name(core::logger::Level level) {
    switch (level) {
        case core::logger::Level::Debug:   return "DEBUG";
        case core::logger::Level::Info:    return "INFO";
        case core::logger::Level::Warning: return "WARNING";
        case core::logger::Level::Error:   return "ERROR";
    }
    return "UNKNOWN";
}

bool should_print(core::logger::Level level) {
    return static_cast<int>(level) >= static_cast<int>(g_level);
}

// Local wall-clock time as "YYYY-MM-DD HH:MM:SS".
// std::strftime + std::localtime keeps this portable; <format> is not used
// anywhere in the codebase because GCC 12 and older MSVC lack it.
std::string timestamp() {
    const std::time_t now = std::time(nullptr);

    std::tm parts{};
#if defined(_WIN32)
    localtime_s(&parts, &now);
#else
    localtime_r(&now, &parts);
#endif

    char buffer[32] = {};
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &parts);
    return buffer;
}

void print(core::logger::Level level, std::string_view message) {
    if (!should_print(level))
        return;

    std::ostringstream line;
    line << '[' << timestamp() << "] " << level_name(level) << ": " << message << '\n';
    std::cout << line.str() << std::flush;
}

} // namespace

namespace core::logger {

void set_level(Level level) {
    g_level = level;
}

Level level() {
    return g_level;
}

void debug(std::string_view message)   { print(Level::Debug, message); }
void info(std::string_view message)    { print(Level::Info, message); }
void warning(std::string_view message) { print(Level::Warning, message); }
void error(std::string_view message)   { print(Level::Error, message); }

} // namespace core::logger
