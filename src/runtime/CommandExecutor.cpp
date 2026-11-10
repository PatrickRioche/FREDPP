/**
 * @file CommandExecutor.cpp
 * @brief Executes already-parsed FRED CommandNode AST against Runtime state.
 *
 * This layer performs editor mutation, address evaluation, file/system I/O and
 * output effects. The U repeat command is the sole exception to the usual
 * no-reparse rule: it parses its stored command body for each iteration.
 * Command-specific address exceptions stay here instead of weakening generic
 * AddressEvaluator semantics.
 *
 * @note FREDPP_LOT8_CPP_DOCUMENTATION
 */
#include "fred/runtime/CommandExecutor.hpp"

#include "fred/ast/AbsoluteAddressNode.hpp"

#include "fred/runtime/AddressEvaluator.hpp"
#include "fred/runtime/CommandExecutionError.hpp"
#include "fred/runtime/NumericOperations.hpp"
#include "fred/runtime/PatternMatcher.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fredpp/version.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <functional>
#include <sstream>
#include <iterator>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace fred {
namespace {

/**
 * @brief Prints the addressed or current line number for historical `=`.
 *
 * Without an address, the current cursor is printed (0 for an empty Buffer).
 * An explicit address must resolve to one existing line. Neither form moves
 * the cursor, edits text, or changes the condition/counter registers.
 */
void execute_line_number(const LineNumberCommandNode& command,
                         ExecutionContext& context) {
    const auto& buffer = context.current_buffer();
    const auto line = command.address()
        ? AddressEvaluator{}.evaluate(command.address(), buffer).first
        : buffer.current_line();
    context.output().write_line(std::to_string(line));
}

/** Historical # prints the shared counter and leaves all editor state intact. */
void execute_counter(ExecutionContext& context) {
    context.output().write_line(std::to_string(context.counter()));
}

/** Historical :n prints up to n lines on either side of the current line. */
void execute_window(const WindowCommandNode& command,
                    ExecutionContext& context) {
    auto& buffer = context.current_buffer();
    if (buffer.empty()) return;
    const std::size_t center = buffer.current_line() ? buffer.current_line() : 1;
    const std::size_t first = center > command.radius() ?
        center - command.radius() : 1;
    const std::size_t last = center + std::min(command.radius(),
        buffer.line_count() - center);
    for (std::size_t number = first; number <= last; ++number) {
        context.output().write_line(buffer.line(number));
    }
    buffer.set_current_line(last);
}

/** Historical Y prints the most recent parser or execution diagnostic. */
void execute_why(ExecutionContext& context) {
    context.output().write_line(context.last_error().empty() ?
        "aucune erreur enregistrée" : context.last_error());
}

/**
 * @brief Prints the addressed lines for P and selects the final printed line.
 *
 * The text and modified state remain unchanged; invalid addresses are
 * rejected by AddressEvaluator before any line is printed.
 */
void execute_print(const PrintCommandNode& command, ExecutionContext& context) {
    auto& buffer = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), buffer);
    for (auto line = range.first; line <= range.last; ++line) {
        const auto ordinal = line - range.first;
        if (context.page_size() && ordinal && ordinal % context.page_size() == 0) {
            context.output().write_line("-- page suivante --");
        }
        context.output().write_line(context.format_output_line(buffer.line(line)));
    }
    buffer.set_current_line(range.last);
}

/** @brief Executes D over the evaluated range. */
void execute_delete(const DeleteCommandNode& command, ExecutionContext& context) {
    auto& buffer = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), buffer);
    buffer.erase(range.first, range.last);
}

/**
 * @brief Executes the currently implemented file-explicit L form.
 *
 * The current-file implicit form remains a Runtime implementation gap.
 */
void execute_list(const ListCommandNode& command, ExecutionContext& context) {
    if (!command.filename()) {
        throw CommandExecutionError(
            "L without a filename requires current-file support (not implemented yet)");
    }

    std::ifstream input(*command.filename(), std::ios::binary);
    if (!input) {
        throw CommandExecutionError("cannot open file: " + *command.filename());
    }

    std::string line;
    std::size_t ordinal = 0;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (context.page_size() && ordinal && ordinal % context.page_size() == 0) {
            context.output().write_line("-- page suivante --");
        }
        context.output().write_line(context.format_output_line(line));
        ++ordinal;
    }

    if (input.bad()) {
        throw CommandExecutionError("error while reading file: " + *command.filename());
    }
}


/**
 * @brief Normalized text-file payload plus encoding/newline metadata detected by R.
 */
struct DecodedFile {
    std::vector<std::string> lines;
    TextEncoding encoding{TextEncoding::Unknown};
    LineEnding line_ending{LineEnding::Lf};
    bool final_newline{false};
};

/**
 * @brief Validates UTF-8 byte sequences accepted by current FREDPP text I/O.
 *
 * Rejects invalid starts/continuations, overlong encodings, surrogate code
 * points and values above U+10FFFF.
 */
bool is_valid_utf8(std::string_view text) noexcept {
    std::size_t index = 0;
    while (index < text.size()) {
        const auto first = static_cast<std::uint8_t>(text[index]);
        if (first <= 0x7F) {
            ++index;
            continue;
        }

        std::size_t length = 0;
        std::uint32_t code_point = 0;
        if (first >= 0xC2 && first <= 0xDF) {
            length = 2;
            code_point = first & 0x1F;
        } else if (first >= 0xE0 && first <= 0xEF) {
            length = 3;
            code_point = first & 0x0F;
        } else if (first >= 0xF0 && first <= 0xF4) {
            length = 4;
            code_point = first & 0x07;
        } else {
            return false;
        }

        if (index + length > text.size()) {
            return false;
        }
        for (std::size_t offset = 1; offset < length; ++offset) {
            const auto continuation =
                static_cast<std::uint8_t>(text[index + offset]);
            if ((continuation & 0xC0) != 0x80) {
                return false;
            }
            code_point = (code_point << 6) | (continuation & 0x3F);
        }

        if ((length == 3 && code_point < 0x800) ||
            (length == 4 && code_point < 0x10000) ||
            (code_point >= 0xD800 && code_point <= 0xDFFF) ||
            code_point > 0x10FFFF) {
            return false;
        }
        index += length;
    }
    return true;
}

/**
 * @brief Decodes text bytes into Buffer lines and file metadata.
 *
 * NUL-containing input is classified as unsupported binary. UTF-8 BOM is
 * removed, ASCII/UTF-8 is classified, CRLF/LF style and final newline are
 * recorded for later round-tripping.
 */
DecodedFile decode_file(std::string bytes, const std::string& filename) {
    if (bytes.find('\0') != std::string::npos) {
        throw CommandExecutionError("binary file is not supported: " + filename);
    }

    bool had_bom = false;
    if (bytes.size() >= 3 &&
        static_cast<unsigned char>(bytes[0]) == 0xEF &&
        static_cast<unsigned char>(bytes[1]) == 0xBB &&
        static_cast<unsigned char>(bytes[2]) == 0xBF) {
        bytes.erase(0, 3);
        had_bom = true;
    }

    const bool ascii = std::all_of(bytes.begin(), bytes.end(), [](char value) {
        return static_cast<unsigned char>(value) <= 0x7F;
    });
    if (!ascii && !is_valid_utf8(bytes)) {
        throw CommandExecutionError(
            "file is neither ASCII nor valid UTF-8: " + filename);
    }

    DecodedFile result;
    result.encoding = (ascii && !had_bom) ? TextEncoding::Ascii
                                          : TextEncoding::Utf8;
    result.line_ending = bytes.find("\r\n") != std::string::npos
        ? LineEnding::CrLf : LineEnding::Lf;
    result.final_newline = !bytes.empty() && bytes.back() == '\n';

    std::size_t start = 0;
    while (start < bytes.size()) {
        const auto newline = bytes.find('\n', start);
        if (newline == std::string::npos) {
            result.lines.push_back(bytes.substr(start));
            break;
        }
        auto line = bytes.substr(start, newline - start);
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        result.lines.push_back(std::move(line));
        start = newline + 1;
    }
    return result;
}

/** @brief Reads a file as bytes and delegates text validation to decode_file(). */
DecodedFile read_file(const std::string& filename) {
    std::ifstream input(filename, std::ios::binary);
    if (!input) {
        throw CommandExecutionError("cannot open file: " + filename);
    }
    std::string bytes((std::istreambuf_iterator<char>(input)),
                      std::istreambuf_iterator<char>());
    if (input.bad()) {
        throw CommandExecutionError("error while reading file: " + filename);
    }
    return decode_file(std::move(bytes), filename);
}

