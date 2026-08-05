#pragma once

#include <string_view>

namespace fredpp {
[[nodiscard]] constexpr std::string_view version() noexcept {
    return "0.0.4";
}
}
