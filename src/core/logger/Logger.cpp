#include <core/Logger.h>

#include <chrono>
#include <format>
#include <iostream>
#include <string>

namespace {

logger::Level current_level = logger::Level::Info;

const char* level_string(logger::Level level) {
    switch (level) {
        case logger::Level::Debug:   return "DEBUG";
        case logger::Level::Info:    return "INFO";
        case logger::Level::Warning: return "WARNING";
        case logger::Level::Error:   return "ERROR";
    }
    return "UNKNOWN";
}

bool should_print(logger::Level level) {
    return static_cast<int>(level) >= static_cast<int>(current_level);
}

void print(logger::Level level, std::string_view message) {
    if (!should_print(level))
        return;

    auto now = std::chrono::system_clock::now();
    auto t   = std::chrono::system_clock::to_time_t(now);

    // simple timestamp – avoids format issues with time_point on some compilers
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

    std::cout << std::format("[{}] {} : {}\n", level_string(level), buf, message);
}

} // anonymous namespace

namespace logger {

void set_level(Level level) {
    current_level = level;
}

void debug(std::string_view message)   { print(Level::Debug,   message); }
void info(std::string_view message)    { print(Level::Info,    message); }
void warning(std::string_view message) { print(Level::Warning, message); }
void error(std::string_view message)   { print(Level::Error,   message); }

} // namespace logger