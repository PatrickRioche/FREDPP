#pragma once

#include <stdexcept>
#include <string>

namespace fred {

/**
 * @brief Base exception for failures detected while executing parsed commands.
 *
 * Parser syntax failures use ParseError; runtime/editor/semantic failures use
 * CommandExecutionError or a derived type.
 */
class CommandExecutionError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/**
 * @brief Specialized runtime error for a recognized but unimplemented command.
 *
 * @note Most current core commands now have execution support. This type remains
 * available as the explicit classification for future recognized commands whose
 * runtime behavior is not yet implemented.
 */
class CommandNotImplementedError final : public CommandExecutionError {
public:
    /**
     * @param command_name Human-readable command name retained by this object.
     */
    explicit CommandNotImplementedError(std::string command_name)
        : CommandExecutionError("command not implemented: " + command_name),
          command_name_(std::move(command_name)) {}

    /** @return Stored command name without the error-message prefix. */
    [[nodiscard]] const std::string& command_name() const noexcept {
        return command_name_;
    }

private:
    std::string command_name_;
};

} // namespace fred
