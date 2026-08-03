#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace fredpp {

class HelpManager {
public:
    [[nodiscard]] bool exists(std::string_view topic) const;
    [[nodiscard]] std::string load(std::string_view topic) const;
    [[nodiscard]] std::vector<std::string> topics() const;

    [[nodiscard]] static std::string normalize_topic(std::string_view topic);
};

} // namespace fredpp
