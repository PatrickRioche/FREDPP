#!/usr/bin/env python3
from __future__ import annotations

import hashlib
import os
import sys
from pathlib import Path

ROOT = Path(sys.argv[1] if len(sys.argv) > 1 else ".").resolve()
FILES: dict[str, str] = {}


def path(rel: str) -> Path:
    return ROOT / rel


def read(rel: str) -> str:
    p = path(rel)
    if not p.is_file():
        raise RuntimeError(f"fichier absent : {rel}")
    return p.read_text(encoding="utf-8").replace("\r\n", "\n")


def load(rel: str) -> None:
    FILES[rel] = read(rel)


def replace_once(rel: str, old: str, new: str, marker: str | None = None) -> None:
    text = FILES[rel]
    if marker and marker in text:
        print(f"déjà OK  : {rel}")
        return
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"{rel}: contexte attendu trouvé {count} fois au lieu de 1")
    FILES[rel] = text.replace(old, new, 1)
    print(f"modifié  : {rel}")


def insert_before(rel: str, anchor: str, block: str, marker: str) -> None:
    text = FILES[rel]
    if marker in text:
        print(f"déjà OK  : {rel}")
        return
    count = text.count(anchor)
    if count != 1:
        raise RuntimeError(f"{rel}: ancre trouvée {count} fois au lieu de 1")
    FILES[rel] = text.replace(anchor, block + anchor, 1)
    print(f"modifié  : {rel}")


def insert_after(rel: str, anchor: str, block: str, marker: str) -> None:
    text = FILES[rel]
    if marker in text:
        print(f"déjà OK  : {rel}")
        return
    count = text.count(anchor)
    if count != 1:
        raise RuntimeError(f"{rel}: ancre trouvée {count} fois au lieu de 1")
    FILES[rel] = text.replace(anchor, anchor + block, 1)
    print(f"modifié  : {rel}")


def remove_once(rel: str, block: str, marker: str) -> None:
    text = FILES[rel]
    if marker not in text:
        print(f"déjà OK  : {rel}")
        return
    count = text.count(block)
    if count != 1:
        raise RuntimeError(f"{rel}: bloc à supprimer trouvé {count} fois au lieu de 1")
    FILES[rel] = text.replace(block, "", 1)
    print(f"modifié  : {rel}")


def ensure_new(rel: str, content: str, marker: str) -> None:
    p = path(rel)
    if p.exists():
        current = p.read_text(encoding="utf-8").replace("\r\n", "\n")
        if marker in current:
            FILES[rel] = current
            print(f"déjà OK  : {rel}")
            return
        raise RuntimeError(f"{rel}: existe déjà avec un contenu inattendu")
    FILES[rel] = content
    print(f"créé     : {rel}")


def sha256_file(p: Path) -> str:
    return hashlib.sha256(p.read_bytes()).hexdigest()


required = [
    "CMakeLists.txt",
    "include/fred/ast/AstNode.hpp",
    "include/fred/ast/CommandNode.hpp",
    "include/fred/runtime/ExecutionContext.hpp",
    "include/fred/core/BufferManager.hpp",
    "src/core/BufferManager.cpp",
    "src/command/CommandRegistry.cpp",
    "src/parser/CommandParser.cpp",
    "src/runtime/ExecutionContext.cpp",
    "src/runtime/CommandExecutor.cpp",
    "src/cli/main.cpp",
    "tests/CMakeLists.txt",
    "tests/test_command_parser.cpp",
    "tests/test_help_manager.cpp",
    "tests/test_options.cpp",
    "docs/fr/fredpp/commandes-speciales.md",
    "docs/project/COMMAND_STATUS.md",
    "CHANGELOG.md",
    "ROADMAP.md",
    "RELEASE_NOTES.md",
    "docs/fr/reference/commandes/fb.md",
    "docs/fr/reference/commandes/fo.md",
    "docs/fr/reference/commandes/oi_parenthesis.md",
]
for rel in required:
    if not path(rel).is_file():
        raise RuntimeError(f"fichier absent : {rel}")

