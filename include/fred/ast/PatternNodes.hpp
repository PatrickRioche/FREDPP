#pragma once

#include "fred/ast/AstNode.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace fred {

class PatternNode : public AstNode {
public:
    explicit PatternNode(SourceLocation location) noexcept : location_(location) {}
    [[nodiscard]] SourceLocation location() const noexcept override { return location_; }

private:
    SourceLocation location_;
};

class LiteralPatternNode final : public PatternNode {
public:
    LiteralPatternNode(char value, SourceLocation location) noexcept
        : PatternNode(location), value_(value) {}
    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::PatternLiteral; }
    [[nodiscard]] char value() const noexcept { return value_; }
private:
    char value_;
};

class AnyCharacterPatternNode final : public PatternNode {
public:
    using PatternNode::PatternNode;
    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::PatternAnyCharacter; }
};

class AnchorPatternNode final : public PatternNode {
public:
    enum class Type { StartOfLine, EndOfLine };
    AnchorPatternNode(Type type, SourceLocation location) noexcept
        : PatternNode(location), type_(type) {}
    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::PatternAnchor; }
    [[nodiscard]] Type type() const noexcept { return type_; }
private:
    Type type_;
};

class SequencePatternNode final : public PatternNode {
public:
    SequencePatternNode(std::vector<std::unique_ptr<PatternNode>> elements,
                        SourceLocation location)
        : PatternNode(location), elements_(std::move(elements)) {}
    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::PatternSequence; }
    [[nodiscard]] const auto& elements() const noexcept { return elements_; }
private:
    std::vector<std::unique_ptr<PatternNode>> elements_;
};

class AlternationPatternNode final : public PatternNode {
public:
    AlternationPatternNode(std::vector<std::unique_ptr<PatternNode>> alternatives,
                           SourceLocation location)
        : PatternNode(location), alternatives_(std::move(alternatives)) {}
    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::PatternAlternation; }
    [[nodiscard]] const auto& alternatives() const noexcept { return alternatives_; }
private:
    std::vector<std::unique_ptr<PatternNode>> alternatives_;
};

class RepetitionPatternNode final : public PatternNode {
public:
    enum class Type { ZeroOrMore, OneOrMore };
    RepetitionPatternNode(std::unique_ptr<PatternNode> operand,
                          Type type,
                          SourceLocation location)
        : PatternNode(location), operand_(std::move(operand)), type_(type) {}
    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::PatternRepetition; }
    [[nodiscard]] const PatternNode& operand() const noexcept { return *operand_; }
    [[nodiscard]] Type type() const noexcept { return type_; }
private:
    std::unique_ptr<PatternNode> operand_;
    Type type_;
};

struct CharacterClassRange {
    char first{};
    char last{};
};

class CharacterClassPatternNode final : public PatternNode {
public:
    CharacterClassPatternNode(bool negated,
                              std::vector<CharacterClassRange> ranges,
                              SourceLocation location)
        : PatternNode(location), negated_(negated), ranges_(std::move(ranges)) {}
    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::PatternCharacterClass; }
    [[nodiscard]] bool negated() const noexcept { return negated_; }
    [[nodiscard]] const auto& ranges() const noexcept { return ranges_; }
private:
    bool negated_{};
    std::vector<CharacterClassRange> ranges_;
};

class GroupPatternNode final : public PatternNode {
public:
    GroupPatternNode(std::unique_ptr<PatternNode> expression, SourceLocation location)
        : PatternNode(location), expression_(std::move(expression)) {}
    [[nodiscard]] AstNodeKind kind() const noexcept override { return AstNodeKind::PatternGroup; }
    [[nodiscard]] const PatternNode& expression() const noexcept { return *expression_; }
private:
    std::unique_ptr<PatternNode> expression_;
};

} // namespace fred
