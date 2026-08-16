cmake_minimum_required(VERSION 3.24)

# Use embedded debug information (/Z7) with MSVC. This avoids concurrent
# compiler access to target PDB files, which can fail on synchronized folders.
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
endif()
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(FREDPP VERSION 0.0.19 LANGUAGES CXX)

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
    fb
    fo
    g
    i
    jm
    jp
    l
    m
    p
    q
    r
    s
    t
    w
    z
    zg
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
math(EXPR FREDPP_EMBEDDED_HELP_COUNT "${FREDPP_HELP_FILES_LENGTH} + 9")

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
    string(LENGTH "${help_command}" help_command_length)
    math(EXPR help_padding_length "12 - ${help_command_length}")
    string(REPEAT " " ${help_padding_length} help_padding)
    string(APPEND FREDPP_HELP_INDEX
        "    ${help_command}${help_padding}Commande ${help_topic_upper}\n")
endforeach()
string(APPEND FREDPP_HELP_INDEX
"\nAUTRES AIDES\n\n"
"    ?           Afficher cette liste\n"
"    ?!          Afficher l'aide historique de la commande !\n"
"\nOPTION HISTORIQUE IMPLÉMENTÉE\n\n"
"    ?oi(        Afficher l'aide de l'option OI(\n"
"    ?om         Afficher l'aide de l'option OM (monitorage)\n"
"\nCOMMANDE SPÉCIALE HISTORIQUE IMPLÉMENTÉE\n\n"
"    ?\"          Afficher l'aide des commentaires FRED\n"
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
file(READ "${FREDPP_OM_HELP_FILE}" FREDPP_OM_HELP_CONTENT)
string(FIND "${FREDPP_OM_HELP_CONTENT}" ")FREDPP_HELP\"" FREDPP_OM_HELP_COLLISION)
if(NOT FREDPP_OM_HELP_COLLISION EQUAL -1)
    message(FATAL_ERROR "OM help contains the reserved raw-string delimiter")
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
"    Entry{\"oi(\", R\"FREDPP_HELP(${FREDPP_OI_HELP_CONTENT})FREDPP_HELP\"},\n"
"    Entry{\"om\", R\"FREDPP_HELP(${FREDPP_OM_HELP_CONTENT})FREDPP_HELP\"},\n"
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

add_executable(fredpp src/cli/main.cpp)
target_link_libraries(fredpp PRIVATE fredpp_core)

if(FREDPP_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

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
