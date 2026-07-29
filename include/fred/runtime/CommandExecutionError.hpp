#pragma once

#include <stdexcept>
#include <string>

namespace fred {

class CommandExecutionError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class CommandNotImplementedError final : public CommandExecutionError {
public:
    explicit CommandNotImplementedError(std::string command_name)
        : CommandExecutionError("command not implemented: " + command_name),
          command_name_(std::move(command_name)) {}

    [[nodiscard]] const std::string& command_name() const noexcept {
        return command_name_;
    }

private:
    std::string command_name_;
};

} // namespace fred
