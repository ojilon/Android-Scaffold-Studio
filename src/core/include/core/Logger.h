#pragma once

#include <string_view>

namespace logger {

enum class Level {
    Debug,
    Info,
    Warning,
    Error
};

void set_level(Level level);
void debug(std::string_view message);
void info(std::string_view message);
void warning(std::string_view message);
void error(std::string_view message);

} // namespace logger