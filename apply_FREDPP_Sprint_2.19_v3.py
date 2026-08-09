#!/usr/bin/env python3
from __future__ import annotations
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(sys.argv[1] if len(sys.argv) > 1 else ".").resolve()
EXPECTED_BASE = "d617d0b"
FILES = {}
ERRORS = []

def p(rel):
    return ROOT / rel

def read(rel):
    q = p(rel)
    if not q.is_file():
        raise RuntimeError(f"fichier absent : {rel}")
    return q.read_text(encoding="utf-8").replace("\r\n", "\n")

def replace_once(rel, old, new, marker):
    text = FILES[rel]
    if marker in text:
        print(f"PRECHECK déjà OK : {rel} :: {marker}")
        return
    count = text.count(old)
    if count != 1:
        ERRORS.append(f"{rel}: contexte trouvé {count} fois au lieu de 1 :: marqueur {marker!r}")
        return
    FILES[rel] = text.replace(old, new, 1)
    print(f"PRECHECK OK      : {rel} :: {marker}")

def insert_before(rel, anchor, block, marker):
    text = FILES[rel]
    if marker in text:
        print(f"PRECHECK déjà OK : {rel} :: {marker}")
        return
    count = text.count(anchor)
    if count != 1:
        ERRORS.append(f"{rel}: ancre trouvée {count} fois au lieu de 1 :: marqueur {marker!r}")
        return
    FILES[rel] = text.replace(anchor, block + anchor, 1)
    print(f"PRECHECK OK      : {rel} :: {marker}")

def ensure_new(rel, content, marker):
    q = p(rel)
    if q.exists():
        current = q.read_text(encoding="utf-8").replace("\r\n", "\n")
        if marker in current:
            FILES[rel] = current
            print(f"PRECHECK déjà OK : {rel} :: {marker}")
            return
        ERRORS.append(f"{rel}: existe déjà avec un contenu inattendu")
        return
    FILES[rel] = content
    print(f"PRECHECK OK      : {rel} :: création")

