#pragma once

#include "fred/core/BufferManager.hpp"
#include "fred/runtime/Output.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>

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
    bool condition_{};
    bool input_parentheses_required_{true};
    bool monitor_commands_{};
    bool exit_requested_{};
    bool immediate_exit_requested_{};
};

} // namespace fred
