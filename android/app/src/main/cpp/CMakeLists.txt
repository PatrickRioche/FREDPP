cmake_minimum_required(VERSION 3.24)
project(fredpp_android LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# This file lives in android/app/src/main/cpp. Five parents lead to the
# FREDPP repository root.
get_filename_component(FREDPP_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE)

if(NOT EXISTS "${FREDPP_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR "FREDPP repository root not found: ${FREDPP_ROOT}")
endif()

set(FREDPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FREDPP_BUILD_CLI OFF CACHE BOOL "" FORCE)
add_subdirectory("${FREDPP_ROOT}"
                 "${CMAKE_CURRENT_BINARY_DIR}/fredpp-core")

add_library(fredpp_android SHARED native-lib.cpp)
target_link_libraries(fredpp_android PRIVATE fredpp_core log android)
target_compile_features(fredpp_android PRIVATE cxx_std_20)
target_compile_options(fredpp_android PRIVATE -Wall -Wextra -Wpedantic)