reference_files = [
    path("docs/fr/reference/commandes/fb.md"),
    path("docs/fr/reference/commandes/fo.md"),
    path("docs/fr/reference/commandes/oi_parenthesis.md"),
]
reference_hashes = {p: sha256_file(p) for p in reference_files}

for rel in required:
    if not rel.startswith("docs/fr/reference/commandes/"):
        load(rel)

# Vérification du Lot 1A déjà homologué.
lot1_markers = [
    ("include/fred/ast/AstNode.hpp", "OptionCommand"),
    ("include/fred/ast/CommandNode.hpp", "class OptionCommandNode final"),
    ("include/fred/runtime/ExecutionContext.hpp", "input_parentheses_required"),
    ("src/runtime/ExecutionContext.cpp", "set_input_parentheses_required"),
    ("src/parser/CommandParser.cpp", "compact_buffer_name"),
    ("src/runtime/CommandExecutor.cpp", "execute_option"),
    ("tests/test_options.cpp", "OI( option and short buffer-name tests passed"),
]
for rel, marker in lot1_markers:
    if marker not in FILES[rel]:
        raise RuntimeError(f"Lot 1A incomplet : {marker!r} absent de {rel}")

# AST FB/FO.
replace_once(
    "include/fred/ast/AstNode.hpp",
    "    OptionCommand,\n    QuitCommand\n",
    "    FactsCommand,\n    OptionCommand,\n    QuitCommand\n",
    "FactsCommand,",
)
insert_before(
    "include/fred/ast/CommandNode.hpp",
    "enum class OptionKind {\n",
    """enum class FactsKind {
    Buffers,
    Options
};

class FactsCommandNode final : public CommandNode {
public:
    FactsCommandNode(FactsKind facts,
                     SourceLocation location) noexcept
        : CommandNode(nullptr, location), facts_(facts) {}

    [[nodiscard]] AstNodeKind kind() const noexcept override {
        return AstNodeKind::FactsCommand;
    }
    [[nodiscard]] FactsKind facts() const noexcept { return facts_; }

private:
    FactsKind facts_;
};

""",
    "class FactsCommandNode final",
)

# BufferManager MRU.
replace_once(
    "include/fred/core/BufferManager.hpp",
    "    [[nodiscard]] std::vector<std::string> names() const;\n    [[nodiscard]] std::vector<std::string> modified_names() const;\n",
    "    [[nodiscard]] std::vector<std::string> names() const;\n    [[nodiscard]] std::vector<std::string> recent_names() const;\n    [[nodiscard]] std::vector<std::string> modified_names() const;\n",
    "recent_names() const",
)
replace_once(
    "include/fred/core/BufferManager.hpp",
    "private:\n    using Storage = std::unordered_map<std::string, std::unique_ptr<Buffer>>;\n\n    Storage buffers_;\n    Buffer* current_{nullptr};\n",
    "private:\n    using Storage = std::unordered_map<std::string, std::unique_ptr<Buffer>>;\n\n    void touch(std::string_view name);\n\n    Storage buffers_;\n    std::vector<std::string> usage_order_;\n    Buffer* current_{nullptr};\n",
    "usage_order_",
)
insert_before(
    "src/core/BufferManager.cpp",
    "Buffer& BufferManager::create_or_select(std::string name) {\n",
    """void BufferManager::touch(std::string_view name) {
    const std::string value(name);
    usage_order_.erase(
        std::remove(usage_order_.begin(), usage_order_.end(), value),
        usage_order_.end());
    usage_order_.insert(usage_order_.begin(), value);
}

""",
    "void BufferManager::touch(",
)
replace_once(
    "src/core/BufferManager.cpp",
    "    current_ = it->second.get();\n    // FR-0006: remove only a transient empty buffer with no file association\n",
    "    current_ = it->second.get();\n    touch(name);\n    // FR-0006: remove only a transient empty buffer with no file association\n",
    "    touch(name);\n    // FR-0006",
)
replace_once(
    "src/core/BufferManager.cpp",
    "    if (remove_previous) {\n        buffers_.erase(previous_name);\n    }\n",
    "    if (remove_previous) {\n        buffers_.erase(previous_name);\n        usage_order_.erase(\n            std::remove(usage_order_.begin(), usage_order_.end(), previous_name),\n            usage_order_.end());\n    }\n",
    "usage_order_.end(), previous_name)",
)
# select() : ajoute touch(name) sans toucher get().
select_old = "    current_ = it->second.get();\n    return *current_;\n}\nconst Buffer& BufferManager::get"
select_new = "    current_ = it->second.get();\n    touch(name);\n    return *current_;\n}\nconst Buffer& BufferManager::get"
if select_new in FILES["src/core/BufferManager.cpp"]:
    print("déjà OK  : src/core/BufferManager.cpp")
