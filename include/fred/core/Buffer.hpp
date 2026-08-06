#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace fred {

enum class TextEncoding {
    Unknown,
    Ascii,
    Utf8
};

enum class LineEnding {
    Lf,
    CrLf
};

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

    [[nodiscard]] bool modified() const noexcept;
    [[nodiscard]] bool has_associated_file() const noexcept;
    [[nodiscard]] const std::optional<std::string>& associated_file() const noexcept;
    [[nodiscard]] TextEncoding encoding() const noexcept;
    [[nodiscard]] LineEnding line_ending() const noexcept;
    [[nodiscard]] bool final_newline() const noexcept;

    void append(std::string text);
    void insert_before(LineNumber number, std::string text);
    void insert_after(LineNumber number, std::vector<std::string> text);
    void replace(LineNumber number, std::string text);
    void erase(LineNumber first, LineNumber last);
    void set_current_line(LineNumber number);

    void load_file(std::vector<std::string> lines,
                   std::string filename,
                   TextEncoding encoding,
                   LineEnding line_ending,
                   bool final_newline);
    void associate_file(std::string filename,
                        TextEncoding encoding,
                        LineEnding line_ending,
                        bool final_newline);
    void mark_clean() noexcept;
    void mark_modified() noexcept;

private:
    void require_existing_line(LineNumber number) const;

    std::string name_;
    std::vector<std::string> lines_;
    LineNumber current_line_{0};
    bool modified_{false};
    std::optional<std::string> associated_file_;
    TextEncoding encoding_{TextEncoding::Unknown};
    LineEnding line_ending_{LineEnding::Lf};
    bool final_newline_{true};
};

} // namespace fred
