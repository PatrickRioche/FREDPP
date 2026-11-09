# End-to-end interactive regression for N: followed by FN (no procedure).
if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}"
    INPUT_FILE "${FIXTURE_DIR}/cli_numeric_assignment_input.txt"
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
    TIMEOUT 10
)

string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "N:/FN interactive CLI failed (${result})\n"
        "stdout:\n${output}\nstderr:\n${error}")
endif()

if(NOT error STREQUAL "")
    message(FATAL_ERROR "Unexpected stderr for N:/FN:\n${error}")
endif()

string(FIND "${output}" "n(alpha):5\nn(beta):10\n" transcript_position)
if(transcript_position EQUAL -1)
    message(FATAL_ERROR
        "N:/FN interactive transcript missing the sorted register listing.\n"
        "Actual:\n${output}")
endif()

message(STATUS "Historical N: interactive assignment and FN listing passed")
