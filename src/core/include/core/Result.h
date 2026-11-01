#pragma once

/*
============================================================

Module:
    core

File:
    Result.h

Purpose:
    The single error-handling vocabulary used across module
    boundaries.

Why:
    The plan asks for std::expected when the toolchain has it
    (C++23) and a small hand-rolled Result otherwise. GCC 12+
    / Clang 16+ / MSVC 19.33+ ship <expected>, so the alias
    below is all we need; there is no fallback type to keep in
    sync and no exception classes to maintain.

Rules:
    - Modules return core::Result instead of throwing.
    - Exceptions are reserved for main() (bad_alloc, etc).
    - Callers must check before reading: `if (!r) { use(r.error()); }`.

============================================================
*/

#include <expected>
#include <string>

namespace core {

// Human readable failure message. Deliberately a plain string: it travels
// through the CLI, tests and (later) the UI without translation layers.
using Error = std::string;

// Result of an operation that produces a value.
template <typename T, typename E = Error>
using Result = std::expected<T, E>;

// Result of an operation that only reports success or failure.
using VoidResult = Result<void, Error>;

} // namespace core
