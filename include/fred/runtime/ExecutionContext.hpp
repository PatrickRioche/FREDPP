#pragma once

#include "fred/core/BufferManager.hpp"
#include "fred/runtime/Output.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace fred {

/**
 * @brief Services and mutable editor/procedure state available to execution.
 *
 * ExecutionContext is a non-owning runtime façade around BufferManager and the
 * currently selected Output destination. It also stores mutable execution state
 * shared by commands/procedures: counter, numeric registers, condition flag,
 * option flags and exit request.
 *
 * @par Ownership
 * BufferManager and Output are borrowed. Both must outlive the context and any
 * operation using references returned by it.
 *
 * @par Initial state
 * counter=0, condition=false, input parentheses required=true, monitor=false,
 * and no exit is requested.
 */
class ExecutionContext {
public:
    /**
     * @param buffers Mutable editor buffer manager; borrowed.
     * @param output Initial runtime output sink; borrowed.
     * @pre Both dependencies must outlive this context.
     */
    ExecutionContext(BufferManager& buffers, Output& output) noexcept;

    /** @return Mutable borrowed BufferManager reference. */
    [[nodiscard]] BufferManager& buffers() noexcept;

    /** @return Const borrowed BufferManager reference. */
    [[nodiscard]] const BufferManager& buffers() const noexcept;

    /**
     * @return Mutable current Buffer through BufferManager.
     * @throws std::logic_error only if BufferManager has no current buffer.
     */
    [[nodiscard]] Buffer& current_buffer();

    /**
     * @return Const current Buffer through BufferManager.
     * @throws std::logic_error only if BufferManager has no current buffer.
     */
    [[nodiscard]] const Buffer& current_buffer() const;

    /** @return Current borrowed Output sink. */
    [[nodiscard]] Output& output() noexcept;

    /**
     * @brief Replaces the current Output sink temporarily/permanently.
     *
     * @param output New borrowed sink.
     * @return Reference to the previously configured sink.
     *
     * @warning Both old and new Output objects must remain alive while any
     *          returned/stored references may be used.
     */
    [[nodiscard]] Output& exchange_output(Output& output) noexcept;

    /** Sets the shared numeric counter (`#` in procedure numeric operands). */
    void set_counter(std::int64_t value) noexcept;

    /** @return Shared numeric counter. */
    [[nodiscard]] std::int64_t counter() const noexcept;

    /**
     * @brief Creates or replaces a named numeric register.
     *
     * @param name Register name stored exactly as supplied.
     * @param value New signed 64-bit value.
     *
     * @note Name syntax/length validation belongs to ProcedureRunner, not this
     *       storage primitive.
     */
    void set_numeric_register(std::string name, std::int64_t value);

    /**
     * @return Register value, or 0 when the register does not exist.
     *
     * @note Use has_numeric_register() when absence must be distinguished from
     *       an explicitly stored zero.
     */
    [[nodiscard]] std::int64_t numeric_register(
        std::string_view name) const;

    /** @return true when an exact register name is present. */
    [[nodiscard]] bool has_numeric_register(
        std::string_view name) const;

    /**
     * @return Names of all created numeric registers in ascending order.
     *
     * @note An empty result means no registers have been created. The result
     *       is a snapshot and does not expose mutable internal storage.
     */
    [[nodiscard]] std::vector<std::string> numeric_register_names() const;

    /**
     * @brief Changes how a numeric register is displayed, not its value.
     *
     * Formatting is attached to the named register and survives subsequent
     * numeric assignments. The caller validates the historical NB/ND bounds.
     * Changing a format creates a zero-valued register if needed, but does not
     * change the shared counter or condition.
     */
    void set_numeric_base(std::string_view name, unsigned base);
    void set_numeric_minimum_digits(std::string_view name, std::size_t digits);
    void set_numeric_fill_character(std::string_view name, char character);

    /** @return Signed register value rendered using its NB/ND/NF settings. */
    [[nodiscard]] std::string format_numeric_register(
        std::string_view name) const;

    /** Sets the shared true/false condition used by conditional procedure jumps. */
    void set_condition(bool value) noexcept;

    /** @return Current condition flag. */
    [[nodiscard]] bool condition() const noexcept;

    /** Controls whether short B syntax is rejected at execution time. */
    void set_input_parentheses_required(bool value) noexcept;

    /** @return Current O+I(/O-I( option state. */
    [[nodiscard]] bool input_parentheses_required() const noexcept;

    /** Controls procedure command monitoring/echo. */
    void set_monitor_commands(bool value) noexcept;

    /** @return Current O+M/O-M monitoring state. */
    [[nodiscard]] bool monitor_commands() const noexcept;

