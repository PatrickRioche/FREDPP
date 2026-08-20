#pragma once

#include "fred/ast/CommandNode.hpp"
#include "fred/runtime/ExecutionContext.hpp"

#include <string>
#include <vector>

namespace fred {

/**
 * @brief Executes already-parsed command AST nodes against ExecutionContext.
 *
 * CommandExecutor is the runtime dispatch boundary. It never tokenizes or
 * parses source text. Buffer mutations, file I/O, output, condition/counter
 * updates and exit requests happen here through ExecutionContext/services.
 *
 * @par Text-input commands
 * A/I/C require extra procedure/REPL text lines and are therefore deliberately
 * rejected by execute(). Callers must collect text input then call
 * execute_append(), execute_insert() or execute_change().
 *
 * @par Errors
 * Runtime semantic/I/O failures are reported as CommandExecutionError (or an
 * exception translated into one by the relevant helper).
 */
class CommandExecutor {
public:
    /**
     * @brief Dispatches one non-text-input command AST.
     *
     * @param command Parsed command node.
     * @param context Mutable runtime state/services.
     *
     * @throws CommandExecutionError for unsupported nodes, invalid runtime
     *         addresses/state, I/O failures, unimplemented forms, or when A/I/C
     *         are passed without text-input handling.
     */
    void execute(const CommandNode& command,
                 ExecutionContext& context) const;

    /**
     * @brief Executes A after its text lines have been collected.
     *
     * @param lines Lines inserted after the evaluated/default append position;
     *        ownership is transferred by value.
     *
     * @note Default A position is historical `$`. Absolute address 0 is
     *       explicitly accepted as insertion before the first line.
     */
    void execute_append(const AppendCommandNode& command,
                        ExecutionContext& context,
                        std::vector<std::string> lines) const;

    /**
     * @brief Executes I after text collection.
     *
     * Default insertion is before the current line; on an empty Buffer the
     * insertion position is 0. Absolute 0 is also accepted explicitly.
     */
    void execute_insert(const InsertCommandNode& command,
                        ExecutionContext& context,
                        std::vector<std::string> lines) const;

    /**
     * @brief Replaces the evaluated C range with collected text lines.
     *
     * The original inclusive range is erased first. If `lines` is empty the
     * change acts as deletion; otherwise replacement lines are inserted where
     * the erased range began.
     */
    void execute_change(const ChangeCommandNode& command,
                        ExecutionContext& context,
                        std::vector<std::string> lines) const;
};

} // namespace fred