elif FILES["src/core/BufferManager.cpp"].count(select_old) == 1:
    FILES["src/core/BufferManager.cpp"] = FILES["src/core/BufferManager.cpp"].replace(select_old, select_new, 1)
    print("modifié  : src/core/BufferManager.cpp")
else:
    raise RuntimeError("src/core/BufferManager.cpp: impossible de modifier select()")

replace_once(
    "src/core/BufferManager.cpp",
    "    const bool deleting_current = current_ == it->second.get();\n    buffers_.erase(it);\n\n    if (buffers_.empty()) {\n        current_ = nullptr;\n        create_or_select(\"0\");\n    } else if (deleting_current) {\n        current_ = buffers_.begin()->second.get();\n    }\n",
    "    const bool deleting_current = current_ == it->second.get();\n    const std::string erased_name = it->first;\n    buffers_.erase(it);\n    usage_order_.erase(\n        std::remove(usage_order_.begin(), usage_order_.end(), erased_name),\n        usage_order_.end());\n\n    if (buffers_.empty()) {\n        current_ = nullptr;\n        create_or_select(\"0\");\n    } else if (deleting_current) {\n        if (!usage_order_.empty()) {\n            current_ = buffers_.at(usage_order_.front()).get();\n        } else {\n            current_ = buffers_.begin()->second.get();\n        }\n    }\n",
    "const std::string erased_name = it->first;",
)
insert_before(
    "src/core/BufferManager.cpp",
    "std::vector<std::string> BufferManager::modified_names() const {\n",
    """std::vector<std::string> BufferManager::recent_names() const {
    return usage_order_;
}

""",
    "BufferManager::recent_names() const",
)

# Registre F.
insert_before(
    "src/command/CommandRegistry.cpp",
    "    registry.register_command(CommandDescriptor{\n        'O', \"Option\",\n",
    """    registry.register_command(CommandDescriptor{
        'F', "Facts",
        [](std::unique_ptr<AddressNode> address, SourceLocation location) {
            if (address) {
                throw std::invalid_argument("F does not accept a line address");
            }
            return std::make_unique<FactsCommandNode>(FactsKind::Buffers, location);
        }});
""",
    "'F', \"Facts\"",
)

