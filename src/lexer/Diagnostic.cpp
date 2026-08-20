#include "fred/lexer/Diagnostic.hpp"

#include <algorithm>
#include <sstream>

namespace fred {

std::string_view diagnostic_severity_name(
    DiagnosticSeverity severity) noexcept {
    switch (severity) {
    case DiagnosticSeverity::Note: return "note";
    case DiagnosticSeverity::Warning: return "warning";
    case DiagnosticSeverity::Error: return "error";
    }
    return "error";
}

std::string format_diagnostic(const Diagnostic& diagnostic) {
    std::ostringstream output;

    output << diagnostic_severity_name(diagnostic.severity)
           << " at "
           << diagnostic.location.line
           << ':'
           << diagnostic.location.column
           << " (level "
           << diagnostic.location.flow_level
           << "): "
           << diagnostic.message;

    if (!diagnostic.source_line.empty()) {
        output << '\n' << diagnostic.source_line << '\n';

        // SourceLocation columns are normally one-based. The defensive
        // column==0 branch avoids an unsigned underflow in malformed/external
        // diagnostics.
        const auto spaces =
            diagnostic.location.column > 0
                ? diagnostic.location.column - 1
                : std::size_t{0};

        output << std::string(spaces, ' ') << '^';
    }

    return output.str();
}

} // namespace fred
