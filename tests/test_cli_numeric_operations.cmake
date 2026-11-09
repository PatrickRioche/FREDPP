# Full interactive regression for N+, N-, N*, N/, N%, N=, N<, N>, N&, N|.
if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}"
    INPUT_FILE "${FIXTURE_DIR}/cli_numeric_operations_input.txt"
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
    TIMEOUT 10
)

string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")

if(NOT result EQUAL 0 OR NOT error STREQUAL "")
    message(FATAL_ERROR
        "Numeric operation CLI failed (${result})\nstdout:\n${output}\nstderr:\n${error}")
endif()

string(FIND "${output}" "n(a):10\nn(b):5\nn(copy):10\n" listing_position)
if(listing_position EQUAL -1)
    message(FATAL_ERROR
        "Missing expected interactive numeric register values:\n${output}")
endif()

message(STATUS "Historical numeric operations CLI regression passed")
