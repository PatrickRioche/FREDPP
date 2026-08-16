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

class ProcedureRunner {
public:
    ProcedureRunner(BufferManager& buffers,
                    ExecutionContext& context,
                    const CommandRegistry& registry,
                    const CommandExecutor& executor,
                    std::size_t maximum_depth = 256) noexcept;

    void execute_buffer(std::string_view buffer_name);
    void load_and_execute_file(const std::string& filename,
                               std::string buffer_name = ".");

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
