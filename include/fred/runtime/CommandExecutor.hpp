#pragma once

#include "fred/ast/CommandNode.hpp"
#include "fred/runtime/ExecutionContext.hpp"

#include <string>
#include <vector>

namespace fred {

class CommandExecutor {
public:
    void execute(const CommandNode& command, ExecutionContext& context) const;
    void execute_append(const AppendCommandNode& command,
                        ExecutionContext& context,
                        std::vector<std::string> lines) const;
    void execute_insert(const InsertCommandNode& command,
                        ExecutionContext& context,
                        std::vector<std::string> lines) const;
    void execute_change(const ChangeCommandNode& command,
                        ExecutionContext& context,
                        std::vector<std::string> lines) const;
};

} // namespace fred