def git(*args):
    r = subprocess.run(["git", *args], cwd=ROOT, text=True,
                       stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if r.returncode != 0:
        raise RuntimeError(r.stderr.strip())
    return r.stdout.strip()

head = git("rev-parse", "--short=7", "HEAD")
if head != EXPECTED_BASE:
    raise RuntimeError(f"base Git inattendue : {head}; attendu : {EXPECTED_BASE}")
if subprocess.run(["git","diff","--quiet"], cwd=ROOT).returncode != 0:
    raise RuntimeError("des fichiers suivis sont déjà modifiés")
if subprocess.run(["git","diff","--cached","--quiet"], cwd=ROOT).returncode != 0:
    raise RuntimeError("des changements sont déjà indexés")

required = ['CMakeLists.txt', 'include/fred/ast/AstNode.hpp', 'include/fred/ast/CommandNode.hpp', 'include/fred/core/BufferManager.hpp', 'include/fred/runtime/ExecutionContext.hpp', 'src/core/BufferManager.cpp', 'src/parser/CommandParser.cpp', 'src/runtime/ExecutionContext.cpp', 'src/runtime/CommandExecutor.cpp', 'tests/CMakeLists.txt', 'tests/test_command_parser.cpp', 'tests/test_help_manager.cpp', 'docs/project/COMMAND_STATUS.md', 'ROADMAP.md', 'CHANGELOG.md', 'RELEASE_NOTES.md', 'docs/fr/reference/commandes/bang.md', 'docs/fr/reference/commandes/zg.md']
for rel in required:
    FILES[rel] = read(rel)

if "# !" not in FILES["docs/fr/reference/commandes/bang.md"]:
    ERRORS.append("bang.md historique inattendu")
if "# ZG" not in FILES["docs/fr/reference/commandes/zg.md"]:
    ERRORS.append("zg.md historique inattendu")

replace_once('CMakeLists.txt', 'project(FREDPP VERSION 0.0.13 LANGUAGES CXX)\n', 'project(FREDPP VERSION 0.0.14 LANGUAGES CXX)\n', 'project(FREDPP VERSION 0.0.14')
replace_once('CMakeLists.txt', '    w\n    z\n)\n', '    w\n    z\n    zg\n)\n', '    zg\n)')
replace_once('CMakeLists.txt', 'math(EXPR FREDPP_EMBEDDED_HELP_COUNT "${FREDPP_HELP_FILES_LENGTH} + 8")\n', 'math(EXPR FREDPP_EMBEDDED_HELP_COUNT "${FREDPP_HELP_FILES_LENGTH} + 9")\n', '${FREDPP_HELP_FILES_LENGTH} + 9')
replace_once(
    'CMakeLists.txt',
    '"    ?wu         Afficher l\'extension UTF-8 de la commande W\\n"\n',
    '"    ?wu         Afficher l\'extension UTF-8 de la commande W\\n"\n'
    '"    ?!          Afficher l\'aide historique de la commande !\\n"\n',
    "?!          Afficher l'aide historique"
)
insert_before('CMakeLists.txt', 'set(FREDPP_QUOTE_HELP_FILE\n', 'set(FREDPP_BANG_HELP_FILE\n    "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/reference/commandes/bang.md"\n)\nif(NOT EXISTS "${FREDPP_BANG_HELP_FILE}")\n    message(FATAL_ERROR "Missing historical bang help: ${FREDPP_BANG_HELP_FILE}")\nendif()\nfile(READ "${FREDPP_BANG_HELP_FILE}" FREDPP_BANG_HELP_CONTENT)\nstring(FIND "${FREDPP_BANG_HELP_CONTENT}" ")FREDPP_HELP\\"" FREDPP_BANG_HELP_COLLISION)\nif(NOT FREDPP_BANG_HELP_COLLISION EQUAL -1)\n    message(FATAL_ERROR "Bang help contains the reserved raw-string delimiter")\nendif()\n\n', 'FREDPP_BANG_HELP_FILE')
replace_once('CMakeLists.txt', '"    Entry{\\":\\", R\\"FREDPP_HELP(${FREDPP_SPECIAL_HELP_CONTENT})FREDPP_HELP\\"},\\n"\n"    Entry{\\"*\\", R\\"FREDPP_HELP(${FREDPP_STAR_ALIAS_HELP_CONTENT})FREDPP_HELP\\"},\\n"\n', '"    Entry{\\":\\", R\\"FREDPP_HELP(${FREDPP_SPECIAL_HELP_CONTENT})FREDPP_HELP\\"},\\n"\n"    Entry{\\"!\\", R\\"FREDPP_HELP(${FREDPP_BANG_HELP_CONTENT})FREDPP_HELP\\"},\\n"\n"    Entry{\\"*\\", R\\"FREDPP_HELP(${FREDPP_STAR_ALIAS_HELP_CONTENT})FREDPP_HELP\\"},\\n"\n', 'Entry{\\"!\\"')
replace_once('include/fred/ast/AstNode.hpp', '    GlobalCommand,\n    ZapCommand,\n    SubstituteCommand,\n', '    GlobalCommand,\n    ZapCommand,\n    ZapGatherCommand,\n    SystemCommand,\n    SubstituteCommand,\n', 'ZapGatherCommand')
insert_before('include/fred/ast/CommandNode.hpp', 'class SubstituteCommandNode final : public CommandNode {\n', 'class ZapGatherCommandNode final : public CommandNode {\npublic:\n    ZapGatherCommandNode(std::string buffer_name,\n                         std::unique_ptr<CommandNode> nested_command,\n                         SourceLocation location) noexcept\n        : CommandNode(nullptr, location),\n          buffer_name_(std::move(buffer_name)),\n          nested_command_(std::move(nested_command)) {}\n\n    [[nodiscard]] AstNodeKind kind() const noexcept override {\n        return AstNodeKind::ZapGatherCommand;\n    }\n\n    [[nodiscard]] const std::string& buffer_name() const noexcept {\n        return buffer_name_;\n    }\n\n    [[nodiscard]] const CommandNode& nested_command() const noexcept {\n        return *nested_command_;\n    }\n\nprivate:\n    std::string buffer_name_;\n    std::unique_ptr<CommandNode> nested_command_;\n};\n\nclass SystemCommandNode final : public CommandNode {\npublic:\n    SystemCommandNode(std::string command,\n                      SourceLocation location) noexcept\n        : CommandNode(nullptr, location), command_(std::move(command)) {}\n\n    [[nodiscard]] AstNodeKind kind() const noexcept override {\n        return AstNodeKind::SystemCommand;\n    }\n\n    [[nodiscard]] const std::string& command() const noexcept {\n        return command_;\n    }\n\nprivate:\n    std::string command_;\n};\n\n', 'class ZapGatherCommandNode')
replace_once('include/fred/core/BufferManager.hpp', '    Buffer& create_or_select(std::string name);\n    Buffer& select(std::string_view name);\n', '    Buffer& create_or_select(std::string name);\n    Buffer& get_or_create(std::string name);\n    Buffer& select(std::string_view name);\n', 'get_or_create(std::string name)')
insert_before('src/core/BufferManager.cpp', 'Buffer& BufferManager::select(std::string_view name) {\n', 'Buffer& BufferManager::get_or_create(std::string name) {\n    if (name.empty()) {\n        throw std::invalid_argument("buffer name must not be empty");\n    }\n    if (name.size() > limits::max_buffer_name_length) {\n        throw std::invalid_argument(\n            "buffer name exceeds historical limit of " +\n            std::to_string(limits::max_buffer_name_length) + " characters");\n    }\n\n    auto [it, inserted] = buffers_.try_emplace(name, nullptr);\n    if (inserted) {\n        it->second = std::make_unique<Buffer>(name);\n    }\n    return *it->second;\n}\n\n', 'BufferManager::get_or_create')
replace_once('include/fred/runtime/ExecutionContext.hpp', '    [[nodiscard]] Output& output() noexcept;\n\n    void set_counter(std::size_t value) noexcept;\n', '    [[nodiscard]] Output& output() noexcept;\n    [[nodiscard]] Output& exchange_output(Output& output) noexcept;\n\n    void set_counter(std::size_t value) noexcept;\n', 'exchange_output(Output& output)')
insert_before('src/runtime/ExecutionContext.cpp', 'void ExecutionContext::set_counter(std::size_t value) noexcept {\n', 'Output& ExecutionContext::exchange_output(Output& output) noexcept {\n    Output* previous = output_;\n    output_ = &output;\n    return *previous;\n}\n\n', 'ExecutionContext::exchange_output')
replace_once('src/parser/CommandParser.cpp', '        bool immediate_quit = false;\n        FileWriteMode write_mode = FileWriteMode::Preserve;\n', '        bool immediate_quit = false;\n        bool zap_gather = false;\n        FileWriteMode write_mode = FileWriteMode::Preserve;\n', 'bool zap_gather = false')
replace_once('src/parser/CommandParser.cpp', '            } else if (first == \'B\') {\n                compact_buffer_name = std::string(1, command.lexeme[1]);\n                command.type = TokenType::Command;\n                command.lexeme = "B";\n            }\n        }\n', '            } else if (first == \'B\') {\n                compact_buffer_name = std::string(1, command.lexeme[1]);\n                command.type = TokenType::Command;\n                command.lexeme = "B";\n            } else if (first == \'Z\' && second == \'G\') {\n                zap_gather = true;\n                command.type = TokenType::Command;\n                command.lexeme = "Z";\n            }\n        }\n', "first == 'Z' && second == 'G'")
insert_before('src/parser/CommandParser.cpp', '        if (command.type == TokenType::Symbol && command.lexeme == "\\"") {\n', '        if (command.type == TokenType::Symbol && command.lexeme == "!") {\n            if (address) {\n                throw ParseError("! does not accept a line address",\n                                 address->location());\n            }\n            auto system_command = consume_plain_text(*tokens_);\n            if (system_command.empty()) {\n                throw ParseError("! requires a system command",\n                                 command.location);\n            }\n            require_command_end();\n            return std::make_unique<SystemCommandNode>(\n                std::move(system_command), command.location);\n        }\n', 'command.lexeme == "!"')
replace_once('src/parser/CommandParser.cpp', '        if (mnemonic == \'Z\') {\n            if (address && address->kind() == AstNodeKind::RangeAddress) {\n                throw ParseError("Z accepts at most one line address",\n                                 address->location());\n            }\n            require_command_end();\n            return std::make_unique<ZapCommandNode>(\n                std::move(address), command.location);\n        }\n', '        if (mnemonic == \'Z\') {\n            if (zap_gather) {\n                if (address) {\n                    throw ParseError("ZG does not accept a line address",\n                                     address->location());\n                }\n                if (tokens_->peek().type != TokenType::LeftParenthesis) {\n                    throw ParseError("ZG requires a destination buffer",\n                                     tokens_->peek().location);\n                }\n\n                std::string buffer_name = parse_parenthesized_buffer_name();\n\n                if (tokens_->peek().type == TokenType::End ||\n                    tokens_->peek().type == TokenType::NewLine) {\n                    throw ParseError("ZG requires a command",\n                                     tokens_->peek().location);\n                }\n\n                CommandParser nested_parser(*tokens_, *registry_);\n                auto nested_command = nested_parser.parse();\n                if (nested_command->has_address()) {\n                    throw ParseError(\n                        "addressed commands inside ZG are not supported yet",\n                        nested_command->location());\n                }\n\n                return std::make_unique<ZapGatherCommandNode>(\n                    std::move(buffer_name),\n                    std::move(nested_command),\n                    command.location);\n            }\n\n            if (address && address->kind() == AstNodeKind::RangeAddress) {\n                throw ParseError("Z accepts at most one line address",\n                                 address->location());\n            }\n            require_command_end();\n            return std::make_unique<ZapCommandNode>(\n                std::move(address), command.location);\n        }\n', 'ZG requires a destination buffer')
replace_once('src/runtime/CommandExecutor.cpp', '#include <algorithm>\n#include <cstdint>\n#include <fstream>\n', '#include <algorithm>\n#include <array>\n#include <cstdio>\n#include <cstdint>\n#include <fstream>\n#include <sstream>\n', '#include <array>')
insert_before('src/runtime/CommandExecutor.cpp', 'void execute_comment(const CommentCommandNode&,\n', 'void execute_system(const SystemCommandNode& command,\n                    ExecutionContext& context) {\n    std::string shell_command = command.command();\n    shell_command += " 2>&1";\n\n#ifdef _WIN32\n    FILE* pipe = _popen(shell_command.c_str(), "r");\n#else\n    FILE* pipe = popen(shell_command.c_str(), "r");\n#endif\n    if (pipe == nullptr) {\n        throw CommandExecutionError("cannot start system command");\n    }\n\n    std::array<char, 4096> chunk{};\n    while (std::fgets(chunk.data(),\n                      static_cast<int>(chunk.size()), pipe) != nullptr) {\n        context.output().write(std::string_view(chunk.data()));\n    }\n\n#ifdef _WIN32\n    const int status = _pclose(pipe);\n#else\n    const int status = pclose(pipe);\n#endif\n    context.set_condition(status == 0);\n}\n\nstd::vector<std::string> captured_lines(std::string_view text) {\n    std::vector<std::string> lines;\n    std::istringstream input{std::string(text)};\n    std::string line;\n    while (std::getline(input, line)) {\n        if (!line.empty() && line.back() == \'\\r\') {\n            line.pop_back();\n        }\n        lines.push_back(std::move(line));\n    }\n    return lines;\n}\n\nvoid execute_zap_gather(const ZapGatherCommandNode& command,\n                        ExecutionContext& context) {\n    StringOutput captured;\n    Output& previous = context.exchange_output(captured);\n\n    try {\n        CommandExecutor{}.execute(command.nested_command(), context);\n    } catch (...) {\n        (void)context.exchange_output(previous);\n        throw;\n    }\n    (void)context.exchange_output(previous);\n\n    auto lines = captured_lines(captured.content());\n    if (lines.empty()) {\n        return;\n    }\n\n    auto& destination =\n        context.buffers().get_or_create(command.buffer_name());\n\n    if (destination.empty()) {\n        for (auto& line : lines) {\n            destination.append(std::move(line));\n        }\n        return;\n    }\n\n    destination.insert_after(destination.current_line(), std::move(lines));\n}\n\n', 'execute_zap_gather')
replace_once('src/runtime/CommandExecutor.cpp', '    case AstNodeKind::ZapCommand:\n        execute_zap(static_cast<const ZapCommandNode&>(command), context);\n        return;\n    case AstNodeKind::SubstituteCommand:\n', '    case AstNodeKind::ZapCommand:\n        execute_zap(static_cast<const ZapCommandNode&>(command), context);\n        return;\n    case AstNodeKind::ZapGatherCommand:\n        execute_zap_gather(\n            static_cast<const ZapGatherCommandNode&>(command), context);\n        return;\n    case AstNodeKind::SystemCommand:\n        execute_system(static_cast<const SystemCommandNode&>(command), context);\n        return;\n    case AstNodeKind::SubstituteCommand:\n', 'case AstNodeKind::ZapGatherCommand:')
insert_before('tests/test_command_parser.cpp', '    {\n        const auto node = parse("2Z");\n', '    {\n        const auto node = parse("!echo FREDPP");\n        assert(node->kind() == fred::AstNodeKind::SystemCommand);\n        const auto& system =\n            static_cast<const fred::SystemCommandNode&>(*node);\n        assert(system.command() == "echo FREDPP");\n    }\n    {\n        const auto node = parse("ZG(capture)!echo FREDPP");\n        assert(node->kind() == fred::AstNodeKind::ZapGatherCommand);\n        const auto& gather =\n            static_cast<const fred::ZapGatherCommandNode&>(*node);\n        assert(gather.buffer_name() == "capture");\n        assert(gather.nested_command().kind() ==\n               fred::AstNodeKind::SystemCommand);\n    }\n', 'ZG(capture)!echo FREDPP')
replace_once('tests/test_command_parser.cpp', '    expect_error("1,2Z", "Z accepts at most one line address");\n', '    expect_error("1,2Z", "Z accepts at most one line address");\n    expect_error("!", "! requires a system command");\n    expect_error("1!echo bad", "! does not accept a line address");\n    expect_error("ZG", "ZG requires a destination buffer");\n    expect_error("ZG(capture)", "ZG requires a command");\n', 'expect_error("ZG(capture)", "ZG requires a command")')
ensure_new('tests/test_system_zg.cpp', '#include "fred/ast/CommandNode.hpp"\n#include "fred/command/CommandRegistry.hpp"\n#include "fred/core/BufferManager.hpp"\n#include "fred/lexer/Lexer.hpp"\n#include "fred/lexer/TokenStream.hpp"\n#include "fred/parser/CommandParser.hpp"\n#include "fred/runtime/CommandExecutor.hpp"\n#include "fred/runtime/ExecutionContext.hpp"\n#include "fred/runtime/Output.hpp"\n\n#include <cassert>\n#include <cstdlib>\n#include <iostream>\n#include <memory>\n#include <string>\n#include <string_view>\n\nnamespace {\n\nstd::unique_ptr<fred::CommandNode> parse(\n    std::string_view source,\n    const fred::CommandRegistry& registry) {\n    fred::Lexer lexer(source);\n    fred::TokenStream tokens(lexer);\n    fred::CommandParser parser(tokens, registry);\n    return parser.parse();\n}\n\n} // namespace\n\nint main() {\n    fred::BufferManager buffers;\n    fred::StringOutput output;\n    fred::ExecutionContext context(buffers, output);\n    fred::CommandExecutor executor;\n    const auto registry = fred::make_core_command_registry();\n\n    {\n        const auto command = parse("!echo FREDPP_SYSTEM", registry);\n        executor.execute(*command, context);\n        assert(output.content().find("FREDPP_SYSTEM") != std::string::npos);\n        assert(context.condition());\n    }\n\n    output.clear();\n\n    {\n        const auto command =\n            parse("ZG(capture)!echo FREDPP_ZG", registry);\n        executor.execute(*command, context);\n        assert(output.empty());\n        assert(buffers.contains("capture"));\n\n        const auto& capture = buffers.get("capture");\n        assert(!capture.empty());\n\n        bool found = false;\n        for (const auto& line : capture.lines()) {\n            if (line.find("FREDPP_ZG") != std::string::npos) {\n                found = true;\n                break;\n            }\n        }\n        assert(found);\n    }\n\n    {\n        const auto command = parse("JM/RESTORED/", registry);\n        executor.execute(*command, context);\n        assert(output.content() == "RESTORED\\n");\n    }\n\n    std::cout << "System command and ZG tests passed.\\n";\n    return EXIT_SUCCESS;\n}\n', 'System command and ZG tests passed.')
replace_once('tests/CMakeLists.txt', 'fredpp_add_test(test_procedure_runner test_procedure_runner.cpp)\n', 'fredpp_add_test(test_procedure_runner test_procedure_runner.cpp)\nfredpp_add_test(test_system_zg test_system_zg.cpp)\n', 'test_system_zg')
replace_once('tests/test_help_manager.cpp', '    assert(help.exists("procedure"));\n', '    assert(help.exists("procedure"));\n    assert(help.exists("!"));\n    assert(help.exists("zg"));\n', 'help.exists("zg")')
replace_once('tests/test_help_manager.cpp', '    assert(std::find(topics.begin(), topics.end(), "procedure") != topics.end());\n', '    assert(std::find(topics.begin(), topics.end(), "procedure") != topics.end());\n    assert(std::find(topics.begin(), topics.end(), "!") != topics.end());\n    assert(std::find(topics.begin(), topics.end(), "zg") != topics.end());\n', 'topics.end(), "zg")')
insert_before('docs/project/COMMAND_STATUS.md', '## Limites actuelles de G\n', "## Commandes `!` et `ZG` — Sprint 2.19\n\nLa commande historique `!` exécute une commande du système hôte. Sa sortie\nstandard et sa sortie d'erreur passent par la sortie FREDPP et peuvent être\ncapturées par `ZG`.\n\n`ZG(buffer)<commande>` exécute la commande imbriquée et place la production\ndestinée à la visualisation dans le buffer désigné, après sa ligne courante.\nSi le buffer n'existe pas, FREDPP le crée.\n\nExemple Windows :\n\n```fred\nzg(buf)!dir\n```\n\nSous Linux/Raspberry Pi OS, l'équivalent peut utiliser `!ls`.\n\n", '## Commandes `!` et `ZG` — Sprint 2.19')
insert_before('ROADMAP.md', '## Prochains jalons\n', "## Version v0.0.14 — Sprint 2.19\n\n### Modifications intégrées\n\n- commande historique `!` pour appeler le shell du système hôte ;\n- sortie de `!` routée par l'abstraction `Output` de FREDPP ;\n- commande historique `ZG(buffer)<commande>` ;\n- capture générique de la production destinée à la visualisation ;\n- insertion de cette production après la ligne courante du buffer destination ;\n- création du buffer destination par `ZG` lorsqu'il n'existe pas ;\n- aides historiques `?!` et `?zg` embarquées sans modifier les références.\n\n", '## Version v0.0.14 — Sprint 2.19')
insert_before('CHANGELOG.md', '## v0.0.13\n', '## v0.0.14\n\n### Added\n\n- Commande historique `!` pour exécuter une commande du système hôte.\n- Commande historique `ZG(buffer)<commande>` pour capturer la production\n  destinée à la visualisation dans un buffer.\n- Aides embarquées `?!` et `?zg`.\n- Test multiplateforme `test_system_zg`.\n\n### Changed\n\n- `ExecutionContext` permet une redirection temporaire de `Output`.\n- `BufferManager` peut créer un buffer sans changer le buffer courant.\n\n', '## v0.0.14')
insert_before('RELEASE_NOTES.md', '## Sprint 2.18', '## Sprint 2.19 — Commandes `!` et `ZG`\n\n- `!<commande>` appelle le shell du système hôte ;\n- la production de la commande passe par `Output` ;\n- `ZG(buffer)<commande>` capture cette production dans un buffer ;\n- `zg(buf)!dir` est pris en charge sous Windows ;\n- le même mécanisme permet `!ls` sous Linux/Raspberry Pi OS ;\n- aides historiques `?!` et `?zg`.\n\n', '## Sprint 2.19 — Commandes `!` et `ZG`')

must_have = [
    ("CMakeLists.txt","project(FREDPP VERSION 0.0.14"),
    ("CMakeLists.txt","    zg\n)"),
    ("CMakeLists.txt","FREDPP_BANG_HELP_FILE"),
    ("CMakeLists.txt",'Entry{\\"!\\"'),
    ("include/fred/ast/AstNode.hpp","ZapGatherCommand"),
    ("include/fred/ast/AstNode.hpp","SystemCommand"),
    ("src/parser/CommandParser.cpp",'command.lexeme == "!"'),
    ("src/parser/CommandParser.cpp","ZG requires a destination buffer"),
    ("src/runtime/CommandExecutor.cpp","execute_system"),
    ("src/runtime/CommandExecutor.cpp","execute_zap_gather"),
    ("tests/CMakeLists.txt","test_system_zg"),
    ("tests/test_system_zg.cpp","System command and ZG tests passed."),
    ("ROADMAP.md","## Version v0.0.14 — Sprint 2.19"),
]
for rel, marker in must_have:
    if marker not in FILES.get(rel, ""):
        ERRORS.append(f"contrôle final : {marker!r} absent de {rel}")

print()
if ERRORS:
    print("=== PRECHECK ÉCHOUÉ : AUCUN FICHIER N'A ÉTÉ MODIFIÉ ===")
    for i, e in enumerate(ERRORS, 1):
        print(f"{i:02d}. {e}")
    print(f"{len(ERRORS)} incompatibilité(s) détectée(s).")
    raise SystemExit(2)

print("=== PRECHECK COMPLET : OK ===")
print("Application atomique du Sprint 2.19 v3...")

written = []
try:
    for rel, content in FILES.items():
        if rel in ("docs/fr/reference/commandes/bang.md",
                   "docs/fr/reference/commandes/zg.md"):
            continue
        q = p(rel)
        previous = q.read_bytes() if q.exists() else None
        written.append((q, previous))
        q.parent.mkdir(parents=True, exist_ok=True)
        tmp = q.with_suffix(q.suffix + ".sprint219v3.tmp")
        tmp.write_text(content, encoding="utf-8", newline="\n")
        os.replace(tmp, q)
except Exception:
    print("ERREUR pendant l'application : rollback...")
    for q, previous in reversed(written):
        if previous is None:
            try:
                q.unlink()
            except FileNotFoundError:
                pass
        else:
            tmp = q.with_suffix(q.suffix + ".rollback.tmp")
            tmp.write_bytes(previous)
            os.replace(tmp, q)
    raise

print("Sprint 2.19 v3 appliqué avec succès.")
print("Version : 0.0.14. Commandes : ! et ZG.")
print("bang.md et zg.md n'ont pas été modifiés.")
print("Étape suivante : git diff --check, rebuild, puis tests fonctionnels.")
