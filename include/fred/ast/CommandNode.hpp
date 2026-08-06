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
    MoveCommandNode(std::unique_ptr<AddressNode> source,
                    std::unique_ptr<AddressNode> destination,
                    SourceLocation location) noexcept
        : CommandNode(std::move(source), location),
          destination_(std::move(destination)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::MoveCommand; }
    [[nodiscard]] const AddressNode* destination() const noexcept { return destination_.get(); }

private:
    std::unique_ptr<AddressNode> destination_;
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
        : CommandNode(nullptr, location),
          buffer_name_(std::move(buffer_name)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::BufferCommand; }
    [[nodiscard]] const std::string& buffer_name() const noexcept { return buffer_name_; }

private:
    std::string buffer_name_;
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
