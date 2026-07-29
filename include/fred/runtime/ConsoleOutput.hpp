#pragma once

#include "fred/runtime/Output.hpp"

#include <iosfwd>

namespace fred {

class ConsoleOutput final : public Output {
public:
    explicit ConsoleOutput(std::ostream& stream) noexcept;

    void write(std::string_view text) override;

private:
    std::ostream* stream_;
};

} // namespace fred