/**
 * @brief Splits an R operand into names, preserving whitespace within quotes.
 *
 * Quote characters delimit a filename, but are not passed to the filesystem.
 * On Windows a literal quote cannot be part of a filename.
 */
std::vector<std::string> read_filename_candidates(std::string_view operand) {
    std::vector<std::string> result;
    std::string candidate;
    bool quoted = false;
    bool started = false;
    for (const char character : operand) {
        if (character == '"') {
            quoted = !quoted;
            started = true;
        } else if (!quoted && std::isspace(
                       static_cast<unsigned char>(character))) {
            if (started) {
                if (candidate.empty()) {
                    throw CommandExecutionError("R filename must not be empty");
                }
                result.push_back(std::move(candidate));
                candidate.clear();
                started = false;
            }
        } else {
            candidate.push_back(character);
            started = true;
        }
    }
    if (quoted) {
        throw CommandExecutionError("unterminated quoted filename in R");
    }
    if (started) {
        if (candidate.empty()) {
            throw CommandExecutionError("R filename must not be empty");
        }
        result.push_back(std::move(candidate));
    }
    return result;
}

/** @brief A decoded file paired with the selected on-disk path. */
struct ResolvedReadFile {
    std::string filename;
    DecodedFile decoded;
};

/**
 * @brief Tries the complete path first, then R's ordered quoted-name list.
 *
 * The complete operand is attempted only if no quote is present: this keeps
 * legacy unquoted single paths with spaces usable. No Buffer state is changed
 * until a candidate has been decoded successfully.
 */
ResolvedReadFile read_first_available_file(const std::string& operand) {
    const auto candidates = read_filename_candidates(operand);
    if (candidates.empty()) {
        throw CommandExecutionError("R filename must not be empty");
    }
    if (operand.find('"') == std::string::npos) {
        try {
            return {operand, read_file(operand)};
        } catch (const CommandExecutionError&) {
            if (candidates.size() == 1) {
                throw;
            }
        }
    }

    std::string last_error;
    for (const auto& candidate : candidates) {
        try {
            return {candidate, read_file(candidate)};
        } catch (const CommandExecutionError& error) {
            last_error = error.what();
        }
    }
    if (candidates.size() == 1) {
        throw CommandExecutionError(last_error);
    }
    throw CommandExecutionError(
        "R cannot read any file from list: " + operand);
}

/**
 * @brief Executes R using either full-buffer load or addressed insertion semantics.
 *
 * A full load establishes file association/encoding metadata through Buffer;
 * addressed R inserts decoded lines and leaves generic AddressEvaluator strict.
 * Counter records the number of read lines and condition becomes true on success.
 */
void execute_read(const ReadCommandNode& command,
                  ExecutionContext& context) {
    if (command.zap_read()) {
        if (!command.filename()) {
            throw CommandExecutionError("ZR requires a filename");
        }
        std::vector<std::string> possible_names{*command.filename()};
        for (auto& candidate : read_filename_candidates(*command.filename())) {
            possible_names.push_back(std::move(candidate));
        }
        // Re-select an associated buffer without reloading or discarding edits.
        for (const auto& name : context.buffers().names()) {
            const auto& existing = context.buffers().get(name);
            if (existing.associated_file() &&
                std::find(possible_names.begin(), possible_names.end(),
                          *existing.associated_file()) != possible_names.end()) {
                const auto& selected = context.buffers().select(name);
                context.output().write_line(
                    "b(" + name + ") " +
                    std::to_string(selected.current_line()) + "," +
                    std::to_string(selected.line_count()) + " " +
                    *selected.associated_file());
                return;
            }
        }
        auto resolved = read_first_available_file(*command.filename());
        std::string name;
        for (char letter = 'a'; letter <= 'z'; ++letter) {
            const std::string candidate(1, letter);
            if (!context.buffers().contains(candidate)) {
                name = candidate;
                break;
            }
        }
        if (name.empty()) {
            throw CommandExecutionError("ZR has no available automatic buffer name");
        }
        const auto count = resolved.decoded.lines.size();
        std::error_code ec;
        const auto bytes = std::filesystem::file_size(resolved.filename, ec);
        auto& selected = context.buffers().create_or_select(name);
        selected.load_file(std::move(resolved.decoded.lines), resolved.filename,
                           resolved.decoded.encoding, resolved.decoded.line_ending,
                           resolved.decoded.final_newline);
        context.set_counter(static_cast<std::int64_t>(count));
        context.set_condition(true);
        context.output().write_line(std::to_string(count) + "," +
            (ec ? "?" : std::to_string(bytes)) + " b(" + name + ") " +
            resolved.filename);
        return;
    }
    auto& buffer = context.current_buffer();

    std::string filename;
    if (command.filename()) {
        filename = *command.filename();
    } else if (buffer.associated_file()) {
        filename = *buffer.associated_file();
    } else {
        throw CommandExecutionError(
            "R requires a filename when the buffer has no associated file");
    }

    if (command.address() == nullptr) {
        if (!buffer.empty()) {
            throw CommandExecutionError("buffer not empty");
        }

        auto resolved = read_first_available_file(filename);
        const auto line_count = resolved.decoded.lines.size();
        buffer.load_file(std::move(resolved.decoded.lines), resolved.filename,
                         resolved.decoded.encoding, resolved.decoded.line_ending,
                         resolved.decoded.final_newline);
        context.set_counter(line_count);
        context.set_condition(true);
        return;
    }

    Buffer::LineNumber insertion_after{};
    if (buffer.empty()) {
        // For addressed R, historical '$' is the end insertion position.
        // On an empty buffer that position is zero. This special case belongs
        // to R insertion semantics; AddressEvaluator remains strict for the
        // other commands and address forms.
        if (command.address()->kind() != AstNodeKind::LastAddress) {
            throw CommandExecutionError(
                "cannot address lines in an empty buffer");
        }
        insertion_after = 0;
    } else {
        const auto range =
            AddressEvaluator{}.evaluate(command.address(), buffer);
        if (range.first != range.last) {
            throw CommandExecutionError(
                "R insertion address must be a single line");
        }
        insertion_after = range.first;
    }

    auto resolved = read_first_available_file(filename);
    const auto line_count = resolved.decoded.lines.size();
    buffer.insert_after(insertion_after, std::move(resolved.decoded.lines));
    context.set_counter(line_count);
    context.set_condition(true);
}

bool contains_non_ascii(const Buffer& buffer, LineRange range) {
    for (auto line = range.first; line <= range.last; ++line) {
        const auto& text = buffer.line(line);
        if (std::any_of(text.begin(), text.end(), [](char value) {
                return static_cast<unsigned char>(value) > 0x7F;
            })) {
            return true;
        }
    }
    return false;
}

void validate_utf8_lines(const Buffer& buffer, LineRange range) {
    for (auto line = range.first; line <= range.last; ++line) {
        if (!is_valid_utf8(buffer.line(line))) {
            throw CommandExecutionError(
                "buffer contains invalid UTF-8 text at line " +
                std::to_string(line));
        }
    }
}

/**
 * @brief Executes W/WA/WU and preserves the Buffer's newline/final-newline policy.
 *
 * WB is rejected as unsupported historical BCD. Full-buffer writes update file
 * association/encoding and mark the Buffer clean; partial writes do not.
 */
