#pragma once

#include "fred/ast/AddressNode.hpp"
#include "fred/ast/PatternNodes.hpp"

#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace fred {

class CommandNode : public AstNode {
public:
    CommandNode(std::unique_ptr<AddressNode> address,
                SourceLocation location) noexcept
        : address_(std::move(address)), location_(location) {}

    ~CommandNode() override = default;

    [[nodiscard]] const AddressNode* address() const noexcept { return address_.get(); }
    [[nodiscard]] bool has_address() const noexcept { return address_ != nullptr; }
    [[nodiscard]] SourceLocation location() const noexcept override { return location_; }

private:
    std::unique_ptr<AddressNode> address_;
    SourceLocation location_;
};

class PrintCommandNode final : public CommandNode {
public:
    using CommandNode::CommandNode;
    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::PrintCommand; }
};

// Historical FRED L command: list a file on the terminal.
class ListCommandNode final : public CommandNode {
public:
    ListCommandNode(std::optional<std::string> filename,
                    SourceLocation location) noexcept
        : CommandNode(nullptr, location), filename_(std::move(filename)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::ListCommand; }
    [[nodiscard]] const std::optional<std::string>& filename() const noexcept { return filename_; }

private:
    std::optional<std::string> filename_;
};

class DeleteCommandNode final : public CommandNode {
public:
    using CommandNode::CommandNode;
    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::DeleteCommand; }
};

class AppendCommandNode final : public CommandNode {
public:
    using CommandNode::CommandNode;
    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::AppendCommand; }
};

class InsertCommandNode final : public CommandNode {
public:
    using CommandNode::CommandNode;
    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::InsertCommand; }
};

class ChangeCommandNode final : public CommandNode {
public:
    using CommandNode::CommandNode;
    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::ChangeCommand; }
};

class MoveCommandNode final : public CommandNode {
public:
    MoveCommandNode(std::unique_ptr<AddressNode> address,
                    std::string buffer_name,
                    SourceLocation location) noexcept
        : CommandNode(std::move(address), location),
          buffer_name_(std::move(buffer_name)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::MoveCommand;
    }

    [[nodiscard]] const std::string& buffer_name() const noexcept {
        return buffer_name_;
    }

private:
    std::string buffer_name_;
};

class TransferCommandNode final : public CommandNode {
public:
    TransferCommandNode(std::unique_ptr<AddressNode> source,
                        std::unique_ptr<AddressNode> destination,
                        SourceLocation location) noexcept
        : CommandNode(std::move(source), location),
          destination_(std::move(destination)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::TransferCommand; }
    [[nodiscard]] const AddressNode* destination() const noexcept { return destination_.get(); }

private:
    std::unique_ptr<AddressNode> destination_;
};

class BufferCommandNode final : public CommandNode {
public:
    BufferCommandNode(std::string buffer_name,
                      SourceLocation location) noexcept
        : BufferCommandNode(std::move(buffer_name), false, location) {}

    BufferCommandNode(std::string buffer_name,
                      bool short_form,
                      SourceLocation location) noexcept
        : CommandNode(nullptr, location),
          buffer_name_(std::move(buffer_name)),
          short_form_(short_form) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::BufferCommand; }
    [[nodiscard]] const std::string& buffer_name() const noexcept { return buffer_name_; }
    [[nodiscard]] bool short_form() const noexcept { return short_form_; }

private:
    std::string buffer_name_;
    bool short_form_{};
};


class GlobalCommandNode final : public CommandNode {
public:
    GlobalCommandNode(std::unique_ptr<AddressNode> address,
                      std::unique_ptr<PatternNode> pattern,
                      bool inverted,
                      std::unique_ptr<CommandNode> nested_command,
                      SourceLocation location) noexcept
        : CommandNode(std::move(address), location),
          pattern_(std::move(pattern)),
          inverted_(inverted),
          nested_command_(std::move(nested_command)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::GlobalCommand;
    }
    [[nodiscard]] const PatternNode& pattern() const noexcept { return *pattern_; }
    [[nodiscard]] bool inverted() const noexcept { return inverted_; }
    [[nodiscard]] const CommandNode& nested_command() const noexcept {
        return *nested_command_;
    }

private:
    std::unique_ptr<PatternNode> pattern_;
    bool inverted_{};
    std::unique_ptr<CommandNode> nested_command_;
};

class ZapCommandNode final : public CommandNode {
public:
    using CommandNode::CommandNode;
    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::ZapCommand;
    }
};


class ZapGatherCommandNode final : public CommandNode {
public:
    ZapGatherCommandNode(std::string buffer_name,
                         std::unique_ptr<CommandNode> nested_command,
                         SourceLocation location) noexcept
        : CommandNode(nullptr, location),
          buffer_name_(std::move(buffer_name)),
          nested_command_(std::move(nested_command)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::ZapGatherCommand;
    }

