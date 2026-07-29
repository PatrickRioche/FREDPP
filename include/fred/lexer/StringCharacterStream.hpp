#pragma once

#include "fred/lexer/CharacterStream.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace fred {

class StringCharacterStream final : public CharacterStream {
public:
    explicit StringCharacterStream(std::string_view source,
                                   std::size_t flow_level = 0);

    [[nodiscard]] std::optional<Character>
    peek(std::size_t lookahead = 0) const noexcept override;

    [[nodiscard]] std::optional<Character> consume() noexcept override;

    [[nodiscard]] bool eof() const noexcept override;
    [[nodiscard]] std::size_t position() const noexcept override;

    void rewind(std::size_t position) override;

    [[nodiscard]] SourceLocation end_location() const noexcept override;

    [[nodiscard]] std::size_t size() const noexcept;

private:
    std::string storage_;
    std::vector<Character> characters_;
    std::size_t position_{0};
    SourceLocation end_location_{};
};

} // namespace fred