void execute_write(const WriteCommandNode& command, ExecutionContext& context) {
    auto& buffer = context.current_buffer();
    if (command.mode() == FileWriteMode::BcdUnsupported) {
        throw CommandExecutionError(
            "WB is the historical GCOS/BCD format and is not supported; "
            "use WA for ASCII or WU for UTF-8");
    }

    std::string filename;
    if (command.filename()) {
        filename = *command.filename();
    } else if (buffer.associated_file()) {
        filename = *buffer.associated_file();
    } else {
        throw CommandExecutionError(
            "W requires a filename when the buffer has no associated file");
    }

    const bool empty_buffer = buffer.empty();
    if (empty_buffer && command.address() != nullptr) {
        throw CommandExecutionError("cannot address lines in an empty buffer");
    }

    LineRange range{0, 0};
    bool full_buffer = true;
    if (!empty_buffer) {
        range = command.address() == nullptr
            ? LineRange{1, buffer.line_count()}
            : AddressEvaluator{}.evaluate(command.address(), buffer);
        full_buffer = range.first == 1 && range.last == buffer.line_count();
    }

    TextEncoding encoding = TextEncoding::Utf8;
    if (command.mode() == FileWriteMode::Ascii) {
        encoding = TextEncoding::Ascii;
    } else if (command.mode() == FileWriteMode::Utf8) {
        encoding = TextEncoding::Utf8;
    } else if (buffer.encoding() == TextEncoding::Ascii) {
        encoding = TextEncoding::Ascii;
    }

    if (!empty_buffer) {
        if (encoding == TextEncoding::Ascii && contains_non_ascii(buffer, range)) {
            throw CommandExecutionError(
                "text cannot be represented in ASCII; use WU for UTF-8");
        }
        if (encoding == TextEncoding::Utf8) {
            validate_utf8_lines(buffer, range);
        }
    }

    // ZW appends without replacing the existing file. When the previous file
    // has an unterminated final line, insert one separator first.
    bool needs_separator = false;
    if (command.append_file()) {
        std::ifstream previous(filename, std::ios::binary | std::ios::ate);
        if (previous && previous.tellg() > 0) {
            previous.seekg(-1, std::ios::end);
            char last = '\0';
            previous.get(last);
            needs_separator = last != '\n';
        }
    }
    const bool newly_created = !std::filesystem::exists(filename);
    std::ofstream output(filename, std::ios::binary |
        (command.append_file() ? std::ios::app : std::ios::trunc));
    if (!output) {
        throw CommandExecutionError("cannot create file: " + filename);
    }

    const std::string_view newline =
        buffer.line_ending() == LineEnding::CrLf ? "\r\n" : "\n";
    if (!empty_buffer) {
        if (needs_separator) {
            output.write(newline.data(), static_cast<std::streamsize>(newline.size()));
        }
        for (auto line = range.first; line <= range.last; ++line) {
            output.write(buffer.line(line).data(),
                         static_cast<std::streamsize>(buffer.line(line).size()));
            const bool write_newline = line < range.last || !full_buffer ||
                                       buffer.final_newline();
            if (write_newline) {
                output.write(newline.data(),
                             static_cast<std::streamsize>(newline.size()));
            }
        }
    }
    if (!output) {
        throw CommandExecutionError("error while writing file: " + filename);
    }
    output.close();
    if (!output) throw CommandExecutionError("error while closing file: " + filename);
#ifndef _WIN32
    // Host-only interpretation of GCOS public read: affect new files only.
    // Never change the permissions of pre-existing user files on overwrite.
    if (newly_created) {
        const auto public_bits = std::filesystem::perms::group_read |
                                 std::filesystem::perms::others_read;
        std::error_code error;
        std::filesystem::permissions(filename, public_bits,
            context.public_read() ? std::filesystem::perm_options::add :
                                    std::filesystem::perm_options::remove,
            error);
        if (error) throw CommandExecutionError("OR file permission update failed: " + error.message());
    }
#endif

    context.set_counter(empty_buffer ? 0 : range.last - range.first + 1);
    context.set_condition(true);
    if (full_buffer && !command.append_file()) {
        buffer.associate_file(filename, encoding, buffer.line_ending(),
                              buffer.final_newline());
        buffer.mark_clean();
    }
}

/**
 * @brief Resolves A's insertion position, including the command-specific 0 case.
 */
Buffer::LineNumber evaluate_append_address(const AppendCommandNode& command,
                                           const Buffer& buffer) {
    const auto* address = command.address();
    if (address == nullptr) {
        return buffer.line_count(); // Historical default: $.
    }
    if (address->kind() == AstNodeKind::RangeAddress) {
        throw CommandExecutionError("A accepts at most one line address");
    }
    if (address->kind() == AstNodeKind::AbsoluteAddress &&
        static_cast<const AbsoluteAddressNode&>(*address).line() == 0) {
        return 0;
    }
    return AddressEvaluator{}.evaluate(address, buffer).first;
}

/**
 * @brief Resolves I to the position after which text will be inserted.
 *
 * This keeps I's zero/before-current behavior outside generic AddressEvaluator.
 */
Buffer::LineNumber evaluate_insert_position(const InsertCommandNode& command,
                                            const Buffer& buffer) {
    const auto* address = command.address();
    if (address == nullptr) {
        if (buffer.empty()) {
            return 0;
        }
        return buffer.current_line() - 1;
    }
    if (address->kind() == AstNodeKind::RangeAddress) {
        throw CommandExecutionError("I accepts at most one line address");
    }
    if (address->kind() == AstNodeKind::AbsoluteAddress &&
        static_cast<const AbsoluteAddressNode&>(*address).line() == 0) {
        return 0;
    }
    return AddressEvaluator{}.evaluate(address, buffer).first - 1;
}


/**
 * @brief Copies addressed source lines into a destination Buffer (historical K).
 *
 * K replaces the destination content. For different source/destination Buffers,
 * source text and its modified flag are preserved, the source cursor moves to
 * the last copied line, and the destination cursor moves to its last new line.
 * Copying into the same Buffer instead replaces that Buffer's entire content
 * with the selected lines and marks it modified. In both cases source
 * lines are snapshotted before the destination is changed.
 */
void execute_copy(const CopyCommandNode& command,
                  ExecutionContext& context) {
    auto& manager = context.buffers();
    auto& source = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), source);

    std::vector<std::string> copied_lines;
    copied_lines.reserve(range.last - range.first + 1);
    for (auto line = range.first; line <= range.last; ++line) {
        copied_lines.push_back(source.line(line));
    }

    if (command.buffer_name() == source.name()) {
        source.erase(1, source.line_count());
        for (auto& line : copied_lines) {
            source.append(std::move(line));
        }
        return;
    }

    // get_or_create does not change the selected source Buffer or MRU order.
    auto& destination = manager.get_or_create(command.buffer_name());
    if (!destination.empty()) {
        destination.erase(1, destination.line_count());
    }
    for (auto& line : copied_lines) {
        destination.append(std::move(line));
    }
    source.set_current_line(range.last);
}

/**
 * @brief Moves the selected source range into a destination Buffer.
 *
 * M replaces the destination content. Moving to the current Buffer is handled
 * specially; cross-buffer moves reselect the source before deleting moved lines.
 */
void execute_move(const MoveCommandNode& command,
                  ExecutionContext& context) {
    auto& manager = context.buffers();
    auto& source = context.current_buffer();
    const std::string source_name = source.name();
    const auto range = AddressEvaluator{}.evaluate(command.address(), source);

    std::vector<std::string> moved_lines;
    moved_lines.reserve(range.last - range.first + 1);
    for (auto line = range.first; line <= range.last; ++line) {
        moved_lines.push_back(source.line(line));
    }

    // The destination may be the current Buffer. In that case its complete
    // content is replaced by the selected source lines.
    if (command.buffer_name() == source_name) {
        source.erase(1, source.line_count());
        for (auto& line : moved_lines) {
            source.append(std::move(line));
        }
        return;
    }

    // M(buffer) replaces the destination Buffer's previous content.
    auto& destination = manager.create_or_select(command.buffer_name());
    if (!destination.empty()) {
        destination.erase(1, destination.line_count());
    }
    for (auto& line : moved_lines) {
        destination.append(std::move(line));
    }

    // Reselect the source explicitly before deleting the moved range.
    manager.select(source_name);
    auto& source_after = manager.current();
    source_after.erase(range.first, range.last);
}


/**
 * @brief Implements historical T (pattern test), including `T~` inversion.
 *
 * The condition is true when at least one addressed line matches, or when no
 * line matches for T~. Neither editor lines nor the current-line cursor nor
 * the numeric counter are modified. Invalid addresses propagate before the
 * condition flag is changed.
 */
void execute_test(const TestCommandNode& command, ExecutionContext& context) {
    const auto& buffer = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), buffer);
    const PatternMatcher matcher(context.case_insensitive_search());
    bool found = false;
    for (auto line = range.first; line <= range.last; ++line) {
        if (matcher.search(command.pattern(), buffer.line(line))) {
            found = true;
            break;
        }
    }
    context.set_condition(command.inverted() ? !found : found);
}



/**
 * @brief Expands a matched S replacement using FREDPP's current rules.
 *
 * An unescaped `&` inserts the matched text. A backslash quotes the next
 * replacement character, including `&` and a literal backslash.
 */
