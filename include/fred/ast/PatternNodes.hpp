#pragma once

#include "fred/ast/AstNode.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace fred {

/**
 * @brief Abstract base for FRED pattern-language AST nodes.
 *
 * PatternNode stores the source location shared by all concrete pattern nodes.
 * Pattern execution is not performed by these classes; PatternMatcher consumes
 * the tree later.
 */
class PatternNode : public AstNode {
public:
    /** @param location Start location of the represented pattern construct. */
    explicit PatternNode(SourceLocation location) noexcept : location_(location) {}

    [[nodiscard]] SourceLocation location() const noexcept override {
        return location_;
    }

private:
    SourceLocation location_;
};

/** @brief One literal character in a FRED pattern. */
class LiteralPatternNode final : public PatternNode {
public:
    /**
     * @param value Literal byte represented by this node.
     * @param location Location of the character in the parsed pattern source.
     */
    LiteralPatternNode(char value, SourceLocation location) noexcept
        : PatternNode(location), value_(value) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::PatternLiteral;
    }

    /** @return Literal byte. */
    [[nodiscard]] char value() const noexcept { return value_; }

private:
    char value_;
};

/** @brief FRED `.` atom: any single character according to matcher semantics. */
class AnyCharacterPatternNode final : public PatternNode {
public:
    using PatternNode::PatternNode;

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::PatternAnyCharacter;
    }
};

/** @brief Start/end-of-line anchor (`^` or `$`). */
class AnchorPatternNode final : public PatternNode {
public:
    enum class Type {
        StartOfLine,
        EndOfLine
    };

    /**
     * @param type Anchor semantic represented by this node.
     * @param location Location of `^` or `$`.
     */
    AnchorPatternNode(Type type, SourceLocation location) noexcept
        : PatternNode(location), type_(type) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::PatternAnchor;
    }

    /** @return StartOfLine or EndOfLine. */
    [[nodiscard]] Type type() const noexcept { return type_; }

private:
    Type type_;
};

/**
 * @brief Ordered concatenation of zero or more pattern nodes.
 *
 * The node owns all elements through std::unique_ptr. An empty sequence is a
 * valid internal representation produced by PatternParser for an empty
 * sequence position; higher grammar rules decide where it is acceptable.
 */
class SequencePatternNode final : public PatternNode {
public:
    /**
     * @param elements Child nodes; ownership is transferred.
     * @param location Start location of the sequence.
     */
    SequencePatternNode(std::vector<std::unique_ptr<PatternNode>> elements,
                        SourceLocation location)
        : PatternNode(location), elements_(std::move(elements)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::PatternSequence;
    }

    /** @return Non-owning const reference to the owned child vector. */
    [[nodiscard]] const auto& elements() const noexcept { return elements_; }

private:
    std::vector<std::unique_ptr<PatternNode>> elements_;
};

/** @brief Ordered alternatives separated by `|`. */
class AlternationPatternNode final : public PatternNode {
public:
    /**
     * @param alternatives Alternative branches; ownership is transferred.
     * @param location Start location of the alternation.
     */
    AlternationPatternNode(
        std::vector<std::unique_ptr<PatternNode>> alternatives,
        SourceLocation location)
        : PatternNode(location), alternatives_(std::move(alternatives)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::PatternAlternation;
    }

    /** @return Non-owning const reference to the owned alternatives. */
    [[nodiscard]] const auto& alternatives() const noexcept {
        return alternatives_;
    }

private:
    std::vector<std::unique_ptr<PatternNode>> alternatives_;
};

/** @brief One pattern operand followed by `*` or `+`. */
class RepetitionPatternNode final : public PatternNode {
public:
    enum class Type {
        ZeroOrMore,
        OneOrMore
    };

    /**
     * @param operand Repeated expression; ownership is transferred.
     * @param type `*` (ZeroOrMore) or `+` (OneOrMore).
     * @param location Start location of the repeated atom/expression.
     *
     * @pre `operand` must be non-null; operand() dereferences it.
     */
    RepetitionPatternNode(std::unique_ptr<PatternNode> operand,
                          Type type,
                          SourceLocation location)
        : PatternNode(location), operand_(std::move(operand)), type_(type) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::PatternRepetition;
    }

    /** @return Non-owning reference to the repeated operand. */
    [[nodiscard]] const PatternNode& operand() const noexcept {
        return *operand_;
    }

    [[nodiscard]] Type type() const noexcept { return type_; }

private:
    std::unique_ptr<PatternNode> operand_;
    Type type_;
};

/**
 * @brief Inclusive byte range stored inside a character class.
 *
 * A single class character is represented by `first == last`.
 */
struct CharacterClassRange {
    char first{};
    char last{};
};

/** @brief FRED character class, optionally negated. */
class CharacterClassPatternNode final : public PatternNode {
public:
    /**
     * @param negated true for a leading `^` inside the class.
     * @param ranges Class members/ranges; ownership is transferred by value.
     * @param location Location of the opening `[` token.
     *
     * @note This value type does not normalize or reorder ranges. Validation of
     *       matching semantics is left to parser/matcher behavior.
     */
    CharacterClassPatternNode(bool negated,
                              std::vector<CharacterClassRange> ranges,
                              SourceLocation location)
        : PatternNode(location),
          negated_(negated),
          ranges_(std::move(ranges)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::PatternCharacterClass;
    }

    [[nodiscard]] bool negated() const noexcept { return negated_; }

    /** @return Non-owning const reference to all stored ranges. */
    [[nodiscard]] const auto& ranges() const noexcept { return ranges_; }

private:
    bool negated_{};
    std::vector<CharacterClassRange> ranges_;
};

/** @brief Explicit parenthesized pattern group. */
class GroupPatternNode final : public PatternNode {
public:
    /**
     * @param expression Group expression; ownership is transferred.
     * @param location Location of the opening `(`.
     *
     * @pre `expression` must be non-null; expression() dereferences it.
     */
    GroupPatternNode(std::unique_ptr<PatternNode> expression,
                     SourceLocation location)
        : PatternNode(location), expression_(std::move(expression)) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::PatternGroup;
    }

    /** @return Non-owning reference to the owned grouped expression. */
    [[nodiscard]] const PatternNode& expression() const noexcept {
        return *expression_;
    }

private:
    std::unique_ptr<PatternNode> expression_;
};

} // namespace fred
