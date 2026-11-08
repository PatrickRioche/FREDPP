# End-to-end Windows CLI test of the historical = line-number command.
if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}" cli_line_number
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "= CLI procedure failed (${result})\n"
        "stdout:\n${output}\nstderr:\n${error}")
endif()

string(CONCAT expected_output
    "FREDPP-EQUALS-BEGIN\n"
    "1\n"
    "3\n"
    "2\n"
    "1\n"
    "FREDPP-EQUALS-END\n"
)

if(NOT output STREQUAL expected_output)
    message(FATAL_ERROR
        "= CLI transcript differed.\n"
        "Expected:\n${expected_output}\n"
        "Actual:\n${output}\n"
        "stderr:\n${error}")
endif()

message(STATUS "Historical = line-number Windows CLI test passed")
