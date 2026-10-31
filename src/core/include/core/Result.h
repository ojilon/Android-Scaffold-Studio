#pragma once

/*
============================================================

Module:
    Core

File:
    Result.h

Purpose:
    Project-wide aliases around std::expected, used across
    module boundaries to return values or errors without
    throwing exceptions.

    The codebase historically used three spellings; all are
    provided here so every module compiles:

      expected::Result<T> / expected::VoidResult   (Filesystem)
      result::Result<T>   / result::VoidResult      (Settings, Gradle)
      core::Result<T, E>  (parser, validator, codegen)

    `expected::Result` and `result::Result` fix the error type to
    std::string; `core::Result` lets the caller pick E.

Dependencies:
    <expected>
    <string>

============================================================
*/

#include <expected>
#include <string>

namespace expected {

using Error = std::string;

template<typename T>
using Result = std::expected<T, Error>;

using VoidResult = Result<void>;

} // namespace expected

namespace result {

template<typename T>
using Result = std::expected<T, std::string>;

using VoidResult = Result<void>;

} // namespace result

namespace core {

template<typename T, typename E = std::string>
using Result = std::expected<T, E>;

} // namespace core
