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

/**
 * @brief Shared safety budgets for all nested U repeat iterations.
 *
 * These are FREDPP runaway-execution limits, not historical FRED syntax
 * limits. They can be increased through the documented environment options.
 */
inline constexpr std::size_t default_repeat_max_steps = 1'000'000;
inline constexpr std::size_t default_repeat_max_seconds = 60;

} // namespace fred::limits
