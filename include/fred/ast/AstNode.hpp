#pragma once

#include "fred/lexer/SourceLocation.hpp"

namespace fred {

/**
 * @brief Enumerates every concrete AST node category currently produced by FREDPP.
 *
 * AstNodeKind is a lightweight runtime discriminator used when code needs to
 * inspect an AstNode without relying exclusively on RTTI. It identifies syntax
 * represented in the AST; it does not imply that the corresponding operation
 * has already been semantically validated or executed.
 */
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
    AppendCommand,
    InsertCommand,
    ChangeCommand,
    MoveCommand,
    TransferCommand,
    BufferCommand,
    GlobalCommand,
    ZapCommand,
    ZapGatherCommand,
    SystemCommand,
    SubstituteCommand,
    ReadCommand,
    WriteCommand,
    CommentCommand,
    MessageCommand,
    FactsCommand,
    OptionCommand,
    QuitCommand
};

/**
 * @brief Abstract root of the FREDPP abstract syntax tree.
 *
 * Every AST node exposes both a stable node kind and the SourceLocation at
 * which the represented construct begins.
 *
 * @par Architectural boundary
 * AstNode and its derived types represent parsed syntax and owned operands.
 * They do not access Buffer state, perform address evaluation, or execute a
 * command. Those responsibilities belong to later semantic/runtime layers.
 */
class AstNode {
public:
    virtual ~AstNode() = default;

    /**
     * @brief Returns the concrete syntactic category of this node.
     *
     * @return AstNodeKind identifying the dynamic node type.
     */
    [[nodiscard]] virtual AstNodeKind kind() const noexcept = 0;

    /**
     * @brief Returns the source position at which this AST construct starts.
     *
     * @return SourceLocation captured during parsing.
     */
    [[nodiscard]] virtual SourceLocation location() const noexcept = 0;
};

} // namespace fred
