# Windows-focused end-to-end regression for T + conditional procedure jumps.
# T itself is silent. J(label)T/F exposes its condition as a visible JM message.
if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}" cli_test_condition_t
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "T/J integration procedure failed (${result})\n"
        "stdout:\n${output}\nstderr:\n${error}")
endif()

string(CONCAT expected_output
"FREDPP-T-TRUE-1\n"
"FREDPP-T-FALSE-2\n"
"FREDPP-T-RANGE-3\n"
"FREDPP-T-INVERT-4\n"
"FREDPP-T-INVERT-FALSE-5\n"
"FREDPP-T-BUFFER-BEGIN\n"
"alpha\n"
"beta\n"
"gamma\n"
"FREDPP-T-BUFFER-END\n"
)
if(NOT output STREQUAL expected_output)
    message(FATAL_ERROR
        "T/J visible result differed from expected transcript.\n"
        "Expected:\n${expected_output}\n"
        "Actual:\n${output}\n"
        "stderr:\n${error}")
endif()

message(STATUS "Historical T -> J(label)T/F visible integration test passed")
