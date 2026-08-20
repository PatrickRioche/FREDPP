#include "fred/runtime/ConsoleOutput.hpp"

#include <ostream>

namespace fred {

ConsoleOutput::ConsoleOutput(std::ostream& stream) noexcept
    : stream_(&stream) {}

void ConsoleOutput::write(std::string_view text) {
    // Use ostream::write rather than operator<< so embedded NUL bytes, if ever
    // supplied by a caller, are forwarded according to the explicit length.
    stream_->write(
        text.data(),
        static_cast<std::streamsize>(text.size()));
}

} // namespace fred
