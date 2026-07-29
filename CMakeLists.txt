cmake_minimum_required(VERSION 3.24)
project(FREDPP VERSION 0.0.1 LANGUAGES CXX)

option(FREDPP_BUILD_TESTS "Build FREDPP tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(fredpp_core
    src/version.cpp
)

target_include_directories(fredpp_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(fredpp_core PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(fredpp_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(fredpp_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(fredpp src/main.cpp)
target_link_libraries(fredpp PRIVATE fredpp_core)

if(FREDPP_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
