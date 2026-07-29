#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace fred {

class Buffer {
public:
    using LineNumber = std::size_t;

    explicit Buffer(std::string name);

    [[nodiscard]] const std::string& name() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] std::size_t line_count() const noexcept;
    [[nodiscard]] LineNumber current_line() const noexcept;
    [[nodiscard]] const std::string& line(LineNumber number) const;
    [[nodiscard]] const std::vector<std::string>& lines() const noexcept;

    void append(std::string text);
    void insert_before(LineNumber number, std::string text);
    void insert_after(LineNumber number, std::vector<std::string> text);
    void replace(LineNumber number, std::string text);
    void erase(LineNumber first, LineNumber last);
    void set_current_line(LineNumber number);

private:
    void require_existing_line(LineNumber number) const;

    std::string name_;
    std::vector<std::string> lines_;
    LineNumber current_line_{0};
};

} // namespace fred
