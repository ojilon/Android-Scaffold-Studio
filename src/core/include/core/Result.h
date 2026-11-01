#pragma once

#include <expected>
#include <string>
#include <vector>

namespace core {

// Primary alias used everywhere
template<typename T, typename E = std::string>
using Result = std::expected<T, E>;

// Convenience for void success / string error
using VoidResult = Result<void, std::string>;

} // namespace core