std::string expand_substitution_replacement(std::string_view replacement,
                                            std::string_view matched_text,
                                            bool expand_ampersand) {
    std::string result;
    for (std::size_t index = 0; index < replacement.size(); ++index) {
        const char value = replacement[index];
        if (value == '\\' && index + 1 < replacement.size()) {
            result.push_back(replacement[++index]);
        } else if (value == '&' && expand_ampersand) {
            result.append(matched_text);
        } else {
            result.push_back(value);
        }
    }
    return result;
}

/**
 * @brief Replaces every match on one line without changing the original text.
 *
 * Returns std::nullopt only if no match was found. Even zero-length matches
 * advance the search to prevent infinite loops; a same-text replacement still
 * counts as a match under the current S command semantics.
 */
std::optional<std::string> substitute_all(const PatternNode& pattern,
                                          std::string_view replacement,
                                          std::string_view text,
                                          bool expand_ampersand,
                                          bool case_insensitive) {
    PatternMatcher matcher(case_insensitive);
    std::string result;
    std::size_t copied_until = 0;
    std::size_t search_from = 0;
    bool matched = false;

    while (search_from <= text.size()) {
        const auto match = matcher.find(pattern, text, search_from);
        if (!match) {
            break;
        }

        matched = true;
        result.append(text.substr(copied_until, match->start - copied_until));
        result += expand_substitution_replacement(
            replacement, text.substr(match->start, match->end - match->start),
            expand_ampersand);

        if (match->end > match->start) {
            copied_until = match->end;
            if (match->end == text.size()) {
                search_from = text.size() + 1;
                break;
            }
            search_from = match->end;
            continue;
        }

        // A zero-length match must still advance through the input to avoid an
        // infinite loop while retaining the character at the match position.
        copied_until = match->start;
        if (match->start == text.size()) {
            search_from = text.size() + 1;
            break;
        }
        result.push_back(text[match->start]);
        copied_until = match->start + 1;
        search_from = copied_until;
    }

    if (!matched) {
        return std::nullopt;
    }
    result.append(text.substr(copied_until));
    return result;
}

/**
 * @brief Executes S over an evaluated range and updates condition/current line.
 *
 * No-match is a Runtime error. Optional P output reports the final changed line.
 */
void execute_substitute(const SubstituteCommandNode& command,
                        ExecutionContext& context) {
    if (!command.closing_separator() && context.substitution_separator_required()) {
        throw CommandExecutionError("S requires a closing separator (O+S/)");
    }
    auto& buffer = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), buffer);
    bool changed = false;
    Buffer::LineNumber last_changed = 0;

    for (auto line = range.first; line <= range.last; ++line) {
        const std::string original = buffer.line(line);
        const auto replacement = substitute_all(
            command.pattern(), command.replacement(), original,
            context.substitution_ampersand(), context.case_insensitive_search());
        if (!replacement) {
            continue;
        }
        buffer.replace(line, *replacement);
        changed = true;
        last_changed = line;
    }

    context.set_condition(changed);
    if (!changed) {
        throw CommandExecutionError("no text changed");
    }

    buffer.set_current_line(last_changed);
    if (command.print_after()) {
        context.output().write_line(buffer.line(last_changed));
    }
}

/**
 * @brief Formats one FB entry: name, cursor, line count, file and modified flag.
 *
 * FB only iterates Buffers in the selection-history list; a Buffer created
 * without ever being selected need not appear in its output.
 */
std::string describe_buffer(const Buffer& buffer) {
    std::string result =
        "b(" + buffer.name() + ") " +
        std::to_string(buffer.current_line()) + "," +
        std::to_string(buffer.line_count());

    if (buffer.associated_file()) {
        result += " " + *buffer.associated_file();
    }
    if (buffer.modified()) {
        result += " ?";
    }
    return result;
}

/**
 * @brief Formats an FF entry for a Buffer known to have a file association.
 *
 * Unlike FB, FF omits cursor/line count and lists only the associated path,
 * followed by `?` when the Buffer is modified.
 * @pre buffer.associated_file() must contain a filename.
 */
std::string describe_file_association(const Buffer& buffer) {
    std::string result = "b(" + buffer.name() + ") " +
                         *buffer.associated_file();
    if (buffer.modified()) {
        result += " ?";
    }
    return result;
}

/**
 * @brief Formats an F? entry with its optional file and modification marker.
 *
 * Unlike FB, F? omits cursor/count. Unlike FF, the current Buffer and any
 * modified Buffer without a file association must also be shown.
 */
std::string describe_modified_buffer(const Buffer& buffer) {
    std::string result = "b(" + buffer.name() + ")";
    if (buffer.associated_file()) {
        result += " " + *buffer.associated_file();
    }
    if (buffer.modified()) {
        result += " ?";
    }
    return result;
}

/**
 * @brief Executes the read-only F, FB, FF, FN, FO and F? queries.
 *
 * FB uses most-recently-selected Buffer order; FF examines every owned Buffer
 * but prints only file-associated entries in name order. F? prints the current
 * Buffer first and then every other modified Buffer in name order, including
 * buffers without files. FN prints the created numeric registers sorted by
 * name, while FO prints the active OI( and OM states. None of these queries
 * select Buffers, move cursors, or change editor or procedure state.
 */
void execute_facts(const FactsCommandNode& command,
                   ExecutionContext& context) {
    switch (command.facts()) {
    case FactsKind::Current:
        context.output().write_line(
            describe_buffer(context.buffers().current()));
        return;

    case FactsKind::Options:
        context.output().write_line(
            context.input_parentheses_required() ? "o+i(" : "o-i(");
        context.output().write_line(
            context.monitor_commands() ? "o+m" : "o-m");
        context.output().write_line(
            context.quit_on_error() ? "o+q" : "o-q");
        context.output().write_line(
            context.substitution_ampersand() ? "o+s&" : "o-s&");
        context.output().write_line(
            context.case_insensitive_search() ? "o+sd" : "o-sd");
        if (context.tabs_enabled()) {
            std::string stops = "o+t";
            for (const auto stop : context.tab_stops()) {
                if (stops != "o+t") stops += ",";
                stops += std::to_string(stop);
            }
            context.output().write_line(stops);
        } else {
            context.output().write_line("o-t");
        }
        if (!context.append_prompt().empty()) {
            context.output().write_line("o+ia/" + context.append_prompt() + "/");
        }
        if (!context.command_prompt().empty()) {
            context.output().write_line("o+ic/" + context.command_prompt() + "/");
        }
        if (!context.return_message().empty()) {
            context.output().write_line("o+ir/" + context.return_message() + "/");
        }
        if (context.lower_input_escape()) {
            context.output().write_line(std::string("o+il") + *context.lower_input_escape());
        }
        if (context.upper_input_escape()) {
            context.output().write_line(std::string("o+iu") + *context.upper_input_escape());
        }
        if (!context.input_change_from().empty()) {
            context.output().write_line("o+c/" + context.input_change_from() +
                "/" + context.input_change_to() + "/");
        }
        if (!context.input_terminator_required())
            context.output().write_line("o-i\\f");
        if (context.input_escape() != '\\')
            context.output().write_line(std::string("o+ie") + context.input_escape());
        if (!context.input_feed()) context.output().write_line("o-if");
        if (context.output_lower()) context.output().write_line("o+ol");
        if (!context.output_octal()) context.output().write_line("o-oo");
        if (context.output_upper()) context.output().write_line("o+ou");
        if (context.page_size())
            context.output().write_line("o+p" + std::to_string(context.page_size()));
        if (!context.substitution_separator_required())
            context.output().write_line("o-s/");
        if (context.work_pages() != 3)
            context.output().write_line("o+b" + std::to_string(context.work_pages()));
        if (!context.output_special_characters().empty())
            context.output().write_line("o+o" + context.output_special_characters());
        if (!context.public_read()) context.output().write_line("o-r");
        if (!context.pattern_disabled_characters().empty())
            context.output().write_line("o-s" + context.pattern_disabled_characters());
        return;

    case FactsKind::Version:
        context.output().write_line(
            "FREDPP v" + std::string(fredpp::version()) +
            " - compiled " + __DATE__ + " " + __TIME__ +
            " - commit " + std::string(fredpp::git_commit()));
        return;

    case FactsKind::Patterns:
        for (const auto& [name, source] : context.patterns()) {
            context.output().write_line("e(" + name + ")" + source);
        }
        return;

    case FactsKind::Buffers:
        for (const auto& name : context.buffers().recent_names()) {
            context.output().write_line(
                describe_buffer(context.buffers().get(name)));
        }
        return;

    case FactsKind::Files:
        // Iterate every owned buffer, even one never selected via B(name).
        // names() is sorted, so output does not depend on selection history.
        for (const auto& name : context.buffers().names()) {
            const auto& buffer = context.buffers().get(name);
            if (buffer.has_associated_file()) {
                context.output().write_line(
                    describe_file_association(buffer));
            }
        }
        return;

    case FactsKind::Registers:
        // A register with value zero is still listed; uncreated registers are
        // not. Sorting is done by ExecutionContext, not hash-table iteration.
        for (const auto& name : context.numeric_register_names()) {
            context.output().write_line(
                "n(" + name + "):" +
                context.format_numeric_register(name));
        }
        return;

    case FactsKind::Modified: {
        const auto& current = context.buffers().current();
        context.output().write_line(describe_modified_buffer(current));
        // modified_names() is sorted; avoid listing the current Buffer twice.
        for (const auto& name : context.buffers().modified_names()) {
            if (name != current.name()) {
                context.output().write_line(
                    describe_modified_buffer(context.buffers().get(name)));
            }
        }
        return;
    }
    }
}

