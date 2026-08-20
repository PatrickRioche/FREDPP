#pragma once

#include "fred/ast/CommandNode.hpp"

#include <functional>
#include <memory>
#include <string>

namespace fred {

/**
 * @brief Callback type stored by a CommandDescriptor.
 *
 * The callback receives ownership of an optional parsed line-address AST and
 * the source location of the command mnemonic. It returns ownership of the
 * constructed CommandNode.
 *
 * @param address Optional address prefix. Ownership is transferred to the
 *        callback.
 * @param location Source location of the command mnemonic.
 * @return Exclusively owned command AST.
 *
 * @warning In the current architecture this callback is not a complete
 *          command parser for every registered mnemonic. Commands with operands
 *          are normally intercepted and fully parsed by CommandParser before
 *          the descriptor callback is used. Some core callbacks therefore
 *          construct only a minimal/default node and must not be treated as a
 *          standalone replacement for CommandParser.
 */
using CommandParseFunction = std::function<std::unique_ptr<CommandNode>(
    std::unique_ptr<AddressNode>, SourceLocation)>;

/**
 * @brief One command-table entry used by CommandRegistry.
 *
 * A descriptor associates an exact one-byte lookup key with a descriptive name
 * and an AST construction callback.
 *
 * @par Validation
 * CommandRegistry::register_command() rejects:
 * - a NUL mnemonic;
 * - an empty name;
 * - an empty callback;
 * - a mnemonic already present in the same registry.
 *
 * @par Case handling
 * CommandRegistry itself performs exact, case-sensitive char lookup. The core
 * registry uses uppercase mnemonics. CommandParser is responsible for
 * normalizing source command letters before registry lookup.
 */
struct CommandDescriptor {
    /** Exact registry key. The core registry uses uppercase ASCII letters. */
    char mnemonic{};

    /** Human-readable command name used for metadata/debugging. */
    std::string name;

    /**
     * AST construction callback.
     *
     * For simple commands this can construct the complete node. For commands
     * with operands, CommandParser may parse and construct the node itself
     * instead of invoking this callback.
     */
    CommandParseFunction parse;
};

} // namespace fred
