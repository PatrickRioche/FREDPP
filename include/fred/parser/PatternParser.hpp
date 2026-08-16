#pragma once

#include "fred/ast/PatternNodes.hpp"
#include "fred/core/CharacterInterpretation.hpp"

#include <memory>
#include <string_view>
#include <vector>

namespace fred {

class PatternParser {
public:
    explicit PatternParser(std::string_view source, std::size_t flow_level = 0) noexcept;
    PatternParser(
        std::string_view source,
        std::vector<CharacterInterpretation> interpretations,
        std::size_t flow_level = 0);

    [[nodiscard]] std::unique_ptr<PatternNode> parse();

private:
    [[nodiscard]] std::unique_ptr<PatternNode> parse_alternation();
    [[nodiscard]] std::unique_ptr<PatternNode> parse_sequence();
    [[nodiscard]] std::unique_ptr<PatternNode> parse_repetition();
    [[nodiscard]] std::unique_ptr<PatternNode> parse_atom();
    [[nodiscard]] std::unique_ptr<PatternNode> parse_group();
    [[nodiscard]] std::unique_ptr<PatternNode> parse_character_class();

    [[nodiscard]] bool at_end() const noexcept;
    [[nodiscard]] bool is_special(std::size_t position) const noexcept;
    [[nodiscard]] bool peek_is_special(char value, std::size_t lookahead = 0) const noexcept;
    [[nodiscard]] char peek(std::size_t lookahead = 0) const noexcept;
    char consume();
    [[nodiscard]] SourceLocation location() const noexcept;
    [[noreturn]] void fail(std::string_view message) const;

    std::string_view source_;
    std::vector<CharacterInterpretation> interpretations_;
    std::size_t position_{};
    std::size_t flow_level_{};
    char delimiter_{};
};

[[nodiscard]] std::string describe_pattern(const PatternNode& node);

} // namespace fred