/**
 * @brief Resolves and applies one historical N: register assignment.
 *
 * Matching the procedure numeric behavior, an assignment updates the shared
 * counter to the stored register value while leaving condition and Buffer
 * state unchanged. Only the parsed N: operation reaches this function.
 */
std::int64_t resolve_numeric_value(NumericValueKind kind,
                                   std::int64_t literal,
                                   std::string_view reference,
                                   ExecutionContext& context) {
    std::int64_t value = literal;
    switch (kind) {
    case NumericValueKind::Literal:
        break;
    case NumericValueKind::Counter:
        value = context.counter();
        break;
    case NumericValueKind::CurrentLine:
    case NumericValueKind::LastLine: {
        const auto line = kind == NumericValueKind::CurrentLine
            ? context.current_buffer().current_line()
            : context.current_buffer().line_count();
        if (line > static_cast<std::size_t>(
                std::numeric_limits<std::int64_t>::max())) {
            throw CommandExecutionError("numeric line value exceeds signed 64-bit range");
        }
        value = static_cast<std::int64_t>(line);
        break;
    }
    case NumericValueKind::Register:
        value = context.numeric_register(reference);
        break;
    }
    return value;
}

void execute_numeric_assignment(const NumericAssignmentCommandNode& command,
                                ExecutionContext& context) {
    const auto value = resolve_numeric_value(
        command.value_kind(), command.literal_value(), {}, context);

    context.set_numeric_register(command.name(), value);
    context.set_counter(value);
}

/** Execute numeric arithmetic, formatting, line queries or printing. */
void execute_numeric_operation(const NumericOperationCommandNode& command,
                               ExecutionContext& context) {
    const auto operation = command.operation();
    if (operation == NumericOperationKind::Print) {
        context.output().write_line(
            context.format_numeric_register(command.name()));
        return;
    }
    if (operation == NumericOperationKind::CurrentLine) {
        const auto current = resolve_numeric_value(
            NumericValueKind::CurrentLine, 0, {}, context);
        context.set_numeric_register(command.name(), current);
        context.set_counter(current);
        return;
    }
    if (operation == NumericOperationKind::CountCharacters) {
        // Count UTF-8 code points within the addressed lines; line separators
        // are not stored in Buffer and are not counted. A continuation byte
        // belongs to the code point that starts at a previous byte.
        const auto& buffer = context.current_buffer();
        const auto range = AddressEvaluator{}.evaluate(command.address(), buffer);
        std::uint64_t count = 0;
        for (auto line = range.first; line <= range.last; ++line) {
            for (const unsigned char byte : buffer.line(line)) {
                if ((byte & 0xC0) != 0x80) {
                    ++count;
                }
                if (count > static_cast<std::uint64_t>(
                        std::numeric_limits<std::int64_t>::max())) {
                    throw CommandExecutionError("numeric character count overflow");
                }
            }
        }
        const auto result = static_cast<std::int64_t>(count);
        context.set_numeric_register(command.name(), result);
        context.set_counter(result);
        return;
    }
    if (operation == NumericOperationKind::FillCharacter) {
        context.set_numeric_fill_character(
            command.name(), command.fill_character());
        return;
    }
    const auto operand = resolve_numeric_value(
        command.value_kind(), command.literal_value(),
        command.operand_register(), context);
    if (operation == NumericOperationKind::ChangeBase) {
        if (operand < 2 || operand > 36) {
            throw CommandExecutionError("numeric base must be between 2 and 36");
        }
        context.set_numeric_base(command.name(),
                                 static_cast<unsigned>(operand));
        return;
    }
    if (operation == NumericOperationKind::MinimumDigits) {
        // Keep malformed input from requesting unbounded output allocation.
        if (operand < 0 || operand > 1024) {
            throw CommandExecutionError("numeric minimum digits must be between 0 and 1024");
        }
        context.set_numeric_minimum_digits(
            command.name(), static_cast<std::size_t>(operand));
        return;
    }
    apply_numeric_operation(context, command.name(), operation, operand);
}

/**
 * @brief Reassociates the current Buffer with a filename without file I/O.
 *
 * Historical `F filename` changes only the association. Existing encoding,
 * newline metadata, cursor and modified flag are retained. No file is read,
 * written or created by this command.
 */
void execute_file_association(const FileAssociationCommandNode& command,
                              ExecutionContext& context) {
    auto& buffer = context.current_buffer();
    buffer.associate_file(command.filename(), buffer.encoding(),
                          buffer.line_ending(), buffer.final_newline());
}

/**
 * @brief Applies O+I(/O-I( or O+M/O-M to shared Runtime options.
 *
 * OI( controls acceptance of B's compact names; OM enables/disables procedure
 * command monitoring performed by ProcedureRunner. No Buffer is edited here.
 */
void execute_option(const OptionCommandNode& command,
                    ExecutionContext& context) {
    switch (command.option()) {
    case OptionKind::InputParenthesis:
        context.set_input_parentheses_required(command.enabled());
        return;
    case OptionKind::Monitor:
        context.set_monitor_commands(command.enabled());
        return;
    case OptionKind::QuitOnError:
        context.set_quit_on_error(command.enabled());
        return;
    case OptionKind::SubstituteAmpersand:
        context.set_substitution_ampersand(command.enabled());
        return;
    case OptionKind::CaseInsensitiveSearch:
        context.set_case_insensitive_search(command.enabled());
        return;
    case OptionKind::TabStops:
        if (command.enabled()) context.set_tab_stops(command.tab_stops());
        context.set_tabs_enabled(command.enabled());
        return;
    case OptionKind::InputAppendPrompt:
        context.set_append_prompt(command.option_value());
        return;
    case OptionKind::InputCommandPrompt:
        context.set_command_prompt(command.option_value());
        return;
    case OptionKind::InputReturnMessage:
        context.set_return_message(command.option_value());
        return;
    case OptionKind::InputLowerEscape:
        if (command.enabled() && context.upper_input_escape() &&
            *context.upper_input_escape() == command.option_value().at(0)) {
            throw CommandExecutionError("OIL and OIU cannot use the same escape");
        }
        context.set_lower_input_escape(command.enabled() ?
            std::optional<char>(command.option_value().at(0)) : std::nullopt);
        return;
    case OptionKind::InputUpperEscape:
        if (command.enabled() && context.lower_input_escape() &&
            *context.lower_input_escape() == command.option_value().at(0)) {
            throw CommandExecutionError("OIL and OIU cannot use the same escape");
        }
        context.set_upper_input_escape(command.enabled() ?
            std::optional<char>(command.option_value().at(0)) : std::nullopt);
        return;
    case OptionKind::InputChange:
        context.set_input_change(command.option_value(), command.option_value2());
        return;
    case OptionKind::InputTerminatorRequired:
        context.set_input_terminator_required(command.enabled());
        return;
    case OptionKind::InputEscape:
        context.set_input_escape(command.enabled() ? command.option_value().at(0) : '\\');
        return;
    case OptionKind::InputFeed:
        context.set_input_feed(command.enabled());
        return;
    case OptionKind::OutputLower:
        if (command.enabled() && context.output_upper()) {
            throw CommandExecutionError("OOL and OOU cannot both be active");
        }
        context.set_output_lower(command.enabled());
        return;
    case OptionKind::OutputOctal:
        context.set_output_octal(command.enabled());
        return;
    case OptionKind::OutputUpper:
        if (command.enabled() && context.output_lower()) {
            throw CommandExecutionError("OOL and OOU cannot both be active");
        }
        context.set_output_upper(command.enabled());
        return;
    case OptionKind::PageSize:
        context.set_page_size(command.enabled() ?
            static_cast<std::size_t>(std::stoul(command.option_value())) : 0);
        return;
    case OptionKind::SubstituteSeparator:
        context.set_substitution_separator_required(command.enabled());
        return;
    case OptionKind::BufferPages:
        context.adjust_work_pages(
            (command.enabled() ? 1 : -1) *
            static_cast<std::int64_t>(std::stoll(command.option_value())));
        return;
    case OptionKind::OutputCharacters:
        context.set_output_special_characters(command.option_value(), command.enabled());
        return;
    case OptionKind::PublicRead:
        context.set_public_read(command.enabled());
        return;
    case OptionKind::SpecialCharacters:
        context.set_pattern_special_characters(command.option_value(), command.enabled());
        return;
    }
}

