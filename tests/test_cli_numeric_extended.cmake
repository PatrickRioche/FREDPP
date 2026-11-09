# End-to-end interactive coverage of all ten historical N commands 41-50.
if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}"
    INPUT_FILE "${FIXTURE_DIR}/cli_numeric_extended_input.txt"
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
        "Extended numeric CLI failed (${result})\nstdout:\n${output}\nstderr:\n${error}")
endif()

string(FIND "${output}" "n(bit):.1110\nn(chars):6\n" listing_position)
if(listing_position EQUAL -1)
    message(FATAL_ERROR
        "Expected formatted numeric values and UTF-8 character count missing:\n${output}")
endif()

message(STATUS "Historical numeric 41-50 CLI regression passed")
