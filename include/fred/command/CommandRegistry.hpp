#pragma once

#include "fred/command/CommandDescriptor.hpp"

#include <optional>
#include <unordered_map>

namespace fred {

/**
 * @brief Registry mapping command mnemonics to CommandDescriptor objects.
 *
 * CommandRegistry is the command-discovery boundary used by CommandParser.
 * Registration validates descriptor integrity and prevents duplicate keys.
 *
 * @par Key semantics
 * Keys are exact `char` values and lookup is case-sensitive. No normalization
 * is performed by this class. The parser uppercases command letters before
 * consulting the core registry.
 *
 * @par Ownership
 * The registry owns copies/moved instances of its CommandDescriptor values.
 * Descriptor callbacks and names therefore live as long as their registry
 * entries.
 *
 * @par Threading
 * No synchronization is provided. Construct/register the registry before
 * sharing it for concurrent read-only use; do not mutate it concurrently with
 * lookups.
 *
 * @par Architectural boundary
 * The registry contains command metadata and construction callbacks only. It
 * does not tokenize input, parse command operands, access Buffer state, or
 * execute commands.
 */
class CommandRegistry {
public:
    /**
     * @brief Adds one descriptor to the registry.
     *
     * @param descriptor Descriptor copied/moved into registry storage.
     *
     * @throws std::invalid_argument if:
     * - `descriptor.mnemonic == '\0'`;
     * - `descriptor.name` is empty;
     * - `descriptor.parse` is empty;
     * - the exact mnemonic is already registered.
     *
     * @post On success size() increases by one.
     * @post Existing descriptors are not replaced.
     *
     * @note Mnemonic case is preserved exactly as supplied.
     */
    void register_command(CommandDescriptor descriptor);

    /**
     * @brief Finds a descriptor by exact mnemonic.
     *
     * @param mnemonic Exact lookup key.
     * @return Non-owning pointer to the stored descriptor, or nullptr when the
     *         key is absent.
     *
     * @note The pointer is tied to the lifetime of this registry. The class
     *       exposes no erase operation. Callers must not delete or modify the
     *       returned descriptor through this const pointer.
     */
    [[nodiscard]] const CommandDescriptor* find(char mnemonic) const noexcept;

    /**
     * @brief Tests whether an exact mnemonic is registered.
     *
     * @param mnemonic Exact lookup key.
     * @return true when find(mnemonic) is non-null.
     */
    [[nodiscard]] bool contains(char mnemonic) const noexcept;

    /** @return Number of registered descriptors. */
    [[nodiscard]] std::size_t size() const noexcept;

private:
    /** Owned descriptors indexed by their exact mnemonic byte. */
    std::unordered_map<char, CommandDescriptor> commands_;
};

/**
 * @brief Builds the standard FREDPP command registry.
 *
 * @return Registry containing the current core command mnemonics:
 *         P, L, D, A, I, B, C, M, T, G, Z, R, W, S, J, F, O and Q.
 *
 * @par Current implementation
 * The returned registry currently contains 18 entries. Simple-command
 * descriptors can directly create their complete AST node. Operand-heavy
 * commands are normally fully handled by CommandParser; their descriptor
 * callbacks may contain only defaults/placeholders required by the common
 * registry interface.
 *
 * @warning Do not invoke arbitrary core descriptor callbacks as a substitute
 *          for parsing source text. Use CommandParser for real command input.
 */
[[nodiscard]] CommandRegistry make_core_command_registry();

} // namespace fred
