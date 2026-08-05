#pragma once

#include <string_view>

namespace fredpp {

[[nodiscard]] std::string_view version() noexcept;
[[nodiscard]] std::string_view git_commit() noexcept;
[[nodiscard]] std::string_view source_state() noexcept;
[[nodiscard]] bool source_is_dirty() noexcept;

} // namespace fredpp