/**
 * @brief Implements Q/QQ through ExecutionContext exit state.
 *
 * Normal Q refuses to exit while modified buffers exist; QQ skips that guard.
 */
void execute_quit(const QuitCommandNode& command, ExecutionContext& context) {
    if (!command.immediate()) {
        const auto modified = context.buffers().modified_names();
        if (!modified.empty()) {
            std::string names;
            for (const auto& name : modified) {
                if (!names.empty()) {
                    names += ", ";
                }
                names += name;
            }
            throw CommandExecutionError(
                "modified buffer(s): " + names + "; use W or QQ");
        }
    }
    context.request_exit(command.immediate());
}

/**
 * @brief Insert a stable snapshot of addressed lines after the target cursor.
 *
 * Unlike K, historical ZK preserves the target's existing content. Both
 * Buffer cursors point to their respective last copied line on success.
 * A same-Buffer copy cannot include its insertion cursor in the source range.
 */
void execute_zap_copy(const ZapCopyCommandNode& command,
                      ExecutionContext& context) {
    auto& source = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), source);

    if (command.buffer_name() == source.name() &&
        source.current_line() >= range.first &&
        source.current_line() <= range.last) {
        throw CommandExecutionError(
            "ZK cannot copy the current line into the same buffer");
    }

    // Snapshot before mutation: self-insertion must not alter source reads.
    std::vector<std::string> copied_lines;
    copied_lines.reserve(range.last - range.first + 1);
    for (auto line = range.first; line <= range.last; ++line) {
        copied_lines.push_back(source.line(line));
    }

    // get_or_create never changes the selected buffer or its MRU position.
    auto& destination = context.buffers().get_or_create(command.buffer_name());
    destination.insert_after(destination.current_line(),
                             std::move(copied_lines));
    if (&destination != &source) {
        source.set_current_line(range.last);
    }
}

void execute_zap_extended(const ZapExtendedCommandNode& command,
                          ExecutionContext& context) {
    auto& source = context.current_buffer();
    auto& buffers = context.buffers();
    const auto kind = command.operation();
    if (kind == ZapExtendedKind::ClearModified) {
        context.set_condition(source.modified());
        source.mark_clean();
        return;
    }
    if (kind == ZapExtendedKind::DeleteBuffer) {
        if (!buffers.contains(command.buffer_name())) {
            throw CommandExecutionError("ZD unknown buffer: " + command.buffer_name());
        }
        buffers.erase(command.buffer_name());
        return;
    }
    if (kind == ZapExtendedKind::PrintBuffer || kind == ZapExtendedKind::AppendBuffer) {
        if (!buffers.contains(command.buffer_name())) {
            throw CommandExecutionError("Z unknown buffer: " + command.buffer_name());
        }
        const auto& other = buffers.get(command.buffer_name());
        if (kind == ZapExtendedKind::PrintBuffer) {
            for (const auto& line : other.lines()) context.output().write_line(line);
        } else {
            auto after = source.current_line();
            if (const auto* address = command.address()) {
                if (address->kind() == AstNodeKind::AbsoluteAddress &&
                    static_cast<const AbsoluteAddressNode&>(*address).line() == 0) {
                    after = 0;
                } else {
                    const auto range = AddressEvaluator{}.evaluate(address, source);
                    if (range.first != range.last) {
                        throw CommandExecutionError("ZA accepts one line address");
                    }
                    after = range.first;
                }
            }
            // Copy before mutation, including when the source is the target.
            const std::vector<std::string> snapshot(other.lines());
            source.insert_after(after, snapshot);
        }
        return;
    }
    const auto range = AddressEvaluator{}.evaluate(command.address(), source);
    if (kind == ZapExtendedKind::CompressTabs ||
        kind == ZapExtendedKind::ExpandTabs) {
        const auto next_stop = [&](std::size_t column) -> std::size_t {
            for (const auto stop : context.tab_stops()) {
                if (stop > column + 1) return stop - 1;
            }
            // After the configured explicit stops, continue on 8-column
            // boundaries rather than producing an unbounded run of spaces.
            return ((column / 8) + 1) * 8;
        };
        std::int64_t modified = 0;
        for (auto line = range.first; line <= range.last; ++line) {
            const std::string original = source.line(line);
            std::string transformed;
            std::size_t column = 0;
            for (std::size_t pos = 0; pos < original.size();) {
                const char character = original[pos];
                if (kind == ZapExtendedKind::ExpandTabs && character == '\t') {
                    const auto target = next_stop(column);
                    const auto count = target > column ? target - column : 1;
                    transformed.append(count, ' ');
                    column += count;
                    ++pos;
                } else if (kind == ZapExtendedKind::CompressTabs &&
                           character == ' ') {
                    std::size_t until = pos;
                    while (until < original.size() && original[until] == ' ') ++until;
                    auto spaces = until - pos;
                    while (spaces > 0) {
                        const auto target = next_stop(column);
                        const auto span = target > column ? target - column : 1;
                        if (span >= 2 && spaces >= span) {
                            transformed.push_back('\t');
                            column += span;
                            spaces -= span;
                        } else {
                            transformed.push_back(' ');
                            ++column;
                            --spaces;
                        }
                    }
                    pos = until;
                } else {
                    transformed.push_back(character);
                    if (character == '\t') column = next_stop(column);
                    else column = character == '\r' ? 0 : column + 1;
                    ++pos;
                }
            }
            if (original != transformed) {
                source.replace(line, std::move(transformed));
                ++modified;
            }
        }
        context.set_counter(modified);
        return;
    }
    if (kind == ZapExtendedKind::MoveLines) {
        const bool same = command.buffer_name() == source.name();
        const auto insertion_cursor = source.current_line();
        if (same && insertion_cursor >= range.first && insertion_cursor <= range.last) {
            throw CommandExecutionError("ZM cannot move its current line into itself");
        }
        std::vector<std::string> lines;
        for (auto n = range.first; n <= range.last; ++n) lines.push_back(source.line(n));
        auto& target = buffers.get_or_create(command.buffer_name());
        if (same) {
            const auto after = insertion_cursor > range.last
                ? insertion_cursor - lines.size() : insertion_cursor;
            source.erase(range.first, range.last);
            source.insert_after(after, std::move(lines));
        } else {
            target.insert_after(target.current_line(), std::move(lines));
            source.erase(range.first, range.last);
        }
        return;
    }
    if (kind == ZapExtendedKind::SortAscending || kind == ZapExtendedKind::SortDescending) {
        std::vector<std::string> lines;
        for (auto n = range.first; n <= range.last; ++n) lines.push_back(source.line(n));
        const bool identical = std::adjacent_find(
            lines.begin(), lines.end(), std::not_equal_to<std::string>{}) == lines.end();
        if (kind == ZapExtendedKind::SortAscending) {
            std::sort(lines.begin(), lines.end());
        } else {
            std::sort(lines.begin(), lines.end(), std::greater<std::string>{});
        }
        const auto original_cursor = source.current_line();
        for (std::size_t i = 0; i < lines.size(); ++i) {
            if (source.line(range.first + i) != lines[i]) {
                source.replace(range.first + i, std::move(lines[i]));
            }
        }
        source.set_current_line(original_cursor);
        context.set_condition(identical);
        return;
    }

    std::vector<std::pair<std::size_t, std::string>> changed_lines;
    std::int64_t count = 0;
    PatternMatcher matcher(context.case_insensitive_search());
    for (auto n = range.first; n <= range.last; ++n) {
        const std::string original = source.line(n);
        std::string next = original;
        if (kind == ZapExtendedKind::Translate) {
            for (auto& ch : next) {
                const auto index = command.from().find(ch);
                if (index != std::string::npos && ch != command.to()[index]) {
                    ch = command.to()[index];
                    ++count;
                }
            }
        } else {
            const bool lower = kind == ZapExtendedKind::Lowercase;
            const auto transform = [&](std::size_t begin, std::size_t end) {
                bool changed = false;
                for (auto i = begin; i < end; ++i) {
                    const auto byte = static_cast<unsigned char>(next[i]);
                    if (byte < 128) {
                        const char replacement = static_cast<char>(lower
                            ? std::tolower(byte) : std::toupper(byte));
                        changed |= replacement != next[i];
                        next[i] = replacement;
                    }
                }
                return changed;
            };
            if (command.pattern()) {
                std::size_t offset = 0;
                while (offset <= original.size()) {
                    const auto match = matcher.find(*command.pattern(), original, offset);
                    if (!match) break;
                    if (transform(match->start, match->end)) ++count;
                    offset = match->end > match->start ? match->end : match->end + 1;
                }
            } else if (transform(0, next.size())) {
                ++count;
            }
        }
        if (next != original) changed_lines.emplace_back(n, std::move(next));
    }
    context.set_counter(count);
    if (changed_lines.empty() &&
        (kind == ZapExtendedKind::Lowercase || kind == ZapExtendedKind::Uppercase)) {
        throw CommandExecutionError("no text changed");
    }
    for (auto& [number, line] : changed_lines) source.replace(number, std::move(line));
}

