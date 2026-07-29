#pragma once

#include "fred/lexer/SourceLocation.hpp"

#include <string>
#include <string_view>

namespace fred {

enum class DiagnosticSeverity {
    Note,
    Warning,
    Error
};

struct Diagnostic {
    DiagnosticSeverity severity{DiagnosticSeverity::Error};
    std::string message;
    SourceLocation location;
    std::string source_line;
};

[[nodiscard]] std::string_view diagnostic_severity_name(
    DiagnosticSeverity severity) noexcept;

[[nodiscard]] std::string format_diagnostic(const Diagnostic& diagnostic);

} // namespace fred
