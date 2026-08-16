#pragma once

#include <cstddef>

namespace fred::limits {

// FRED historical compatibility: 15 characters; FREDPP extension: 64.
inline constexpr std::size_t max_buffer_name_length = 64;

} // namespace fred::limits