/**
 * @brief Moves the current-line cursor to Z's single resolved line.
 *
 * Ranges are invalid. Z does not delete or overwrite any line and does not
 * alter the buffer's modified state.
 */
void execute_zap(const ZapCommandNode& command, ExecutionContext& context) {
    auto& buffer = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), buffer);
    if (range.first != range.last) {
        throw CommandExecutionError("Z accepts at most one line address");
    }
    buffer.set_current_line(range.first);
}

LineRange evaluate_global_range(const GlobalCommandNode& command,
                                const Buffer& buffer) {
    if (buffer.empty()) {
        throw CommandExecutionError("current buffer is empty");
    }
    if (command.address() == nullptr) {
        return {1, buffer.line_count()};
    }
    return AddressEvaluator{}.evaluate(command.address(), buffer);
}

/**
 * @brief Executes current G selection semantics over a stable logical range.
 *
 * Current Runtime support is intentionally limited to nested P/D/Z/S. This is
 * an implementation limit, not an architectural prohibition on future forms.
 */
void execute_global(const GlobalCommandNode& command,
                    ExecutionContext& context) {
    auto& buffer = context.current_buffer();
    auto range = evaluate_global_range(command, buffer);
    const auto& nested = command.nested_command();

    if (nested.has_address()) {
        throw CommandExecutionError(
            "addressed commands inside G are not supported yet");
    }
    if (nested.kind() != AstNodeKind::PrintCommand &&
        nested.kind() != AstNodeKind::DeleteCommand &&
        nested.kind() != AstNodeKind::ZapCommand &&
        nested.kind() != AstNodeKind::SubstituteCommand) {
        throw CommandExecutionError(
            "G currently supports nested P, D, Z and S commands");
    }

    PatternMatcher matcher(context.case_insensitive_search());
    std::size_t selected_count = 0;
    auto line = range.first;
    auto last = range.last;

    while (line <= last && !buffer.empty()) {
        const bool pattern_found = matcher.search(command.pattern(), buffer.line(line));
        const bool selected = command.inverted() ? !pattern_found : pattern_found;
        if (!selected) {
            ++line;
            continue;
        }

        ++selected_count;
        buffer.set_current_line(line);

        if (nested.kind() == AstNodeKind::PrintCommand) {
            context.output().write_line(buffer.line(line));
            ++line;
            continue;
        }
        if (nested.kind() == AstNodeKind::ZapCommand) {
            ++line;
            continue;
        }
        if (nested.kind() == AstNodeKind::SubstituteCommand) {
            execute_substitute(
                static_cast<const SubstituteCommandNode&>(nested), context);
            ++line;
            continue;
        }

        buffer.erase(line, line);
        --last;
        if (line > buffer.line_count()) {
            break;
        }
    }

    context.set_counter(selected_count);
}

/**
 * @brief Executes `!` through the host shell and forwards combined output.
 *
 * Condition reflects the shell process status. This is inherently platform
 * dependent front-facing behavior implemented at Runtime command level.
 */
void execute_system(const SystemCommandNode& command,
                    ExecutionContext& context) {
    std::string shell_command = command.command();
    shell_command += " 2>&1";

#ifdef _WIN32
    FILE* pipe = _popen(shell_command.c_str(), "r");
#else
    FILE* pipe = popen(shell_command.c_str(), "r");
#endif
    if (pipe == nullptr) {
        throw CommandExecutionError("cannot start system command");
    }

    std::array<char, 4096> chunk{};
    while (std::fgets(chunk.data(),
                      static_cast<int>(chunk.size()), pipe) != nullptr) {
        context.output().write(std::string_view(chunk.data()));
    }

#ifdef _WIN32
    const int status = _pclose(pipe);
#else
    const int status = pclose(pipe);
#endif
    context.set_condition(status == 0);
}

std::vector<std::string> captured_lines(std::string_view text) {
    std::vector<std::string> lines;
    std::istringstream input{std::string(text)};
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(std::move(line));
    }
    return lines;
}

/**
 * @brief Captures nested command Output and appends it to the destination Buffer.
 *
 * The previous Output sink is restored on both success and exceptions; Output
 * ownership never transfers to ExecutionContext.
 */
void execute_zap_gather(const ZapGatherCommandNode& command,
                        ExecutionContext& context) {
    StringOutput captured;
    Output& previous = context.exchange_output(captured);

    try {
        CommandExecutor{}.execute(command.nested_command(), context);
    } catch (...) {
        (void)context.exchange_output(previous);
        throw;
    }
    (void)context.exchange_output(previous);

    auto lines = captured_lines(captured.content());
    if (lines.empty()) {
        return;
    }

    auto& destination =
        context.buffers().get_or_create(command.buffer_name());

    if (destination.empty()) {
        for (auto& line : lines) {
            destination.append(std::move(line));
        }
        return;
    }

    destination.insert_after(destination.current_line(), std::move(lines));
}

/** @brief Ignores the rest-of-line historical `"` comment at Runtime. */
void execute_comment(const CommentCommandNode&,
                     ExecutionContext&) noexcept {
    // Historical FRED comments are intentionally a runtime no-op.
}

/**
 * @brief Writes JM with a trailing newline or JP without one.
 *
 * Both spellings enforce the historical 2000-character expanded-message limit
 * and do not modify any editor Buffer.
 */
void execute_message(const MessageCommandNode& command,
                     ExecutionContext& context) {
    const std::string& expanded = command.message();

    if (expanded.size() > 2000) {
        throw CommandExecutionError(
            "expanded J message exceeds historical 2000-character limit");
    }

    if (command.newline()) {
        context.output().write_line(expanded);
    } else {
        context.output().write(expanded);
    }
    if (command.user_error()) {
        context.set_last_error(expanded);
    }
    if (command.user_error() && context.quit_on_error()) {
        context.request_exit(true);
    }
}

/**
 * @brief Selects or creates the B destination without modifying its text.
 *
 * A compact one-character B form is refused when O+I( requires parentheses;
 * the normal parenthesized form is unaffected by that option.
 */
void execute_buffer(const BufferCommandNode& command, ExecutionContext& context) {
    if (command.short_form() && context.input_parentheses_required()) {
        throw CommandExecutionError("buff/reg name invalid");
    }

    try {
        (void)context.buffers().create_or_select(command.buffer_name());
    } catch (const std::exception& error) {
        throw CommandExecutionError(error.what());
    }
}

struct RepeatBreak final {};
struct RepeatContinue final {};

