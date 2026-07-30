#pragma once

#include "fred/ast/AddressNode.hpp"

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

} // namespace fred