# Parser FB/FO.
replace_once(
    "src/parser/CommandParser.cpp",
    "        std::optional<std::string> compact_buffer_name;\n        if (command.type == TokenType::Identifier && command.lexeme.size() == 2) {\n",
    "        std::optional<std::string> compact_buffer_name;\n        std::optional<FactsKind> facts_kind;\n        if (command.type == TokenType::Identifier && command.lexeme.size() == 2) {\n",
    "std::optional<FactsKind> facts_kind;",
)
replace_once(
    "src/parser/CommandParser.cpp",
    "            } else if (first == 'B') {\n                compact_buffer_name = std::string(1, command.lexeme[1]);\n                command.type = TokenType::Command;\n                command.lexeme = \"B\";\n            }\n",
    "            } else if (first == 'F' && (second == 'B' || second == 'O')) {\n                facts_kind = second == 'B' ? FactsKind::Buffers\n                                           : FactsKind::Options;\n                command.type = TokenType::Command;\n                command.lexeme = \"F\";\n            } else if (first == 'B') {\n                compact_buffer_name = std::string(1, command.lexeme[1]);\n                command.type = TokenType::Command;\n                command.lexeme = \"B\";\n            }\n",
    "facts_kind = second == 'B'",
)
insert_before(
    "src/parser/CommandParser.cpp",
    "        if (mnemonic == 'O') {\n",
    """        if (mnemonic == 'F') {
            if (address) {
                throw ParseError("F does not accept a line address", address->location());
            }
            if (!facts_kind) {
                throw ParseError("only FB and FO are implemented", command.location);
            }
            require_command_end();
            return std::make_unique<FactsCommandNode>(*facts_kind, command.location);
        }

""",
    "only FB and FO are implemented",
)

# Runtime FB/FO.
insert_before(
    "src/runtime/CommandExecutor.cpp",
    "void execute_option(const OptionCommandNode& command,\n",
    """std::string describe_buffer(const Buffer& buffer) {
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

void execute_facts(const FactsCommandNode& command, ExecutionContext& context) {
    switch (command.facts()) {
    case FactsKind::Options:
        context.output().write_line(
            context.input_parentheses_required() ? "o+i(" : "o-i(");
        return;
    case FactsKind::Buffers:
        for (const auto& name : context.buffers().recent_names()) {
            context.output().write_line(describe_buffer(context.buffers().get(name)));
        }
        return;
    }
}

""",
    "void execute_facts(const FactsCommandNode&",
)
insert_before(
    "src/runtime/CommandExecutor.cpp",
    "    case AstNodeKind::OptionCommand:\n",
    "    case AstNodeKind::FactsCommand:\n        execute_facts(static_cast<const FactsCommandNode&>(command), context);\n        return;\n",
    "case AstNodeKind::FactsCommand:",
)

# CLI.
replace_once(
    "src/cli/main.cpp",
    "    case fred::AstNodeKind::OptionCommand: name = \"Option\"; break;\n",
    "    case fred::AstNodeKind::FactsCommand: name = \"Facts\"; break;\n    case fred::AstNodeKind::OptionCommand: name = \"Option\"; break;\n",
    "AstNodeKind::FactsCommand: name = \"Facts\"",
)
replace_once(
    "src/cli/main.cpp",
    "              << \" - executable P, L, D, A, B, I, C, M, T, G, Z, S, Q, R, W commands and OI( option; * aliases 1,$\\n\";\n",
    "              << \" - executable P, L, D, A, B, I, C, M, T, G, Z, S, Q, R, W, FB and FO commands; OI( option; * aliases 1,$\\n\";\n",
    "W, FB and FO commands",
)
remove_once(
    "src/cli/main.cpp",
    """            if (input == ":buffers") {
                for (const auto& name : manager.names()) {
                    std::cout << (name == manager.current().name() ? "* " : "  ")
                              << name << '\n';
                }
                continue;
            }
""",
    ":buffers",
)