    [[nodiscard]] const std::string& buffer_name() const noexcept {
        return buffer_name_;
    }

    [[nodiscard]] const CommandNode& nested_command() const noexcept {
        return *nested_command_;
    }

private:
    std::string buffer_name_;
    std::unique_ptr<CommandNode> nested_command_;
};

class SystemCommandNode final : public CommandNode {
public:
    SystemCommandNode(std::string command,
                      SourceLocation location) noexcept
        : CommandNode(nullptr, location), command_(std::move(command)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::SystemCommand;
    }

    [[nodiscard]] const std::string& command() const noexcept {
        return command_;
    }

private:
    std::string command_;
};

class SubstituteCommandNode final : public CommandNode {
public:
    SubstituteCommandNode(std::unique_ptr<AddressNode> address,
                          std::unique_ptr<PatternNode> pattern,
                          std::string replacement,
                          bool print_after,
                          SourceLocation location) noexcept
        : CommandNode(std::move(address), location),
          pattern_(std::move(pattern)),
          replacement_(std::move(replacement)),
          print_after_(print_after) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::SubstituteCommand;
    }
    [[nodiscard]] const PatternNode& pattern() const noexcept { return *pattern_; }
    [[nodiscard]] const std::string& replacement() const noexcept {
        return replacement_;
    }
    [[nodiscard]] bool print_after() const noexcept { return print_after_; }

private:
    std::unique_ptr<PatternNode> pattern_;
    std::string replacement_;
    bool print_after_{};
};

enum class FileWriteMode {
    Preserve,
    Ascii,
    Utf8,
    BcdUnsupported
};

class ReadCommandNode final : public CommandNode {
public:
    ReadCommandNode(std::unique_ptr<AddressNode> address,
                    std::optional<std::string> filename,
                    SourceLocation location) noexcept
        : CommandNode(std::move(address), location),
          filename_(std::move(filename)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::ReadCommand;
    }
    [[nodiscard]] const std::optional<std::string>& filename() const noexcept {
        return filename_;
    }

private:
    std::optional<std::string> filename_;
};

class WriteCommandNode final : public CommandNode {
public:
    WriteCommandNode(std::unique_ptr<AddressNode> address,
                     std::optional<std::string> filename,
                     FileWriteMode mode,
                     SourceLocation location) noexcept
        : CommandNode(std::move(address), location),
          filename_(std::move(filename)), mode_(mode) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::WriteCommand;
    }
    [[nodiscard]] const std::optional<std::string>& filename() const noexcept {
        return filename_;
    }
    [[nodiscard]] FileWriteMode mode() const noexcept { return mode_; }

private:
    std::optional<std::string> filename_;
    FileWriteMode mode_{FileWriteMode::Preserve};
};

class CommentCommandNode final : public CommandNode {
public:
    CommentCommandNode(std::string text,
                       SourceLocation location) noexcept
        : CommandNode(nullptr, location), text_(std::move(text)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::CommentCommand;
    }

    [[nodiscard]] const std::string& text() const noexcept {
        return text_;
    }

private:
    std::string text_;
};

class MessageCommandNode final : public CommandNode {
public:
    MessageCommandNode(std::string message,
                       bool newline,
                       SourceLocation location) noexcept
        : CommandNode(nullptr, location),
          message_(std::move(message)),
          newline_(newline) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::MessageCommand;
    }

    [[nodiscard]] const std::string& message() const noexcept {
        return message_;
    }

    [[nodiscard]] bool newline() const noexcept {
        return newline_;
    }

private:
    std::string message_;
    bool newline_{};
};

enum class FactsKind {
    Buffers,
    Options
};

class FactsCommandNode final : public CommandNode {
public:
    FactsCommandNode(FactsKind facts,
                     SourceLocation location) noexcept
        : CommandNode(nullptr, location), facts_(facts) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::FactsCommand;
    }

    [[nodiscard]] FactsKind facts() const noexcept { return facts_; }

private:
    FactsKind facts_;
};

enum class OptionKind {
    InputParenthesis,
    Monitor
};

class OptionCommandNode final : public CommandNode {
public:
    OptionCommandNode(OptionKind option,
                      bool enabled,
                      SourceLocation location) noexcept
        : CommandNode(nullptr, location), option_(option), enabled_(enabled) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::OptionCommand;
    }
    [[nodiscard]] OptionKind option() const noexcept { return option_; }
    [[nodiscard]] bool enabled() const noexcept { return enabled_; }

private:
    OptionKind option_;
    bool enabled_{};
};

class QuitCommandNode final : public CommandNode {
public:
    QuitCommandNode(bool immediate, SourceLocation location) noexcept
        : CommandNode(nullptr, location), immediate_(immediate) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::QuitCommand;
    }
    [[nodiscard]] bool immediate() const noexcept { return immediate_; }

private:
    bool immediate_{};
};

} // namespace fred
