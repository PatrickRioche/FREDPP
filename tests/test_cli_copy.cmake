# End-to-end Windows CLI regression for historical K (copy to Buffer).
if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}" cli_copy
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "K CLI procedure failed (${result})\n"
        "stdout:\n${output}\nstderr:\n${error}")
endif()

string(CONCAT expected_output
    "FREDPP-K-SOURCE-CURRENT\n"
    "beta\n"
    "FREDPP-K-SOURCE-ALL\n"
    "alpha\n"
    "beta\n"
    "gamma\n"
    "FREDPP-K-DEST-CURRENT\n"
    "beta\n"
    "FREDPP-K-DEST-ALL\n"
    "alpha\n"
    "beta\n"
)

if(NOT output STREQUAL expected_output)
    message(FATAL_ERROR
        "K CLI transcript differed.\n"
        "Expected:\n${expected_output}\n"
        "Actual:\n${output}\n"
        "stderr:\n${error}")
endif()

message(STATUS "Historical K copy/replacement Windows test passed")
