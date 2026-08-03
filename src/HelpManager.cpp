#include "HelpManager.h"
#include "EmbeddedHelp.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace fredpp {

std::string HelpManager::normalize_topic(std::string_view topic) {
    const auto first = topic.find_first_not_of(" \t\r\n");
    const auto last = topic.find_last_not_of(" \t\r\n");

    if (first == std::string_view::npos) {
        return "index";
    }

    std::string normalized(topic.substr(first, last - first + 1));
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
        [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

    if (normalized == "h" || normalized == "help") {
        return "index";
    }
    return normalized;
}

bool HelpManager::exists(std::string_view topic) const {
    const auto normalized = normalize_topic(topic);
    return embedded_help::find(normalized) != nullptr;
}

std::string HelpManager::load(std::string_view topic) const {
    const auto normalized = normalize_topic(topic);
    if (const auto* text = embedded_help::find(normalized)) {
        return std::string(*text);
    }
    throw std::runtime_error("Aucune rubrique d'aide : " + normalized);
}

std::vector<std::string> HelpManager::topics() const {
    auto result = embedded_help::topics();
    std::sort(result.begin(), result.end());
    return result;
}

} // namespace fredpp
