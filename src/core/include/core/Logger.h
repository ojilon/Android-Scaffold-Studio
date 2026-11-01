#pragma once

/*
============================================================

Module:
    core

File:
    Logger.h

Purpose:
    Tiny line logger for the CLI and the future UI log pane.

Why free functions:
    A logger is a process-wide sink. Hiding it in a singleton
    class buys nothing; a namespace and one level flag are
    simpler and keep call sites short.

Note:
    The level is global mutable state. That is acceptable for
    a single-threaded tool; if the UI thread arrives later it
    owns the sink and this header becomes the seam.

============================================================
*/

#include <string_view>

namespace core::logger {

enum class Level {
    Debug,
    Info,
    Warning,
    Error
};

void set_level(Level level);
Level level();

void debug(std::string_view message);
void info(std::string_view message);
void warning(std::string_view message);
void error(std::string_view message);

} // namespace core::logger