/** Executes a bounded U command body through the normal AST parser. */
void execute_repeat(const RepeatCommandNode& command, ExecutionContext& context) {
    // An accidental recursive U definition must not exhaust the native stack.
    static thread_local std::size_t active_depth = 0;
    if (active_depth >= 16) {
        throw CommandExecutionError("U nesting exceeds 16 levels");
    }
    struct DepthGuard {
        std::size_t& depth;
        explicit DepthGuard(std::size_t& value) : depth(value) { ++depth; }
        ~DepthGuard() { --depth; }
    } guard(active_depth);
    const auto limit = command.limit() == 0 ? std::size_t{10000} : command.limit();
    const auto registry = make_core_command_registry();
    const CommandExecutor executor;
    for (std::size_t iteration = 0; iteration < limit; ++iteration) {
        bool failed = false;
        try {
            const auto expanded = context.expand_named_patterns(command.body());
            Lexer lexer(expanded);
            TokenStream tokens(lexer);
            CommandParser parser(tokens, registry, &context);
            while (!tokens.eof() && !context.exit_requested()) {
                const auto node = parser.parse_one();
                if (node->kind() == AstNodeKind::RepeatControlCommand) {
                    const auto& control = static_cast<const RepeatControlCommandNode&>(*node);
                    const bool active = control.condition() == 'A' ||
                        (control.condition() == 'T' && context.condition()) ||
                        (control.condition() == 'F' && !context.condition());
                    if (active && control.breaks()) throw RepeatBreak{};
                    if (active) throw RepeatContinue{};
                } else {
                    executor.execute(*node, context);
                }
            }
        } catch (const RepeatBreak&) {
            return;
        } catch (const RepeatContinue&) {
            // Continue is evaluated before the loop's stop condition below.
        } catch (const std::exception& error) {
            if (command.until() != 'E') throw;
            context.set_last_error(error.what());
            failed = true;
        }
        if (failed || context.exit_requested() ||
            (command.until() == 'T' && context.condition()) ||
            (command.until() == 'F' && !context.condition())) return;
    }
    if (command.limit() == 0) {
        throw CommandExecutionError("U exceeded the portable 10000-iteration safety limit");
    }
}

} // namespace

/**
 * @brief Dispatches one already-parsed command AST to its Runtime implementation.
 *
 * A/I/C deliberately require the specialized text-input entry points because
 * front ends/procedures own text collection.
 */
void CommandExecutor::execute(const CommandNode& command,
                              ExecutionContext& context) const {
    try {
    switch (command.kind()) {
    case AstNodeKind::LineNumberCommand:
        execute_line_number(static_cast<const LineNumberCommandNode&>(command),
                            context);
        return;
    case AstNodeKind::CounterCommand:
        execute_counter(context);
        return;
    case AstNodeKind::WindowCommand:
        execute_window(static_cast<const WindowCommandNode&>(command), context);
        return;
    case AstNodeKind::LabelCommand:
        // In a procedure, the runner resolves @(label) before normal dispatch.
        return;
    case AstNodeKind::WaitCommand:
        std::this_thread::sleep_for(std::chrono::seconds(
            static_cast<const WaitCommandNode&>(command).seconds()));
        return;
    case AstNodeKind::WhyCommand:
        execute_why(context);
        return;
    case AstNodeKind::NamedPatternCommand: {
        const auto& definition = static_cast<const NamedPatternCommandNode&>(command);
        context.record_undo();
        context.define_pattern(definition.name(), definition.pattern());
        return;
    }
    case AstNodeKind::RepeatCommand:
        execute_repeat(static_cast<const RepeatCommandNode&>(command), context);
        return;
    case AstNodeKind::RepeatControlCommand:
        throw CommandExecutionError("JB/JO require an active U loop");
    case AstNodeKind::UndoCommand:
        context.undo();
        return;
    case AstNodeKind::RedoCommand:
        context.redo();
        return;
    case AstNodeKind::PrintCommand:
        execute_print(static_cast<const PrintCommandNode&>(command), context);
        return;
    case AstNodeKind::ListCommand:
        execute_list(static_cast<const ListCommandNode&>(command), context);
        return;
    case AstNodeKind::ReadCommand:
        context.record_undo();
        execute_read(static_cast<const ReadCommandNode&>(command), context);
        return;
    case AstNodeKind::WriteCommand:
        execute_write(static_cast<const WriteCommandNode&>(command), context);
        return;
    case AstNodeKind::DeleteCommand:
        context.record_undo();
        execute_delete(static_cast<const DeleteCommandNode&>(command), context);
        return;
    case AstNodeKind::AppendCommand:
        throw CommandExecutionError("A requires text input mode");
    case AstNodeKind::InsertCommand:
        throw CommandExecutionError("I requires text input mode");
    case AstNodeKind::ChangeCommand:
        throw CommandExecutionError("C requires text input mode");
    case AstNodeKind::MoveCommand:
        context.record_undo();
        execute_move(static_cast<const MoveCommandNode&>(command), context);
        return;
    case AstNodeKind::CopyCommand:
        context.record_undo();
        execute_copy(static_cast<const CopyCommandNode&>(command), context);
        return;
    case AstNodeKind::TestCommand:
        execute_test(static_cast<const TestCommandNode&>(command), context);
        return;
    case AstNodeKind::BufferCommand:
        context.record_undo();
        execute_buffer(static_cast<const BufferCommandNode&>(command), context);
        return;
    case AstNodeKind::GlobalCommand:
        context.record_undo();
        execute_global(static_cast<const GlobalCommandNode&>(command), context);
        return;
    case AstNodeKind::ZapCommand:
        execute_zap(static_cast<const ZapCommandNode&>(command), context);
        return;
    case AstNodeKind::ZapCopyCommand:
        context.record_undo();
        execute_zap_copy(static_cast<const ZapCopyCommandNode&>(command), context);
        return;
    case AstNodeKind::ZapGatherCommand:
        context.record_undo();
        execute_zap_gather(
            static_cast<const ZapGatherCommandNode&>(command), context);
        return;
    case AstNodeKind::ZapExtendedCommand:
        context.record_undo();
        execute_zap_extended(static_cast<const ZapExtendedCommandNode&>(command), context);
        return;
    case AstNodeKind::SystemCommand:
        execute_system(static_cast<const SystemCommandNode&>(command), context);
        return;
    case AstNodeKind::SubstituteCommand:
        context.record_undo();
        execute_substitute(static_cast<const SubstituteCommandNode&>(command),
                           context);
        return;
    case AstNodeKind::CommentCommand:
        execute_comment(static_cast<const CommentCommandNode&>(command), context);
        return;
    case AstNodeKind::MessageCommand:
        execute_message(static_cast<const MessageCommandNode&>(command), context);
        return;
    case AstNodeKind::FactsCommand:
        execute_facts(static_cast<const FactsCommandNode&>(command), context);
        return;
    case AstNodeKind::NumericAssignmentCommand:
        context.record_undo();
        execute_numeric_assignment(
            static_cast<const NumericAssignmentCommandNode&>(command), context);
        return;
    case AstNodeKind::NumericOperationCommand:
        context.record_undo();
        execute_numeric_operation(
            static_cast<const NumericOperationCommandNode&>(command), context);
        return;
    case AstNodeKind::FileAssociationCommand:
        context.record_undo();
        execute_file_association(
            static_cast<const FileAssociationCommandNode&>(command), context);
        return;
    case AstNodeKind::OptionCommand:
        execute_option(static_cast<const OptionCommandNode&>(command), context);
        return;
    case AstNodeKind::QuitCommand:
        execute_quit(static_cast<const QuitCommandNode&>(command), context);
        return;
    default:
        throw CommandExecutionError("unsupported command node");
    }
    } catch (const std::exception& error) {
        context.set_last_error(error.what());
        throw;
    }
}

/** @brief Completes an already-parsed A command with collected text lines. */
void CommandExecutor::execute_append(const AppendCommandNode& command,
                                     ExecutionContext& context,
                                     std::vector<std::string> lines) const {
    auto& buffer = context.current_buffer();
    const auto after = evaluate_append_address(command, buffer);
    context.record_undo();
    buffer.insert_after(after, std::move(lines));
}

/** @brief Completes an already-parsed I command with collected text lines. */
void CommandExecutor::execute_insert(const InsertCommandNode& command,
                                     ExecutionContext& context,
                                     std::vector<std::string> lines) const {
    auto& buffer = context.current_buffer();
    const auto after = evaluate_insert_position(command, buffer);
    context.record_undo();
    buffer.insert_after(after, std::move(lines));
}

/**
 * @brief Completes C by erasing the addressed range then inserting collected text.
 */
void CommandExecutor::execute_change(const ChangeCommandNode& command,
                                     ExecutionContext& context,
                                     std::vector<std::string> lines) const {
    auto& buffer = context.current_buffer();
    const auto range = AddressEvaluator{}.evaluate(command.address(), buffer);
    const auto insertion_after = range.first - 1;

    context.record_undo();
    buffer.erase(range.first, range.last);
    if (!lines.empty()) {
        buffer.insert_after(insertion_after, std::move(lines));
    }
}

} // namespace fred
