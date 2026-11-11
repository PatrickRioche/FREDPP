cmake_minimum_required(VERSION 3.24)

# Use embedded debug information (/Z7) with MSVC. This avoids concurrent
# compiler access to target PDB files, which can fail on synchronized folders.
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
endif()
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(FREDPP VERSION 0.1.0 LANGUAGES CXX)

# The historical command coverage is independent of the software release
# version. Keep the documented coverage and this expectation synchronized;
# semantic versioning must not change merely because a command is added.
set(FREDPP_EXPECTED_REFERENCE_COUNT 103)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/docs/project/COMMAND_STATUS.md"
    FREDPP_REFERENCE_TOTAL_ROW REGEX "^[|] [*][*]Total[*][*] [|]")
if(NOT FREDPP_REFERENCE_TOTAL_ROW)
    message(FATAL_ERROR "Missing historical command coverage total row")
endif()
string(REGEX MATCHALL "[0-9]+" FREDPP_REFERENCE_TOTAL_VALUES
    "${FREDPP_REFERENCE_TOTAL_ROW}")
list(LENGTH FREDPP_REFERENCE_TOTAL_VALUES FREDPP_REFERENCE_TOTAL_FIELDS)
if(NOT FREDPP_REFERENCE_TOTAL_FIELDS EQUAL 3)
    message(FATAL_ERROR "Invalid historical command coverage total row")
endif()
list(GET FREDPP_REFERENCE_TOTAL_VALUES 1 FREDPP_IMPLEMENTED_REFERENCE_COUNT)
if(NOT "${FREDPP_EXPECTED_REFERENCE_COUNT}" STREQUAL "${FREDPP_IMPLEMENTED_REFERENCE_COUNT}")
    message(FATAL_ERROR
        "Expected ${FREDPP_EXPECTED_REFERENCE_COUNT} historical reference "
        "commands but COMMAND_STATUS.md reports ${FREDPP_IMPLEMENTED_REFERENCE_COUNT}; "
        "update FREDPP_EXPECTED_REFERENCE_COUNT and COMMAND_STATUS.md together")
endif()

if(MSVC)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>"
    )
    # Release kits use the static MSVC runtime so the portable ZIP does not
    # require a separate Visual C++ redistributable installation.
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

