cmake_minimum_required(VERSION 3.24)

# Use embedded debug information (/Z7) with MSVC. This avoids concurrent
# compiler access to target PDB files, which can fail on synchronized folders.
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
endif()

project(FREDPP VERSION 0.0.4 LANGUAGES CXX)

if(MSVC)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>"
    )
endif()

option(FREDPP_BUILD_TESTS "Build FREDPP tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Embed only the French reference pages for commands available in FREDPP.
# Files under docs/fr/reference/commandes are source documentation and are
# deliberately never modified by the build.
set(FREDPP_IMPLEMENTED_HELP_TOPICS
    a
    b
    c
    d
    g
    i
    l
    m
    p
    t
    z
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
math(EXPR FREDPP_EMBEDDED_HELP_COUNT "${FREDPP_HELP_FILES_LENGTH} + 1")

string(CONCAT FREDPP_HELP_INDEX
"# Aide FREDPP\n\n"
"Cette aide embarque uniquement les documentations françaises des commandes disponibles dans FREDPP.\n\n"
"## Rubriques embarquées\n\n"
)
foreach(help_topic IN LISTS FREDPP_IMPLEMENTED_HELP_TOPICS)
    string(TOUPPER "${help_topic}" help_topic_upper)
    string(APPEND FREDPP_HELP_INDEX "- `?${help_topic}` — commande ${help_topic_upper}\n")
endforeach()
string(APPEND FREDPP_HELP_INDEX
"\n## Consultation\n\n"
"- `?` ou `?index` : afficher cet index ;\n"
"- `?version` : afficher la version de l'exécutable.\n"
)

set(FREDPP_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${FREDPP_GENERATED_DIR}")
set(FREDPP_EMBEDDED_HELP_HPP "${FREDPP_GENERATED_DIR}/EmbeddedHelp.hpp")
set(FREDPP_EMBEDDED_HELP_CPP "${FREDPP_GENERATED_DIR}/EmbeddedHelp.cpp")

file(WRITE "${FREDPP_EMBEDDED_HELP_HPP}"
"#pragma once\n#include <string>\n#include <string_view>\n#include <vector>\nnamespace fredpp::embedded_help {\nconst std::string_view* find(std::string_view topic) noexcept;\nstd::vector<std::string> topics();\n}\n")

file(WRITE "${FREDPP_EMBEDDED_HELP_CPP}"
"#include \"EmbeddedHelp.hpp\"\n#include <array>\n#include <utility>\nnamespace fredpp::embedded_help {\nnamespace {\nusing Entry = std::pair<std::string_view, std::string_view>;\nstatic constexpr std::array<Entry, ${FREDPP_EMBEDDED_HELP_COUNT}> entries = {{\n"
"    Entry{\"index\", R\"FREDPP_HELP(${FREDPP_HELP_INDEX})FREDPP_HELP\"},\n"
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
