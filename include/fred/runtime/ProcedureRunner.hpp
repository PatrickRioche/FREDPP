#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace fred {

class BufferManager;
class CommandExecutor;
class CommandRegistry;
class ExecutionContext;
struct ExpandedCommandInput;

/**
 * @brief Executes FRED procedures stored in Buffers or loaded from files.
 *
 * ProcedureRunner coordinates the already-existing Flow, Lexer, Parser and
 * CommandExecutor layers. It does not own any of those services.
 *
 * @par Ownership/lifetime
 * BufferManager, ExecutionContext, CommandRegistry and CommandExecutor are all
 * borrowed and must outlive the runner.
 *
 * @par Central flow rule
 * Non-comment procedure lines are expanded with
 * expand_command_input_with_metadata() before lexing/parsing. ProcedureRunner
 * therefore preserves the same transversal command-argument expansion rule as
 * interactive front ends.
 *
 * @par Procedure-specific syntax currently handled here
 * - `\B(buffer)` nested procedure-buffer execution;
 * - `@(label)` label definitions;
 * - `J(label)[T|F]` jumps;
 * - minimal `N(register)` numeric operations;
 * - A/I/C text blocks terminated by `\F`.
 */
class ProcedureRunner {
public:
    /**
     * @param buffers Borrowed BufferManager.
     * @param context Borrowed shared ExecutionContext.
     * @param registry Borrowed command registry.
     * @param executor Borrowed command executor.
     * @param maximum_depth Nested procedure/flow protection.
     *
     * @note The constructor is noexcept and currently does not reject zero;
     *       a zero maximum causes execute_buffer_impl() to fail immediately.
     */
    ProcedureRunner(BufferManager& buffers,
                    ExecutionContext& context,
                    const CommandRegistry& registry,
                    const CommandExecutor& executor,
                    std::size_t maximum_depth = 256) noexcept;

    /**
     * @brief Executes a named existing Buffer as a procedure.
     *
     * @throws CommandExecutionError for unknown buffer, depth overflow or
     *         procedure/runtime errors.
     */
    void execute_buffer(std::string_view buffer_name);

    /**
     * @brief Loads a procedure file into an empty temporary/specified Buffer and
     * executes it while restoring the previously selected Buffer.
     *
     * @param filename File read using normal R command semantics.
     * @param buffer_name Procedure Buffer name; default `"."`.
     *
     * @throws CommandExecutionError when the target procedure Buffer name is
     * empty or the Buffer is not empty/clean/unassociated.
     */
    void load_and_execute_file(const std::string& filename,
                               std::string buffer_name = ".");

    /**
     * @brief Recognizes and executes a standalone `\B(buffer)` procedure
     * directive.
     *
     * @return false when source is not a buffer-flow directive; true after
     * successful execution.
     * @throws CommandExecutionError for malformed directives/runtime failures.
     */
    bool execute_buffer_directive(std::string_view source);

private:
    void execute_buffer_impl(std::string_view buffer_name,
                             std::size_t depth);

    void execute_procedure_line(const std::vector<std::string>& lines,
                                std::size_t& index,
                                std::size_t depth);

    void execute_single_command(
        const ExpandedCommandInput& source,
        const std::vector<std::string>* lines,
        std::size_t* index);

    BufferManager* buffers_;
    ExecutionContext* context_;
    const CommandRegistry* registry_;
    const CommandExecutor* executor_;
    std::size_t maximum_depth_;
};

} // namespace fred
