#pragma once

#include <string>
#include <string_view>

namespace fred {

/**
 * @brief Abstract destination for all runtime-visible text.
 *
 * Runtime command code must route visible text through Output rather than write
 * directly to std::cout or a file. This keeps command execution deterministic,
 * testable and embeddable across CLI/Android front ends.
 */
class Output {
public:
    virtual ~Output() = default;

    /**
     * @brief Writes text exactly as supplied.
     *
     * @param text Non-owning view valid for the duration of the call.
     */
    virtual void write(std::string_view text) = 0;

    /**
     * @brief Writes text followed by one `\n`.
     *
     * Implemented entirely in terms of write().
     */
    void write_line(std::string_view text) {
        write(text);
        write("\n");
    }
};

/**
 * @brief In-memory Output implementation used by tests and embedders.
 */
class StringOutput final : public Output {
public:
    /** Appends text to the owned output buffer. */
    void write(std::string_view text) override {
        content_.append(text);
    }

    /** @return Non-owning const reference to accumulated output. */
    [[nodiscard]] const std::string& content() const noexcept {
        return content_;
    }

    /** @return true when no output is currently accumulated. */
    [[nodiscard]] bool empty() const noexcept {
        return content_.empty();
    }

    /** @brief Removes all accumulated output without replacing the object. */
    void clear() noexcept {
        content_.clear();
    }

private:
    std::string content_;
};

} // namespace fred
