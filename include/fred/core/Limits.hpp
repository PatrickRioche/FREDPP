#pragma once

#include <cstddef>

namespace fred::limits {

/**
 * @brief Maximum buffer-name length accepted by FREDPP.
 *
 * Historical FRED compatibility used 15 characters. FREDPP deliberately
 * extends this implementation limit to 64 characters.
 *
 * @note This is an explicit FREDPP extension, not a claim about the historical
 *       language limit.
 */
inline constexpr std::size_t max_buffer_name_length = 64;

} // namespace fred::limits
