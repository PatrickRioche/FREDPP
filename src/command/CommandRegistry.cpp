#include "fred/command/CommandRegistry.hpp"

#include <stdexcept>
#include <utility>

namespace fred {

void CommandRegistry::register_command(CommandDescriptor descriptor) {
    // Keep registry integrity checks at the registration boundary so parser
    // lookup can remain simple and noexcept.
    if (descriptor.mnemonic == '\0') {
        throw std::invalid_argument("command mnemonic may not be NUL");
    }
    if (descriptor.name.empty()) {
        throw std::invalid_argument("command name may not be empty");
    }
    if (!descriptor.parse) {
        throw std::invalid_argument("command parser may not be empty");
    }

    // Registration is exact and case-sensitive. The core table uses uppercase
    // keys; source normalization is performed by CommandParser, not here.
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

    // For simple commands, descriptor callbacks construct the complete AST.
    // For commands with additional operands, CommandParser normally intercepts
    // the mnemonic and performs the full operand parse before the generic
    // descriptor fallback is reached. Consequently, several callbacks below
    // intentionally contain only a minimal/default construction and are not a
    // standalone source-parsing API.

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
            // CommandParser handles the optional filename before constructing
            // the real ListCommandNode. This callback represents the no-file
            // default form only.
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

    registry.register_command(CommandDescriptor{
        'B', "Buffer",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            if (address) {
                throw std::invalid_argument("B does not accept a line address");
            }
            // Placeholder/default only. CommandParser parses the real buffer
            // name and constructs BufferCommandNode directly.
            return std::make_unique<BufferCommandNode>("", location);
        }});

    registry.register_command(CommandDescriptor{
        'C', "Change",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            return std::make_unique<ChangeCommandNode>(
                std::move(address), location);
        }});

    registry.register_command(CommandDescriptor{
        'M', "Move",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            // Placeholder/default only; CommandParser supplies the destination
            // buffer for real M source syntax.
            return std::make_unique<MoveCommandNode>(
                std::move(address), std::string{}, location);
        }});

    registry.register_command(CommandDescriptor{
        'T', "Transfer",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            // Placeholder/default only; a real T command requires a parsed
            // destination address and is constructed by CommandParser.
            return std::make_unique<TransferCommandNode>(
                std::move(address), nullptr, location);
        }});

    registry.register_command(CommandDescriptor{
        'G', "Global",
        [](std::unique_ptr<AddressNode>, SourceLocation location) {
            // Registry placeholder. A usable GlobalCommandNode requires a
            // pattern and nested command, both parsed by CommandParser.
            return std::make_unique<GlobalCommandNode>(
                nullptr, nullptr, false, nullptr, location);
        }});

    registry.register_command(CommandDescriptor{
        'Z', "Zap",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            // CommandParser applies the current "at most one address" rule
            // before constructing normal Z syntax.
            return std::make_unique<ZapCommandNode>(
                std::move(address), location);
        }});

    registry.register_command(CommandDescriptor{
        'R', "Read",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            // Represents the no-filename default. CommandParser handles an
            // optional filename and the single-address insertion restriction.
            return std::make_unique<ReadCommandNode>(
                std::move(address), std::nullopt, location);
        }});

    registry.register_command(CommandDescriptor{
        'W', "Write",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            // Represents plain W with default mode/no filename. CommandParser
            // handles WA/WU/WB and optional filenames.
            return std::make_unique<WriteCommandNode>(
                std::move(address), std::nullopt, FileWriteMode::Preserve,
                location);
        }});

    registry.register_command(CommandDescriptor{
        'S', "Substitute",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            // Registry placeholder. A usable SubstituteCommandNode requires a
            // parsed pattern; CommandParser constructs it for real S syntax.
            return std::make_unique<SubstituteCommandNode>(
                std::move(address), nullptr, std::string{}, false, location);
        }});

    registry.register_command(CommandDescriptor{
        'J', "Jump",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            if (address) {
                throw std::invalid_argument("J does not accept a line address");
            }
            // Historical registry naming and current J operand handling have
            // evolved independently. CommandParser currently constructs the
            // implemented JM/JP MessageCommandNode forms itself.
            return std::make_unique<MessageCommandNode>(
                std::string{}, true, location);
        }});

    registry.register_command(CommandDescriptor{
        'F', "Facts",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            if (address) {
                throw std::invalid_argument("F does not accept a line address");
            }
            // Default placeholder; CommandParser requires and distinguishes
            // the implemented FB and FO source forms.
            return std::make_unique<FactsCommandNode>(
                FactsKind::Buffers, location);
        }});

    registry.register_command(CommandDescriptor{
        'O', "Option",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            if (address) {
                throw std::invalid_argument("O does not accept a line address");
            }
            // Default placeholder; CommandParser parses the implemented option
            // selector and +/- state before constructing the real node.
            return std::make_unique<OptionCommandNode>(
                OptionKind::InputParenthesis, true, location);
        }});

    registry.register_command(CommandDescriptor{
        'Q', "Quit",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            if (address) {
                throw std::invalid_argument("Q does not accept a line address");
            }
            // Plain Q default. CommandParser additionally recognizes QQ and
            // rejects Q! until external TSS command support exists.
            return std::make_unique<QuitCommandNode>(false, location);
        }});

    return registry;
}

} // namespace fred
