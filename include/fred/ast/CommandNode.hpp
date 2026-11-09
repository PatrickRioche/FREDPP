#pragma once

#include "fred/ast/AddressNode.hpp"
#include "fred/ast/PatternNodes.hpp"

#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace fred {

/**
 * @brief Base class for parsed FRED commands.
 *
 * CommandNode optionally owns the line-address expression written before the
 * command and records the location of the command itself.
 *
 * @par Ownership
 * `address` is transferred into the node through std::unique_ptr. address()
 * returns a non-owning pointer and may return nullptr.
 *
 * @par Architectural boundary
 * Command nodes describe syntax and command operands only. They do not execute
 * the command and must not directly mutate Buffer/Runtime state.
 */
class CommandNode : public AstNode {
public:
    /**
     * @param address Optional parsed address; ownership is transferred.
     * @param location Source location of the command mnemonic/symbol.
     */
    CommandNode(std::unique_ptr<AddressNode> address,
                SourceLocation location) noexcept
        : address_(std::move(address)), location_(location) {}

    ~CommandNode() override = default;

    /** @return Non-owning address pointer, or nullptr when no address was supplied. */
    [[nodiscard]] const AddressNode* address() const noexcept {
        return address_.get();
    }

    /** @return true when this command owns an explicit address AST. */
    [[nodiscard]] bool has_address() const noexcept {
        return address_ != nullptr;
    }

    [[nodiscard]] SourceLocation location() const noexcept override {
        return location_;
    }

private:
    std::unique_ptr<AddressNode> address_;
    SourceLocation location_;
};

/** @brief `P` command AST. */
class PrintCommandNode final : public CommandNode {
public:
    using CommandNode::CommandNode;
    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::PrintCommand;
    }
};

/**
 * @brief Historical `=` command AST: print a line number without moving
 * the current cursor.
 *
 * An optional single-line address is owned by the base class; no address means
 * the current line, which is 0 for an empty buffer.
 */
class LineNumberCommandNode final : public CommandNode {
public:
    using CommandNode::CommandNode;
    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::LineNumberCommand;
    }
};

/**
 * @brief Historical `L` command AST: list a file on the terminal.
 *
 * Unlike ordinary addressed commands, ListCommandNode is constructed without a
 * line address and optionally owns a filename.
 */
class ListCommandNode final : public CommandNode {
public:
    /**
     * @param filename Optional parsed filename.
     * @param location Location of the L command.
     */
    ListCommandNode(std::optional<std::string> filename,
                    SourceLocation location) noexcept
        : CommandNode(nullptr, location), filename_(std::move(filename)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::ListCommand;
    }

    /** @return Optional filename owned by this node. */
    [[nodiscard]] const std::optional<std::string>& filename() const noexcept {
        return filename_;
    }

private:
    std::optional<std::string> filename_;
};

/** @brief `D` command AST. */
class DeleteCommandNode final : public CommandNode {
public:
    using CommandNode::CommandNode;
    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::DeleteCommand;
    }
};

/** @brief `A` command AST. */
class AppendCommandNode final : public CommandNode {
public:
    using CommandNode::CommandNode;
    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::AppendCommand;
    }
};

/** @brief `I` command AST. */
class InsertCommandNode final : public CommandNode {
public:
    using CommandNode::CommandNode;
    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::InsertCommand;
    }
};

/** @brief `C` command AST. */
class ChangeCommandNode final : public CommandNode {
public:
    using CommandNode::CommandNode;
    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::ChangeCommand;
    }
};

/** @brief `M(buffer)` command AST. */
class MoveCommandNode final : public CommandNode {
public:
    /**
     * @param address Optional source address; ownership is transferred.
     * @param buffer_name Destination buffer name owned by the node.
     * @param location Location of the M command.
     */
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

/**
 * @brief Historical K command: copy addressed lines into a named Buffer.
 *
 * The destination's previous content is replaced. When source and destination
 * are different Buffers, source text is preserved. When they are the same
 * Buffer, the selected lines replace that Buffer's entire previous content.
 * This node owns syntax only; Runtime performs the copy.
 */
class CopyCommandNode final : public CommandNode {
public:
    /**
     * @param address Optional source address; defaults to the current line.
     * @param buffer_name Destination Buffer name owned by this node.
     * @param location Location of the K command.
     */
    CopyCommandNode(std::unique_ptr<AddressNode> address,
                    std::string buffer_name,
                    SourceLocation location) noexcept
        : CommandNode(std::move(address), location),
          buffer_name_(std::move(buffer_name)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::CopyCommand;
    }