# Aide CMake.
replace_once("CMakeLists.txt", "    d\n    g\n", "    d\n    fb\n    fo\n    g\n", "    fb\n    fo\n")
replace_once(
    "CMakeLists.txt",
    'math(EXPR FREDPP_EMBEDDED_HELP_COUNT "${FREDPP_HELP_FILES_LENGTH} + 4")\n',
    'math(EXPR FREDPP_EMBEDDED_HELP_COUNT "${FREDPP_HELP_FILES_LENGTH} + 5")\n',
    '${FREDPP_HELP_FILES_LENGTH} + 5',
)
replace_once(
    "CMakeLists.txt",
    '"    ?wu         Afficher l\'extension UTF-8 de la commande W\\n"\n"\\nALIAS HISTORIQUE IMPLÉMENTÉ\\n\\n"\n',
    '"    ?wu         Afficher l\'extension UTF-8 de la commande W\\n"\n"\\nOPTION HISTORIQUE IMPLÉMENTÉE\\n\\n"\n"    ?oi(        Afficher l\'aide de l\'option OI(\\n"\n"\\nALIAS HISTORIQUE IMPLÉMENTÉ\\n\\n"\n',
    "?oi(",
)
insert_before(
    "CMakeLists.txt",
    "set(FREDPP_WU_HELP_FILE\n",
    """set(FREDPP_OI_HELP_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/reference/commandes/oi_parenthesis.md"
)
if(NOT EXISTS "${FREDPP_OI_HELP_FILE}")
    message(FATAL_ERROR "Missing historical OI( help: ${FREDPP_OI_HELP_FILE}")
endif()
file(READ "${FREDPP_OI_HELP_FILE}" FREDPP_OI_HELP_CONTENT)
string(FIND "${FREDPP_OI_HELP_CONTENT}" ")FREDPP_HELP\\\"" FREDPP_OI_HELP_COLLISION)
if(NOT FREDPP_OI_HELP_COLLISION EQUAL -1)
    message(FATAL_ERROR "OI( help contains the reserved raw-string delimiter")
endif()

""",
    "FREDPP_OI_HELP_FILE",
)
insert_before(
    "CMakeLists.txt",
    '"    Entry{\\"wu\\", R\\"FREDPP_HELP(${FREDPP_WU_HELP_CONTENT})FREDPP_HELP\\"},\\n"\n',
    '"    Entry{\\"oi(\\", R\\"FREDPP_HELP(${FREDPP_OI_HELP_CONTENT})FREDPP_HELP\\"},\\n"\n',
    'Entry{\\"oi(\\"',
)

# Supprime :buffers de l'aide FREDPP.
replace_once(
    "docs/fr/fredpp/commandes-speciales.md",
    "GESTION DES BUFFERS\n    :buffers                 Lister les buffers et identifier le buffer courant\n    :print                   Afficher le buffer courant avec les numéros de ligne\n",
    "GESTION DES BUFFERS\n    :print                   Afficher le buffer courant avec les numéros de ligne\n",
    "GESTION DES BUFFERS\n    :print",
)

# Tests registre + test_facts.
replace_once("tests/test_command_parser.cpp", "        assert(registry.size() == 16);\n", "        assert(registry.size() == 17);\n", "registry.size() == 17")
insert_before("tests/test_command_parser.cpp", "        assert(registry.contains('G'));\n", "        assert(registry.contains('F'));\n", "registry.contains('F')")
insert_after("tests/CMakeLists.txt", "fredpp_add_test(test_options test_options.cpp)\n", "fredpp_add_test(test_facts test_facts.cpp)\n", "test_facts test_facts.cpp")