option(FREDPP_BUILD_TESTS "Build FREDPP tests" ON)
option(FREDPP_BUILD_CLI "Build FREDPP command-line executable" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build identity. The software version is defined only by project(... VERSION).
# Git information is captured when CMake configures the build.
set(FREDPP_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${FREDPP_GENERATED_DIR}")

set(FREDPP_GIT_COMMIT "inconnu")
set(FREDPP_GIT_STATE "inconnu")
set(FREDPP_GIT_DIRTY 0)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --short=7 HEAD
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        RESULT_VARIABLE FREDPP_GIT_COMMIT_RESULT
        OUTPUT_VARIABLE FREDPP_GIT_COMMIT_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(FREDPP_GIT_COMMIT_RESULT EQUAL 0 AND NOT FREDPP_GIT_COMMIT_OUTPUT STREQUAL "")
        set(FREDPP_GIT_COMMIT "${FREDPP_GIT_COMMIT_OUTPUT}")

        execute_process(
            COMMAND "${GIT_EXECUTABLE}" status --porcelain --untracked-files=normal
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            RESULT_VARIABLE FREDPP_GIT_STATUS_RESULT
            OUTPUT_VARIABLE FREDPP_GIT_STATUS_OUTPUT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if(FREDPP_GIT_STATUS_RESULT EQUAL 0)
            if(FREDPP_GIT_STATUS_OUTPUT STREQUAL "")
                set(FREDPP_GIT_STATE "propre")
            else()
                set(FREDPP_GIT_STATE "modifié")
                set(FREDPP_GIT_DIRTY 1)
            endif()
        endif()
    endif()
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/VersionInfo.hpp.in"
    "${FREDPP_GENERATED_DIR}/VersionInfo.hpp"
    @ONLY
)

message(STATUS "FREDPP ${PROJECT_VERSION} - commit ${FREDPP_GIT_COMMIT} - sources ${FREDPP_GIT_STATE}")

# Embed only the French reference pages for commands available in FREDPP.
# Files under docs/fr/reference/commandes are source documentation and are
# deliberately never modified by the build.
set(FREDPP_IMPLEMENTED_HELP_TOPICS
    a
    b
    c
    d
    e
    f
    fe
    fb
    ff
    fq
    fn
    fo
    fv
    g
    i
    j
    jb
    oi_backslash_f
    oia
    oc
    oic
    oie
    oif
    oil
    oir
    oiu
    ool
    ooo
    oou
    op
    ob
    oo
    or
    os
    jm
    jp
    je
    jo
    k
    l
    m
    ncolon
    nplus
    nminus
    nmultiply
    ndivide
    nmodulo
    nequals
    nless
    ngreater
    nand
    npipe
    ncomplement
    nxor
    nshiftleft
    nshiftright
    na
    nb
    nd
    nf
    nl
    np
    p
    q
    r
    s
    os_separator
    t
    u
    v
    w
    y
    z
    za
    zb
    zc
    zd
    zg
    zi
    zk
    zl
    zm
    zo
    zp
    zr
    zs
    zt
    zu
    zv
    zw
)

set(FREDPP_HELP_FILES)
foreach(help_topic IN LISTS FREDPP_IMPLEMENTED_HELP_TOPICS)
    set(help_file
        "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/reference/commandes/${help_topic}.md"
    )
    if(NOT EXISTS "${help_file}")
        message(FATAL_ERROR "Missing French help page: ${help_file}")
    endif()
    list(APPEND FREDPP_HELP_FILES "${help_file}")
endforeach()

list(LENGTH FREDPP_HELP_FILES FREDPP_HELP_FILES_LENGTH)
math(EXPR FREDPP_EMBEDDED_HELP_COUNT "${FREDPP_HELP_FILES_LENGTH} + 20")

string(CONCAT FREDPP_HELP_INDEX
"============================================================\n"
"AIDE FRED — COMMANDES DISPONIBLES\n"
"============================================================\n\n"
"Cette aide embarque uniquement les documentations françaises\n"
"des commandes disponibles dans FREDPP.\n\n"
"COMMANDES IMPLÉMENTÉES\n\n"
)
foreach(help_topic IN LISTS FREDPP_IMPLEMENTED_HELP_TOPICS)
    string(TOUPPER "${help_topic}" help_topic_upper)
    set(help_command "?${help_topic}")
    if(help_topic STREQUAL "fq")
        # The historical filename fq.md corresponds to the literal F? command.
        set(help_topic_upper "F?")
        set(help_command "?f?")
    elseif(help_topic STREQUAL "oi_backslash_f")
        set(help_topic_upper "OI\\F")
        set(help_command "?oi\\f")
    elseif(help_topic STREQUAL "os_separator")
        set(help_topic_upper "OS/")
        set(help_command "?os/")
    elseif(help_topic STREQUAL "ncolon")
        # ncolon.md documents the historical N: assignment spelling.
        set(help_topic_upper "N:")
        set(help_command "?n:")
    elseif(help_topic STREQUAL "nplus")
        set(help_topic_upper "N+")
        set(help_command "?n+")
    elseif(help_topic STREQUAL "nminus")
        set(help_topic_upper "N-")
        set(help_command "?n-")
    elseif(help_topic STREQUAL "nmultiply")
        set(help_topic_upper "N*")
        set(help_command "?n*")
    elseif(help_topic STREQUAL "ndivide")
        set(help_topic_upper "N/")
        set(help_command "?n/")
    elseif(help_topic STREQUAL "nmodulo")
        set(help_topic_upper "N%")
        set(help_command "?n%")
    elseif(help_topic STREQUAL "nequals")
        set(help_topic_upper "N=")
        set(help_command "?n=")
    elseif(help_topic STREQUAL "nless")
        set(help_topic_upper "N<")
        set(help_command "?n<")
    elseif(help_topic STREQUAL "ngreater")
        set(help_topic_upper "N>")
        set(help_command "?n>")
    elseif(help_topic STREQUAL "nand")
        set(help_topic_upper "N&")
        set(help_command "?n&")
    elseif(help_topic STREQUAL "npipe")
        set(help_topic_upper "N|")
        set(help_command "?n|")
    elseif(help_topic STREQUAL "ncomplement")
        set(help_topic_upper "N~")
        set(help_command "?n~")
    elseif(help_topic STREQUAL "nxor")
        set(help_topic_upper "N^")
        set(help_command "?n^")
    elseif(help_topic STREQUAL "nshiftleft")
        set(help_topic_upper "N{")
        set(help_command "?n{")
    elseif(help_topic STREQUAL "nshiftright")
        set(help_topic_upper "N}")
        set(help_command "?n}")
    endif()
    string(LENGTH "${help_command}" help_command_length)
    math(EXPR help_padding_length "12 - ${help_command_length}")
    string(REPEAT " " ${help_padding_length} help_padding)
    string(APPEND FREDPP_HELP_INDEX
        "    ${help_command}${help_padding}Commande ${help_topic_upper}\n")
endforeach()
string(APPEND FREDPP_HELP_INDEX
"\nAUTRES AIDES\n\n"
"    ?           Afficher cette liste\n"
"    ?limites    Limites de securite et variables d'environnement\n"
"    ?directives Directives du flot et compatibilite\n"
"    ?!          Afficher l'aide historique de la commande !\n"
"\nOPTION HISTORIQUE IMPLÉMENTÉE\n\n"
"    ?oi(        Afficher l'aide de l'option OI(\n"
"    ?om         Afficher l'aide de l'option OM (monitorage)\n"
"    ?oq         Afficher l'aide de l'option OQ (arrêt sur erreur)\n"
"    ?os&        Afficher l'aide de l'option OS& (substitution)\n"
"    ?osd        Afficher l'aide de l'option OSD (recherche)\n"
"    ?ot         Afficher l'aide de l'option OT (tabulations)\n"
"    ?oia        Prompt de saisie A/I/C\n"
"    ?oic        Prompt de commande\n"
"    ?oir        Message à chaque retour clavier\n"
"    ?oil        Échappement pour minuscules en entrée\n"
"    ?oiu        Échappement pour majuscules en entrée\n"
"\nCOMMANDES SPÉCIALES HISTORIQUES IMPLÉMENTÉES\n\n"
"    ?\"          Afficher l'aide des commentaires FRED\n"
"    ?=          Afficher l'aide du numéro de ligne\n"
"    ?#          Afficher l'aide du compteur\n"
"    ?@          Afficher l'aide des étiquettes de procédure\n"
"    ?window     Afficher l'aide de la fenêtre de lignes :n\n"
"    ?wait       Afficher l'aide de l'attente (deux antislashs)\n"
"\nALIAS HISTORIQUE IMPLÉMENTÉ\n\n"
"    ?*          Afficher l'aide de l'alias * (1,$)\n"
)

set(FREDPP_SPECIAL_HELP_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/fredpp/commandes-speciales.md"
)
if(NOT EXISTS "${FREDPP_SPECIAL_HELP_FILE}")
    message(FATAL_ERROR "Missing FREDPP special-command help: ${FREDPP_SPECIAL_HELP_FILE}")
endif()
file(READ "${FREDPP_SPECIAL_HELP_FILE}" FREDPP_SPECIAL_HELP_CONTENT)
string(FIND "${FREDPP_SPECIAL_HELP_CONTENT}" ")FREDPP_HELP\"" FREDPP_SPECIAL_HELP_COLLISION)
if(NOT FREDPP_SPECIAL_HELP_COLLISION EQUAL -1)
    message(FATAL_ERROR "Special-command help contains the reserved raw-string delimiter")
endif()

set(FREDPP_BANG_HELP_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/reference/commandes/bang.md"
)
if(NOT EXISTS "${FREDPP_BANG_HELP_FILE}")
    message(FATAL_ERROR "Missing historical bang help: ${FREDPP_BANG_HELP_FILE}")
endif()
file(READ "${FREDPP_BANG_HELP_FILE}" FREDPP_BANG_HELP_CONTENT)
string(FIND "${FREDPP_BANG_HELP_CONTENT}" ")FREDPP_HELP\"" FREDPP_BANG_HELP_COLLISION)
if(NOT FREDPP_BANG_HELP_COLLISION EQUAL -1)
    message(FATAL_ERROR "Bang help contains the reserved raw-string delimiter")
endif()

set(FREDPP_QUOTE_HELP_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/reference/commandes/quote.md"
)
if(NOT EXISTS "${FREDPP_QUOTE_HELP_FILE}")
    message(FATAL_ERROR "Missing historical quote help: ${FREDPP_QUOTE_HELP_FILE}")
endif()
file(READ "${FREDPP_QUOTE_HELP_FILE}" FREDPP_QUOTE_HELP_CONTENT)
string(FIND "${FREDPP_QUOTE_HELP_CONTENT}" ")FREDPP_HELP\"" FREDPP_QUOTE_HELP_COLLISION)
if(NOT FREDPP_QUOTE_HELP_COLLISION EQUAL -1)
    message(FATAL_ERROR "Quote help contains the reserved raw-string delimiter")
endif()

set(FREDPP_EQUALS_HELP_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/reference/commandes/equals.md"
)
if(NOT EXISTS "${FREDPP_EQUALS_HELP_FILE}")
    message(FATAL_ERROR "Missing historical equals help: ${FREDPP_EQUALS_HELP_FILE}")
endif()
file(READ "${FREDPP_EQUALS_HELP_FILE}" FREDPP_EQUALS_HELP_CONTENT)
string(FIND "${FREDPP_EQUALS_HELP_CONTENT}" ")FREDPP_HELP\"" FREDPP_EQUALS_HELP_COLLISION)
if(NOT FREDPP_EQUALS_HELP_COLLISION EQUAL -1)
    message(FATAL_ERROR "Equals help contains the reserved raw-string delimiter")
endif()

set(FREDPP_OI_HELP_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/reference/commandes/oi_parenthesis.md"
)
if(NOT EXISTS "${FREDPP_OI_HELP_FILE}")
    message(FATAL_ERROR "Missing historical OI( help: ${FREDPP_OI_HELP_FILE}")
endif()
file(READ "${FREDPP_OI_HELP_FILE}" FREDPP_OI_HELP_CONTENT)
string(FIND "${FREDPP_OI_HELP_CONTENT}" ")FREDPP_HELP\"" FREDPP_OI_HELP_COLLISION)
if(NOT FREDPP_OI_HELP_COLLISION EQUAL -1)
    message(FATAL_ERROR "OI( help contains the reserved raw-string delimiter")
endif()

set(FREDPP_OM_HELP_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/reference/commandes/om.md"
)
if(NOT EXISTS "${FREDPP_OM_HELP_FILE}")
    message(FATAL_ERROR "Missing historical OM help: ${FREDPP_OM_HELP_FILE}")
endif()

foreach(option_help IN ITEMS oq os_ampersand osd ot)
    string(TOUPPER "${option_help}" option_help_upper)
    set(option_help_file
        "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/reference/commandes/${option_help}.md")
    file(READ "${option_help_file}" option_help_contents)
    string(FIND "${option_help_contents}" ")FREDPP_HELP\"" option_collision)
    if(NOT option_collision EQUAL -1)
        message(FATAL_ERROR "Option help contains reserved delimiter: ${option_help_file}")
    endif()
    set("FREDPP_${option_help_upper}_HELP_CONTENT" "${option_help_contents}")
endforeach()

foreach(special_help IN ITEMS at backslashbackslash colon counter)
    string(TOUPPER "${special_help}" special_help_upper)
    set(special_help_file
        "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/reference/commandes/${special_help}.md")
    file(READ "${special_help_file}" special_help_contents)
    string(FIND "${special_help_contents}" ")FREDPP_HELP\"" special_collision)
    if(NOT special_collision EQUAL -1)
        message(FATAL_ERROR "Special help contains reserved delimiter: ${special_help_file}")
    endif()
    set("FREDPP_${special_help_upper}_HELP_CONTENT" "${special_help_contents}")
endforeach()
file(READ "${FREDPP_OM_HELP_FILE}" FREDPP_OM_HELP_CONTENT)
string(FIND "${FREDPP_OM_HELP_CONTENT}" ")FREDPP_HELP\"" FREDPP_OM_HELP_COLLISION)
if(NOT FREDPP_OM_HELP_COLLISION EQUAL -1)
    message(FATAL_ERROR "OM help contains the reserved raw-string delimiter")
endif()

# The limits page is FREDPP-specific help, not a historical command.
set(FREDPP_DIRECTIVES_HELP_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/fredpp/directives-flot.md"
)
if(NOT EXISTS "${FREDPP_DIRECTIVES_HELP_FILE}")
    message(FATAL_ERROR "Missing flow directives help")
endif()
file(READ "${FREDPP_DIRECTIVES_HELP_FILE}" FREDPP_DIRECTIVES_HELP_CONTENT)
string(FIND "${FREDPP_DIRECTIVES_HELP_CONTENT}" ")FREDPP_HELP\"" FREDPP_DIRECTIVES_HELP_COLLISION)
if(NOT FREDPP_DIRECTIVES_HELP_COLLISION EQUAL -1)
    message(FATAL_ERROR "Flow directives help contains reserved delimiter")
endif()

set(FREDPP_LIMITES_HELP_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/fredpp/limites.md"
)
if(NOT EXISTS "${FREDPP_LIMITES_HELP_FILE}")
    message(FATAL_ERROR "Missing FREDPP limits help: ${FREDPP_LIMITES_HELP_FILE}")
endif()
file(READ "${FREDPP_LIMITES_HELP_FILE}" FREDPP_LIMITES_HELP_CONTENT)
string(FIND "${FREDPP_LIMITES_HELP_CONTENT}" ")FREDPP_HELP\"" FREDPP_LIMITES_HELP_COLLISION)
if(NOT FREDPP_LIMITES_HELP_COLLISION EQUAL -1)
    message(FATAL_ERROR "Limits help contains the reserved raw-string delimiter")
endif()

set(FREDPP_PROCEDURE_HELP_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/fredpp/procedures.md"
)
if(NOT EXISTS "${FREDPP_PROCEDURE_HELP_FILE}")
    message(FATAL_ERROR "Missing FREDPP procedure help: ${FREDPP_PROCEDURE_HELP_FILE}")
endif()
file(READ "${FREDPP_PROCEDURE_HELP_FILE}" FREDPP_PROCEDURE_HELP_CONTENT)
string(FIND "${FREDPP_PROCEDURE_HELP_CONTENT}" ")FREDPP_HELP\"" FREDPP_PROCEDURE_HELP_COLLISION)
if(NOT FREDPP_PROCEDURE_HELP_COLLISION EQUAL -1)
    message(FATAL_ERROR "Procedure help contains the reserved raw-string delimiter")
endif()

set(FREDPP_WU_HELP_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/fredpp/extension-wu.md"
)
if(NOT EXISTS "${FREDPP_WU_HELP_FILE}")
    message(FATAL_ERROR "Missing FREDPP WU help: ${FREDPP_WU_HELP_FILE}")
endif()
file(READ "${FREDPP_WU_HELP_FILE}" FREDPP_WU_HELP_CONTENT)
string(FIND "${FREDPP_WU_HELP_CONTENT}" ")FREDPP_HELP\"" FREDPP_WU_HELP_COLLISION)
if(NOT FREDPP_WU_HELP_COLLISION EQUAL -1)
    message(FATAL_ERROR "WU help contains the reserved raw-string delimiter")
endif()

set(FREDPP_STAR_ALIAS_HELP_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/fr/fredpp/alias-etoile.md"
)
if(NOT EXISTS "${FREDPP_STAR_ALIAS_HELP_FILE}")
    message(FATAL_ERROR "Missing FRED star-alias help: ${FREDPP_STAR_ALIAS_HELP_FILE}")
endif()
file(READ "${FREDPP_STAR_ALIAS_HELP_FILE}" FREDPP_STAR_ALIAS_HELP_CONTENT)
string(FIND "${FREDPP_STAR_ALIAS_HELP_CONTENT}" ")FREDPP_HELP\"" FREDPP_STAR_ALIAS_HELP_COLLISION)
if(NOT FREDPP_STAR_ALIAS_HELP_COLLISION EQUAL -1)
    message(FATAL_ERROR "Star-alias help contains the reserved raw-string delimiter")
endif()

set(FREDPP_EMBEDDED_HELP_HPP "${FREDPP_GENERATED_DIR}/EmbeddedHelp.hpp")
set(FREDPP_EMBEDDED_HELP_CPP "${FREDPP_GENERATED_DIR}/EmbeddedHelp.cpp")

file(WRITE "${FREDPP_EMBEDDED_HELP_HPP}"
"#pragma once\n#include <string>\n#include <string_view>\n#include <vector>\nnamespace fredpp::embedded_help {\nconst std::string_view* find(std::string_view topic) noexcept;\nstd::vector<std::string> topics();\n}\n")

file(WRITE "${FREDPP_EMBEDDED_HELP_CPP}"
"#include \"EmbeddedHelp.hpp\"\n#include <array>\n#include <utility>\nnamespace fredpp::embedded_help {\nnamespace {\nusing Entry = std::pair<std::string_view, std::string_view>;\nstatic constexpr std::array<Entry, ${FREDPP_EMBEDDED_HELP_COUNT}> entries = {{\n"
"    Entry{\"index\", R\"FREDPP_HELP(${FREDPP_HELP_INDEX})FREDPP_HELP\"},\n"
"    Entry{\":\", R\"FREDPP_HELP(${FREDPP_SPECIAL_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"!\", R\"FREDPP_HELP(${FREDPP_BANG_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"*\", R\"FREDPP_HELP(${FREDPP_STAR_ALIAS_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"\\\"\", R\"FREDPP_HELP(${FREDPP_QUOTE_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"=\", R\"FREDPP_HELP(${FREDPP_EQUALS_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"#\", R\"FREDPP_HELP(${FREDPP_COUNTER_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"@\", R\"FREDPP_HELP(${FREDPP_AT_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"window\", R\"FREDPP_HELP(${FREDPP_COLON_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"wait\", R\"FREDPP_HELP(${FREDPP_BACKSLASHBACKSLASH_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"oi(\", R\"FREDPP_HELP(${FREDPP_OI_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"om\", R\"FREDPP_HELP(${FREDPP_OM_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"oq\", R\"FREDPP_HELP(${FREDPP_OQ_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"os&\", R\"FREDPP_HELP(${FREDPP_OS_AMPERSAND_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"osd\", R\"FREDPP_HELP(${FREDPP_OSD_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"ot\", R\"FREDPP_HELP(${FREDPP_OT_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"directives\", R\"FREDPP_HELP(${FREDPP_DIRECTIVES_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"limites\", R\"FREDPP_HELP(${FREDPP_LIMITES_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"procedure\", R\"FREDPP_HELP(${FREDPP_PROCEDURE_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"wu\", R\"FREDPP_HELP(${FREDPP_WU_HELP_CONTENT})FREDPP_HELP\"},\n"
)

foreach(help_file IN LISTS FREDPP_HELP_FILES)
    get_filename_component(help_topic "${help_file}" NAME_WE)
    file(READ "${help_file}" help_content)
    string(FIND "${help_content}" ")FREDPP_HELP\"" help_delimiter_collision)
    if(NOT help_delimiter_collision EQUAL -1)
        message(FATAL_ERROR "Markdown help page contains the reserved raw-string delimiter: ${help_file}")
    endif()
    file(APPEND "${FREDPP_EMBEDDED_HELP_CPP}"
        "    Entry{\"${help_topic}\", R\"FREDPP_HELP(${help_content})FREDPP_HELP\"},\n")
endforeach()

file(APPEND "${FREDPP_EMBEDDED_HELP_CPP}"
"}};\n} // namespace\nconst std::string_view* find(std::string_view topic) noexcept {\n    for (const auto& entry : entries) {\n        if (entry.first == topic) return &entry.second;\n    }\n    return nullptr;\n}\nstd::vector<std::string> topics() {\n    std::vector<std::string> result;\n    result.reserve(entries.size());\n    for (const auto& entry : entries) result.emplace_back(entry.first);\n    return result;\n}\n} // namespace fredpp::embedded_help\n")

add_library(fredpp_core
    src/version.cpp
    src/HelpManager.cpp
    src/Terminal.cpp
    src/HelpPager.cpp
    ${FREDPP_EMBEDDED_HELP_CPP}
    src/core/Buffer.cpp
    src/core/BufferManager.cpp
    src/flow/BufferInputSource.cpp
    src/flow/InputStack.cpp
    src/flow/FlowEngine.cpp
    src/lexer/StringCharacterStream.cpp
    src/lexer/Lexer.cpp
    src/lexer/TokenStream.cpp
    src/lexer/Diagnostic.cpp
    src/parser/AddressParser.cpp
    src/parser/PatternParser.cpp
    src/parser/CommandParser.cpp
    src/command/CommandRegistry.cpp
    src/runtime/ConsoleOutput.cpp
    src/runtime/ExecutionContext.cpp
    src/runtime/CommandExecutor.cpp
    src/runtime/AddressEvaluator.cpp
    src/runtime/ProcedureRunner.cpp
    src/runtime/PatternMatcher.cpp
)

target_include_directories(fredpp_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${FREDPP_GENERATED_DIR}>
)
target_compile_features(fredpp_core PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(fredpp_core PRIVATE /W4 /permissive- /utf-8 /FS)
else()
    target_compile_options(fredpp_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(FREDPP_BUILD_CLI)
    add_executable(fredpp src/cli/main.cpp)
    target_link_libraries(fredpp PRIVATE fredpp_core)
endif()

if(FREDPP_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

if(FREDPP_BUILD_CLI)
    # Installation and release packaging. Portable ZIP/TGZ archives are assembled
    # by scripts/package-release.ps1 and scripts/package-release.sh. CPack provides
    # the native Debian package from the same install manifest.
    include(GNUInstallDirs)

    install(TARGETS fredpp
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    set(FREDPP_INSTALL_DOCDIR "${CMAKE_INSTALL_DATAROOTDIR}/doc/fredpp")
    install(FILES
        LICENSE
        NOTICE
        README.md
        CHANGELOG.md
        ROADMAP.md
        RELEASE_NOTES.md
        docs/project/RELEASES.md
        DESTINATION ${FREDPP_INSTALL_DOCDIR}
    )
    install(FILES packaging/LISEZMOI-DEBIAN.txt
        DESTINATION ${FREDPP_INSTALL_DOCDIR}
        RENAME LISEZMOI.txt
    )

    set(CPACK_PACKAGE_NAME "fredpp")
    set(CPACK_PACKAGE_VENDOR "FREDPP")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
        "Modern C++20 reimplementation of the historical FRED text editor"
    )
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_CONTACT "Patrick Rioche")
    set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/PatrickRioche/FREDPP")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Patrick Rioche")
    set(CPACK_DEBIAN_PACKAGE_SECTION "editors")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/PatrickRioche/FREDPP")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
    # Let CPack derive the native Debian architecture through
    # dpkg --print-architecture. This keeps the same manifest valid for
    # both amd64 and arm64 release runners.
    include(CPack)
endif()