    [[nodiscard]] const std::string& buffer_name() const noexcept {
        return buffer_name_;
    }

private:
    std::string buffer_name_;
};

/**
 * @brief Historical `T` command AST for testing a pattern over addressed lines.
 *
 * T is read-only: it updates the shared runtime condition flag, not the Buffer.
 */
class TestCommandNode final : public CommandNode {
public:
    /**
     * @param address Optional test range; defaults to the current line.
     * @param pattern Pattern to search for; ownership is transferred.
     * @param inverted true for `T~`, which negates the range-wide result.
     * @param location Location of the T command.
     * @pre `pattern` must be non-null for execution.
     */
    TestCommandNode(std::unique_ptr<AddressNode> address,
                    std::unique_ptr<PatternNode> pattern,
                    bool inverted,
                    SourceLocation location) noexcept
        : CommandNode(std::move(address), location),
          pattern_(std::move(pattern)), inverted_(inverted) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::TestCommand;
    }

    /** @return Borrowed pattern reference; valid for the AST node's lifetime. */
    [[nodiscard]] const PatternNode& pattern() const noexcept {
        return *pattern_;
    }

    /** @return Whether the `~` condition inversion was requested. */
    [[nodiscard]] bool inverted() const noexcept { return inverted_; }

private:
    std::unique_ptr<PatternNode> pattern_;
    bool inverted_{};
};

/**
 * @brief Selects or creates a named Buffer through historical B syntax.
 *
 * The parser records whether a compact one-character name was used. Runtime
 * rejects that short form while the O+I( option requires parenthesized names;
 * this AST does not itself change the selected Buffer.
 */
class BufferCommandNode final : public CommandNode {
public:
    /**
     * @brief Constructs the normal/long form.
     *
     * @param buffer_name Parsed buffer name.
     * @param location Location of the B command.
     */
    BufferCommandNode(std::string buffer_name,
                      SourceLocation location) noexcept
        : BufferCommandNode(std::move(buffer_name), false, location) {}

    /**
     * @param buffer_name Parsed buffer name.
     * @param short_form true when compact historical syntax was used.
     * @param location Location of the B command.
     */
    BufferCommandNode(std::string buffer_name,
                      bool short_form,
                      SourceLocation location) noexcept
        : CommandNode(nullptr, location),
          buffer_name_(std::move(buffer_name)),
          short_form_(short_form) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::BufferCommand;
    }

    [[nodiscard]] const std::string& buffer_name() const noexcept {
        return buffer_name_;
    }

    /** @return true for compact one-character buffer syntax. */
    [[nodiscard]] bool short_form() const noexcept { return short_form_; }

private:
    std::string buffer_name_;
    bool short_form_{};
};

/**
 * @brief `G` global-selection command AST.
 *
 * The node owns the selection pattern and the nested command. Current parser
 * behavior rejects nested commands that themselves carry an explicit address.
 */
class GlobalCommandNode final : public CommandNode {
public:
    /**
     * @param address Optional outer selection address; ownership transferred.
     * @param pattern Selection pattern; ownership transferred.
     * @param inverted true for `G~`.
     * @param nested_command Command executed for matching/non-matching lines.
     * @param location Location of G.
     *
     * @pre `pattern` and `nested_command` must be non-null. Accessors
     *      dereference them without checks.
     */
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

    [[nodiscard]] const PatternNode& pattern() const noexcept {
        return *pattern_;
    }

    [[nodiscard]] bool inverted() const noexcept { return inverted_; }

    [[nodiscard]] const CommandNode& nested_command() const noexcept {
        return *nested_command_;
    }

private:
    std::unique_ptr<PatternNode> pattern_;
    bool inverted_{};
    std::unique_ptr<CommandNode> nested_command_;
};

/**
 * @brief Selects the addressed line as the current cursor position.
 *
 * Z accepts at most one line address and does not modify the Buffer's text.
 * An absent address resolves to the current line.
 */
class ZapCommandNode final : public CommandNode {
public:
    using CommandNode::CommandNode;
    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::ZapCommand;
    }
};

