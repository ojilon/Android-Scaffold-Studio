/*
============================================================

Module:
    Core

File:
    Logger.cpp

Purpose:
    Logger implementation.

============================================================
*/

#include <core/Logger.h>
#include <chrono>
#include <format>
#include <iostream>

namespace {
    logger::Level current_level = logger::Level::Info;

    const char* level_string(logger::Level level) {
        switch (level) {
            case logger::Level::Debug:
                return "DEBUG";

            case logger::Level::Info:
                return "INFO";

            case logger::Level::Warning:
                return "WARNING";

            case logger::Level::Error:
                return "ERROR";
        }

        return "UNKNOWN";
    }

    bool should_print(logger::Level level){
        return static_cast<int>(level) >= static_cast<int>(current_level);
    }

    void print(logger::Level level, std::string_view message) {
        if (!should_print(level))
            return;

        auto now = std::chrono::system_clock::now();

        std::cout << std::format( "[{}] {} : {}\n", level_string(level), now, message );
    }

}

namespace logger {
    void set_level(Level level) {
        current_level = level;
    }

    void debug(std::string_view message) {
        print(Level::Debug, message);
    }

    void info(std::string_view message) {
        print(Level::Info, message);
    }

    void warning(std::string_view message){
        print(Level::Warning, message);
    }

    void error(std::string_view message){
        print(Level::Error, message);
    }

}