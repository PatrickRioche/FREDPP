#pragma once

#include <string>
#include <string_view>

namespace fred {

// Abstract destination for all runtime-visible text.
// The runtime must not write directly to std::cout or to a file.
class Output {
public:
    virtual ~Output() = default;

    virtual void write(std::string_view text) = 0;

    void write_line(std::string_view text) {
        write(text);
        write("\n");
    }
};

// In-memory output used by tests and embedders.
class StringOutput final : public Output {
public:
    void write(std::string_view text) override {
        content_.append(text);
    }

    [[nodiscard]] const std::string& content() const noexcept {
        return content_;
    }

    [[nodiscard]] bool empty() const noexcept {
        return content_.empty();
    }

    void clear() noexcept {
        content_.clear();
    }

private:
    std::string content_;
};

} // namespace fred