    void set_quit_on_error(bool value) noexcept;
    [[nodiscard]] bool quit_on_error() const noexcept;
    void set_substitution_ampersand(bool value) noexcept;
    [[nodiscard]] bool substitution_ampersand() const noexcept;
    void set_case_insensitive_search(bool value) noexcept;
    [[nodiscard]] bool case_insensitive_search() const noexcept;
    void set_tab_stops(std::vector<std::size_t> stops);
    [[nodiscard]] const std::vector<std::size_t>& tab_stops() const noexcept;
    void set_tabs_enabled(bool value) noexcept;
    [[nodiscard]] bool tabs_enabled() const noexcept;

    /** Historical OIA/OIC/OIR terminal strings (empty means default/off). */
    void set_append_prompt(std::string value);
    [[nodiscard]] const std::string& append_prompt() const noexcept;
    void set_command_prompt(std::string value);
    [[nodiscard]] const std::string& command_prompt() const noexcept;
    void set_return_message(std::string value);
    [[nodiscard]] const std::string& return_message() const noexcept;

    /** ASCII-only input case escape prefixes (OIL/OIU). */
    void set_lower_input_escape(std::optional<char> value) noexcept;
    [[nodiscard]] std::optional<char> lower_input_escape() const noexcept;
    void set_upper_input_escape(std::optional<char> value) noexcept;
    [[nodiscard]] std::optional<char> upper_input_escape() const noexcept;
    [[nodiscard]] std::string transform_input_line(std::string_view line) const;

    /** Host-terminal adaptations of GCOS input and output options. */
    void set_input_change(std::string from, std::string to);
    [[nodiscard]] const std::string& input_change_from() const noexcept;
    [[nodiscard]] const std::string& input_change_to() const noexcept;
    void set_input_terminator_required(bool value) noexcept;
    [[nodiscard]] bool input_terminator_required() const noexcept;
    void set_input_escape(char value) noexcept;
    [[nodiscard]] char input_escape() const noexcept;
    void set_input_feed(bool value) noexcept;
    [[nodiscard]] bool input_feed() const noexcept;
    void set_output_lower(bool value) noexcept;
    [[nodiscard]] bool output_lower() const noexcept;
    void set_output_octal(bool value) noexcept;
    [[nodiscard]] bool output_octal() const noexcept;
    void set_output_upper(bool value) noexcept;
    [[nodiscard]] bool output_upper() const noexcept;
    void set_page_size(std::size_t value) noexcept;
    [[nodiscard]] std::size_t page_size() const noexcept;
    void set_substitution_separator_required(bool value) noexcept;
    [[nodiscard]] bool substitution_separator_required() const noexcept;
    [[nodiscard]] std::string format_output_line(std::string_view line) const;

    /** The last diagnostic is retained until another error replaces it. */
    void set_last_error(std::string message);
    [[nodiscard]] const std::string& last_error() const noexcept;

    /**
     * @brief Records a Q/QQ exit request.
     *
     * @param immediate false for normal Q, true for immediate QQ.
     *
     * @post exit_requested() is true.
     * @post immediate_exit_requested() equals `immediate`.
     */
    void request_exit(bool immediate) noexcept;

    /** @return Whether execution should stop. */
    [[nodiscard]] bool exit_requested() const noexcept;

    /** @return Whether the recorded exit request was immediate. */
    [[nodiscard]] bool immediate_exit_requested() const noexcept;

private:
    /** Borrowed services. */
    BufferManager* buffers_;
    Output* output_;

    std::int64_t counter_{};
    std::unordered_map<std::string, std::int64_t> numeric_registers_;
    struct NumericDisplayOptions {
        unsigned base{10};
        std::size_t minimum_digits{1};
        char fill_character{'0'};
    };
    std::unordered_map<std::string, NumericDisplayOptions> numeric_formats_;
    bool condition_{};
    bool input_parentheses_required_{true};
    bool monitor_commands_{};
    bool quit_on_error_{};
    bool substitution_ampersand_{true};
    bool case_insensitive_search_{};
    std::vector<std::size_t> tab_stops_{5, 9};
    bool tabs_enabled_{true};
    std::string append_prompt_;
    std::string command_prompt_;
    std::string return_message_;
    std::optional<char> lower_input_escape_;
    std::optional<char> upper_input_escape_;
    std::string input_change_from_;
    std::string input_change_to_;
    bool input_terminator_required_{true};
    char input_escape_{'\\'};
    bool input_feed_{true};
    bool output_lower_{};
    // Historical FRED enables octal output of special characters by default.
    bool output_octal_{true};
    bool output_upper_{};
    std::size_t page_size_{};
    bool substitution_separator_required_{true};
    std::string last_error_;
    bool exit_requested_{};
    bool immediate_exit_requested_{};
};

} // namespace fred
