#pragma once

#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/lexer/TokenStream.hpp"

#include <memory>
#include <string>

namespace fred {

class CommandParser {
public:
    CommandParser(TokenStream& tokens, const CommandRegistry& registry) noexcept;

    [[nodiscard]] std::unique_ptr<CommandNode> parse();

private:
    [[nodiscard]] bool begins_address(const Token& token) const noexcept;
    [[nodiscard]] std::string parse_parenthesized_buffer_name();
    [[nodiscard]] std::unique_ptr<PatternNode> parse_delimited_pattern();
    void require_command_end();

    TokenStream* tokens_;
    const CommandRegistry* registry_;
};

} // namespace fred
