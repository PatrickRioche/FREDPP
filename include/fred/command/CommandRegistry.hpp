#pragma once

#include "fred/command/CommandDescriptor.hpp"

#include <optional>
#include <unordered_map>

namespace fred {

class CommandRegistry {
public:
    void register_command(CommandDescriptor descriptor);

    [[nodiscard]] const CommandDescriptor* find(char mnemonic) const noexcept;
    [[nodiscard]] bool contains(char mnemonic) const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;

private:
    std::unordered_map<char, CommandDescriptor> commands_;
};

[[nodiscard]] CommandRegistry make_core_command_registry();

} // namespace fred
