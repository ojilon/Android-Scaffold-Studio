#pragma once

/*
============================================================

Module:
    Core

File:
    Logger.h

Purpose:
    Lightweight logging interface.

Dependencies:
    Standard Library

Used By:
    Entire application

============================================================
*/

#include <string_view>

namespace logger{

enum class Level{
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

}