# Tests aide.
insert_before("tests/test_help_manager.cpp", '    assert(help.exists("g"));\n', '    assert(help.exists("fb"));\n    assert(help.exists("fo"));\n', 'help.exists("fb")')
insert_before("tests/test_help_manager.cpp", '    assert(help.exists("z"));\n', '    assert(help.exists("oi("));\n', 'help.exists("oi(")')
insert_before("tests/test_help_manager.cpp", '    assert(index.find("    ?g") != std::string::npos);\n', '    assert(index.find("    ?fb") != std::string::npos);\n    assert(index.find("    ?fo") != std::string::npos);\n', 'index.find("    ?fb")')
insert_before("tests/test_help_manager.cpp", '    assert(index.find("    ?z") != std::string::npos);\n', '    assert(index.find("    ?oi(") != std::string::npos);\n', 'index.find("    ?oi(")')
replace_once("tests/test_help_manager.cpp", '    assert(special.find("    :buffers") != std::string::npos);\n', '    assert(special.find("    :buffers") == std::string::npos);\n', 'special.find("    :buffers") == std::string::npos')
insert_before(
    "tests/test_help_manager.cpp",
    '    const auto global = help.load_for_terminal("g");\n',
    '    const auto facts_buffers = help.load_for_terminal("fb");\n    assert(facts_buffers.find("FB") != std::string::npos);\n\n    const auto facts_options = help.load_for_terminal("fo");\n    assert(facts_options.find("FO") != std::string::npos);\n\n    const auto option_parenthesis = help.load_for_terminal("oi(");\n    assert(option_parenthesis.find("OI(") != std::string::npos);\n\n',
    'const auto facts_buffers = help.load_for_terminal("fb");',
)
insert_before("tests/test_help_manager.cpp", '    assert(std::find(topics.begin(), topics.end(), "g") != topics.end());\n', '    assert(std::find(topics.begin(), topics.end(), "fb") != topics.end());\n    assert(std::find(topics.begin(), topics.end(), "fo") != topics.end());\n', 'topics.end(), "fb")')
insert_before("tests/test_help_manager.cpp", '    assert(std::find(topics.begin(), topics.end(), "z") != topics.end());\n', '    assert(std::find(topics.begin(), topics.end(), "oi(") != topics.end());\n', 'topics.end(), "oi(")')

facts_test = r'''#include "fred/ast/CommandNode.hpp"
#include "fred/command/CommandRegistry.hpp"
#include "fred/core/BufferManager.hpp"
#include "fred/lexer/Lexer.hpp"
#include "fred/lexer/TokenStream.hpp"
#include "fred/parser/CommandParser.hpp"
#include "fred/runtime/CommandExecutor.hpp"
#include "fred/runtime/ExecutionContext.hpp"
#include "fred/runtime/Output.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <string_view>

namespace {
std::unique_ptr<fred::CommandNode> parse(std::string_view source) {
    fred::Lexer lexer(source);
    fred::TokenStream tokens(lexer);
    const auto registry = fred::make_core_command_registry();
    fred::CommandParser parser(tokens, registry);
    return parser.parse();
}
void execute(std::string_view source, fred::CommandExecutor& executor,
             fred::ExecutionContext& context) {
    auto command = parse(source);
    executor.execute(*command, context);
}
} // namespace

int main() {
    {
        const auto node = parse("fb");
        assert(node->kind() == fred::AstNodeKind::FactsCommand);
        const auto& facts = static_cast<const fred::FactsCommandNode&>(*node);
        assert(facts.facts() == fred::FactsKind::Buffers);
    }
    {
        const auto node = parse("FO");
        assert(node->kind() == fred::AstNodeKind::FactsCommand);
        const auto& facts = static_cast<const fred::FactsCommandNode&>(*node);
        assert(facts.facts() == fred::FactsKind::Options);
    }

    fred::BufferManager buffers;
    fred::StringOutput output;
    fred::ExecutionContext context(buffers, output);
    fred::CommandExecutor executor;

    execute("FO", executor, context);
    assert(output.content() == "o+i(\n");
    output.clear();

    execute("O-I(", executor, context);
    execute("fo", executor, context);
    assert(output.content() == "o-i(\n");
    output.clear();

    auto& alpha = buffers.create_or_select("alpha");
    alpha.append("A1");
    alpha.associate_file("alpha.txt", fred::TextEncoding::Utf8,
                         fred::LineEnding::Lf, true);

    auto& beta = buffers.create_or_select("beta");
    beta.append("B1");
    beta.mark_clean();

    buffers.select("alpha");
    execute("FB", executor, context);

    const std::string expected =
        "b(alpha) 1,1 alpha.txt ?\n"
        "b(beta) 1,1\n"
        "b(0) 0,0\n";
    assert(output.content() == expected);
    return 0;
}
'''
ensure_new("tests/test_facts.cpp", facts_test, "FactsKind::Buffers")

