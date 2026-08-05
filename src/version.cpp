#include "fredpp/version.hpp"
#include "VersionInfo.hpp"

namespace fredpp {

std::string_view version() noexcept {
    return FREDPP_VERSION_STRING;
}

std::string_view git_commit() noexcept {
    return FREDPP_GIT_COMMIT_STRING;
}

std::string_view source_state() noexcept {
    return FREDPP_GIT_STATE_STRING;
}

bool source_is_dirty() noexcept {
    return FREDPP_GIT_DIRTY_VALUE != 0;
}

} // namespace fredpp
