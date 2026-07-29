#pragma once

#include "fred/lexer/SourceLocation.hpp"

namespace fred {

enum class AstNodeKind {
    AbsoluteAddress,
    CurrentAddress,
    LastAddress,
    RelativeAddress,
    RangeAddress,
    PatternLiteral,
    PatternAnyCharacter,
    PatternAnchor,
    PatternSequence,
    PatternAlternation,
    PatternRepetition,
    PatternCharacterClass,
    PatternGroup,
    PrintCommand,
    ListCommand,
    DeleteCommand,
    AppendCommand
};

class AstNode {
public:
    virtual ~AstNode() = default;

    [[nodiscard]] virtual AstNodeKind kind() const noexcept = 0;
    [[nodiscard]] virtual SourceLocation location() const noexcept = 0;
};

} // namespace fred