# COMMAND_STATUS + couverture.
coverage = """## Couverture de la référence historique

| Famille | Total référence | Implémenté / homologué | Reste |
|---|---:|---:|---:|
| Commandes principales | 22 | 15 | 7 |
| Commandes spéciales | 7 | 0 | 7 |
| Famille F | 7 | 2 (`FB`, `FO`) | 5 |
| Famille J | 5 | 0 | 5 |
| Famille N | 21 | 0 | 21 |
| Famille O | 24 | 1 (`OI(`) | 23 |
| Famille Z | 17 | 0 | 17 |
| **Total** | **103** | **18** | **85** |

La couverture fonctionnelle de la référence est de **18 / 103**, soit environ **17,5 %**. Une entrée n'est comptée comme implémentée qu'après compilation, tests automatisés et validation interactive lorsque celle-ci est pertinente.

"""
insert_before("docs/project/COMMAND_STATUS.md", "| Commande | État dans FREDPP | Aide embarquée |\n", coverage, "## Couverture de la référence historique")
insert_before("docs/project/COMMAND_STATUS.md", "| G | Disponible avec une commande imbriquée P, D, Z ou S | Oui |\n", "| FB | Disponible; ordre décroissant de dernière utilisation, ligne courante, nombre de lignes, fichier associé et état modifié | Oui (`?fb`) |\n| FO | Disponible; affiche les options effectivement en service | Oui (`?fo`) |\n", "| FB | Disponible;")
replace_once("docs/project/COMMAND_STATUS.md", "| `OI(` | Sprint 2.15 lot 1A : noyau `O+I(` / `O-I(` et noms courts de buffer | Lot 1B |\n", "| `OI(` | Disponible : `O+I(` / `O-I(`; valeur implicite `O+I(`; noms courts de buffer autorisés avec `O-I(` | Oui (`?oi(`) |\n", "| `OI(` | Disponible :")

# Version 0.0.10 et docs.
replace_once("CMakeLists.txt", "project(FREDPP VERSION 0.0.9 LANGUAGES CXX)\n", "project(FREDPP VERSION 0.0.10 LANGUAGES CXX)\n", "project(FREDPP VERSION 0.0.10")

changelog = """## v0.0.10

### Added

- Option historique `O+I(` / `O-I(` et noms courts de buffers à un caractère.
- Commande historique `FO` pour afficher les options en service.
- Commande historique `FB` avec état des buffers, fichier associé et ordre de dernière utilisation.
- Aides embarquées `?oi(`, `?fo` et `?fb`.
- Tableau de couverture de la référence historique.

### Changed

- `FB` remplace la commande de développement `:buffers`.
- La suite passe à 29 tests.

"""
insert_before("CHANGELOG.md", "## v0.0.9\n", changelog, "## v0.0.10")

roadmap = """## Version v0.0.10 — Sprint 2.15
### Modifications intégrées

- option historique `O+I(` / `O-I(` avec `O+I(` comme valeur implicite ;
- noms courts de buffers à un caractère lorsque `O-I(` est actif ;
- commande `FO` pour afficher les options effectivement en service ;
- commande `FB` pour afficher les buffers par ordre décroissant de dernière utilisation, avec ligne courante, nombre de lignes, fichier associé et indicateur de modification ;
- suppression de la commande de développement `:buffers`, remplacée par `FB` ;
- aides embarquées `?oi(`, `?fo` et `?fb` ;
- suivi de couverture dans `docs/project/COMMAND_STATUS.md` ;
- passage à 29 tests.

"""
insert_before("ROADMAP.md", "## État fonctionnel actuel\n", roadmap, "## Version v0.0.10 — Sprint 2.15")
replace_once("ROADMAP.md", "P, L, D, A, B, I, C, M, T, G, Z, S, Q, R, W, WA, WU\n", "P, L, D, A, B, I, C, M, T, G, Z, S, Q, R, W, WA, WU, FB, FO\n", "WU, FB, FO")

