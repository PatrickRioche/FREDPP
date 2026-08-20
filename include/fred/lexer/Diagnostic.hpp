#pragma once

#include "fred/lexer/SourceLocation.hpp"

#include <string>
#include <string_view>

namespace fred {

/**
 * @brief Severity attached to a source diagnostic.
 */
enum class DiagnosticSeverity {
    /** Informational context accompanying another diagnostic. */
    Note,

    /** Non-fatal condition that should be reported to the user. */
    Warning,

    /** Invalid input or processing failure. */
    Error
};

/**
 * @brief Structured diagnostic associated with a FRED source location.
 *
 * `source_line` is optional. When present, format_diagnostic() prints it and
 * adds a caret beneath `location.column`.
 */
struct Diagnostic {
    /** Diagnostic importance. Defaults to Error. */
    DiagnosticSeverity severity{DiagnosticSeverity::Error};

    /** Human-readable diagnostic text, without location prefix. */
    std::string message;

    /** Source position associated with the diagnostic. */
    SourceLocation location;

    /** Optional complete source line used to render a caret. */
    std::string source_line;
};

/**
 * @brief Returns the lowercase display name of a diagnostic severity.
 *
 * @param severity Severity to convert.
 * @return Static string view: "note", "warning" or "error".
 *
 * @note Invalid enum values fall back to "error".
 */
[[nodiscard]] std::string_view diagnostic_severity_name(
    DiagnosticSeverity severity) noexcept;

/**
 * @brief Formats a diagnostic for terminal/user display.
 *
 * @param diagnostic Structured diagnostic to render.
 * @return Newly allocated string containing severity, line/column, flow level,
 *         message and, when available, a source-line/caret display.
 *
 * @note A column value of 0 is tolerated and produces no leading caret spaces.
 * @note This function performs formatting only; it does not write to stdout or
 *       stderr.
 */
[[nodiscard]] std::string format_diagnostic(const Diagnostic& diagnostic);

} // namespace fred
