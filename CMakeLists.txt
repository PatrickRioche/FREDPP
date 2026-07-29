cmake_minimum_required(VERSION 3.24)

# Use embedded debug information (/Z7) with MSVC. This avoids concurrent
# compiler access to target PDB files, which can fail on synchronized folders.
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
endif()

project(FREDPP VERSION 0.0.2 LANGUAGES CXX)

if(MSVC)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>"
    )
endif()

option(FREDPP_BUILD_TESTS "Build FREDPP tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(fredpp_core
    src/version.cpp
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
)

target_include_directories(fredpp_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
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