release_notes = """## Sprint 2.15 — Options et informations sur les buffers

- prise en charge de `O+I(` et `O-I(` ;
- prise en charge des noms courts de buffers avec `O-I(` ;
- ajout de `FO` pour afficher les options actives ;
- ajout de `FB` pour afficher l'état des buffers et leur fichier associé ;
- suppression de `:buffers` ;
- ajout des aides `?oi(`, `?fo` et `?fb` ;
- suivi de couverture par rapport à la documentation historique ;
- passage à 29 tests.

"""
insert_before("RELEASE_NOTES.md", "## Sprint 2.14 — Première distribution officielle\n", release_notes, "## Sprint 2.15 — Options et informations sur les buffers")

# Contrôles finaux avant écriture.
for p, digest in reference_hashes.items():
    if sha256_file(p) != digest:
        raise RuntimeError(f"référence historique modifiée pendant le patch : {p}")

checks = [
    ("src/cli/main.cpp", ":buffers", False),
    ("docs/fr/fredpp/commandes-speciales.md", ":buffers", False),
    ("src/parser/CommandParser.cpp", "FactsKind::Buffers", True),
    ("src/runtime/CommandExecutor.cpp", "execute_facts", True),
    ("src/core/BufferManager.cpp", "recent_names() const", True),
    ("CMakeLists.txt", "project(FREDPP VERSION 0.0.10", True),
    ("tests/test_command_parser.cpp", "registry.size() == 17", True),
    ("docs/project/COMMAND_STATUS.md", "**18** | **85**", True),
]
for rel, marker, expected in checks:
    present = marker in FILES[rel]
    if present != expected:
        raise RuntimeError(f"contrôle final échoué pour {marker!r} dans {rel}")

# Écriture seulement maintenant.
for rel, text in FILES.items():
    p = path(rel)
    p.parent.mkdir(parents=True, exist_ok=True)
    tmp = p.with_suffix(p.suffix + ".sprint215.tmp")
    tmp.write_text(text, encoding="utf-8", newline="\n")
    os.replace(tmp, p)

for p, digest in reference_hashes.items():
    if sha256_file(p) != digest:
        raise RuntimeError(f"ERREUR CRITIQUE : référence historique modifiée : {p}")

# Nettoyage des patchs temporaires précédents.
temporary = [
    "apply_FREDPP_Sprint_2.15_Lot1.py",
    "apply_FREDPP_Sprint_2.15_Lot1A_resume.py",
    "fix_FREDPP_Sprint_2.15_ExecutionContext_cpp.py",
    "normalize_FREDPP_ExecutionContext_cpp.py",
    "fix_FREDPP_Sprint_2.15_duplicate_option.py",
    "apply_FREDPP_Sprint_2.15_Lot2.ps1",
]
for name in temporary:
    p = ROOT / name
    if p.exists():
        p.unlink()
        print(f"supprimé  : {name}")

print()
print("Sprint 2.15 / Lot 2 appliqué.")
print("Références historiques : inchangées.")
print("Version cible : v0.0.10.")
print("FB, FO, ?oi(, ?fb et ?fo intégrés ; :buffers supprimé.")
print("Tests attendus après rebuild : 29.")
print("Scripts temporaires du sprint supprimés.")

# Auto-suppression du Lot 2 : aucun patch Python ne doit rester dans le dépôt.
try:
    Path(__file__).resolve().unlink()
    print("supprimé  : " + Path(__file__).name)
except OSError:
    print("NOTE : supprime manuellement " + Path(__file__).name)
