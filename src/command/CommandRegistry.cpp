#include "fred/command/CommandRegistry.hpp"

#include <stdexcept>
#include <utility>

namespace fred {

void CommandRegistry::register_command(CommandDescriptor descriptor) {
    if (descriptor.mnemonic == '\0') {
        throw std::invalid_argument("command mnemonic may not be NUL");
    }
    if (descriptor.name.empty()) {
        throw std::invalid_argument("command name may not be empty");
    }
    if (!descriptor.parse) {
        throw std::invalid_argument("command parser may not be empty");
    }

    const auto mnemonic = descriptor.mnemonic;
    const auto [iterator, inserted] = commands_.emplace(
        mnemonic, std::move(descriptor));
    (void)iterator;
    if (!inserted) {
        throw std::invalid_argument(
            std::string("command '") + mnemonic + "' is already registered");
    }
}

const CommandDescriptor* CommandRegistry::find(char mnemonic) const noexcept {
    const auto iterator = commands_.find(mnemonic);
    return iterator == commands_.end() ? nullptr : &iterator->second;
}

bool CommandRegistry::contains(char mnemonic) const noexcept {
    return find(mnemonic) != nullptr;
}

std::size_t CommandRegistry::size() const noexcept {
    return commands_.size();
}

CommandRegistry make_core_command_registry() {
    CommandRegistry registry;

    registry.register_command(CommandDescriptor{
        'P', "Print",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            return std::make_unique<PrintCommandNode>(
                std::move(address), location);
        }});

    registry.register_command(CommandDescriptor{
        'L', "List",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            if (address) {
                throw std::invalid_argument("L does not accept a line address");
            }
            return std::make_unique<ListCommandNode>(std::nullopt, location);
        }});

    registry.register_command(CommandDescriptor{
        'D', "Delete",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            return std::make_unique<DeleteCommandNode>(
                std::move(address), location);
        }});

    registry.register_command(CommandDescriptor{
        'A', "Append",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            if (address && address->kind() == AstNodeKind::RangeAddress) {
                throw std::invalid_argument("A accepts at most one line address");
            }
            return std::make_unique<AppendCommandNode>(
                std::move(address), location);
        }});

    registry.register_command(CommandDescriptor{
        'I', "Insert",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            if (address && address->kind() == AstNodeKind::RangeAddress) {
                throw std::invalid_argument("I accepts at most one line address");
            }
            return std::make_unique<InsertCommandNode>(
                std::move(address), location);
        }});

    return registry;
}

} // namespace fred