/**
 * @brief `ZK(buffer)` AST for copying lines after a destination cursor.
 *
 * Owns the optional source range and destination Buffer name. Runtime must
 * snapshot source text before inserting, particularly for same-Buffer copies.
 */
class ZapCopyCommandNode final : public CommandNode {
public:
    ZapCopyCommandNode(std::unique_ptr<AddressNode> address,
                       std::string buffer_name,
                       SourceLocation location) noexcept
        : CommandNode(std::move(address), location),
          buffer_name_(std::move(buffer_name)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::ZapCopyCommand;
    }

    [[nodiscard]] const std::string& buffer_name() const noexcept {
        return buffer_name_;
    }

private:
    std::string buffer_name_;
};

/**
 * @brief `ZG(buffer) command` AST.
 *
 * Owns the destination buffer name and nested command.
 */
class ZapGatherCommandNode final : public CommandNode {
public:
    /**
     * @param buffer_name Destination buffer.
     * @param nested_command Nested command; ownership transferred.
     * @param location Location of ZG.
     *
     * @pre `nested_command` must be non-null.
     */
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

/** @brief `!` system-command AST containing reconstructed command text. */
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

/**
 * @brief Holds the pattern, replacement and optional print suffix for S.
 *
 * Runtime substitutes all matches on each addressed line. An unescaped `&`
 * in the replacement reproduces the matched text; a backslash quotes the
 * following character. A command with no pattern match reports an error.
 */
class SubstituteCommandNode final : public CommandNode {
public:
    /**
     * @param address Optional target address; ownership transferred.
     * @param pattern Search pattern; ownership transferred.
     * @param replacement Replacement text owned by the node.
     * @param print_after true when trailing `P` was parsed.
     * @param location Location of S.
     *
     * @pre `pattern` must be non-null.
     */
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

    [[nodiscard]] const PatternNode& pattern() const noexcept {
        return *pattern_;
    }

    [[nodiscard]] const std::string& replacement() const noexcept {
        return replacement_;
    }

    [[nodiscard]] bool print_after() const noexcept { return print_after_; }

private:
    std::unique_ptr<PatternNode> pattern_;
    std::string replacement_;
    bool print_after_{};
};

/**
 * @brief Parsed output mode for the W command family.
 */
enum class FileWriteMode {
    /** Preserve/default mode. */
    Preserve,

    /** WA: ASCII output mode. */
    Ascii,

    /** WU: UTF-8 output mode. */
    Utf8,

    /** WB: historical BCD form recognized syntactically but unsupported. */
    BcdUnsupported
};

/** @brief `R` read-command AST. */
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

/** @brief `W`, `WA`, `WU` or syntactically recognized `WB` command AST. */
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

/** @brief `"` comment-command AST containing the remaining command-line text. */
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

/** @brief `JM`/`JP` message-command AST. */
class MessageCommandNode final : public CommandNode {
public:
    /**
     * @param message Message text.
     * @param newline true for JM, false for JP.
     * @param location Location of the J command form.
     */
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

/** @brief Selects one of the implemented read-only F information queries. */
enum class FactsKind {
    /** FB: recently selected Buffers in MRU order, including cursor/count. */
    Buffers,

    /** FF: only file-associated Buffers, sorted by Buffer name. */
    Files,

    /** FO: currently effective input-parenthesis and monitor options. */
    Options
};

/**
 * @brief `FB`, `FF` or `FO` query without a line address or Buffer mutation.
 *
 * The selected query determines the output format and ordering; the AST only
 * records the query kind, not a snapshot of Runtime state.
 */
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

/** @brief Option selector currently recognized by CommandParser. */
enum class OptionKind {
    /** O+I( requires parentheses in B names; O-I( permits compact names. */
    InputParenthesis,

    /** O+M traces procedure commands; O-M disables that monitoring. */
    Monitor
};

/**
 * @brief Changes one supported O option in the shared ExecutionContext.
 *
 * The `+` source spelling enables an option and `-` disables it. O does not
 * accept a line address or directly edit Buffer text. FO reports the resulting
 * option state; monitoring is applied by the procedure runner.
 */
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

/** @brief `Q` / `QQ` quit-command AST. */
class QuitCommandNode final : public CommandNode {
public:
    /**
     * @param immediate false for Q, true for compact immediate form QQ.
     * @param location Location of Q/QQ.
     */
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
