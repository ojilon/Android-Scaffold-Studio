#pragma once

/*
============================================================

Module:
    Core

File:
    Expected.h

Purpose:
    Project-wide aliases around std::expected.

Dependencies:
    <expected>
    <string>

Used By:
    Entire application

============================================================
*/

#include <expected>
#include <string>

namespace expected {

using Error = std::string;

template<typename T>
using Result = std::expected<T, Error>;

using VoidResult = Result<void>;

}