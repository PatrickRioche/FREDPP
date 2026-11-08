# End-to-end Windows CLI regression for historical ZK (insert copy).
if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}" cli_zap_copy
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "ZK CLI procedure failed (${result})\n"
        "stdout:\n${output}\nstderr:\n${error}")
endif()

string(CONCAT expected_output
    "FREDPP-ZK-SOURCE-CURRENT\n"
    "beta\n"
    "FREDPP-ZK-SOURCE-ALL\n"
    "alpha\n"
    "beta\n"
    "gamma\n"
    "FREDPP-ZK-DEST-CURRENT\n"
    "beta\n"
    "FREDPP-ZK-DEST-ALL\n"
    "old-1\n"
    "alpha\n"
    "beta\n"
    "old-2\n"
)

if(NOT output STREQUAL expected_output)
    message(FATAL_ERROR
        "ZK CLI transcript differed.\n"
        "Expected:\n${expected_output}\n"
        "Actual:\n${output}\n"
        "stderr:\n${error}")
endif()

message(STATUS "Historical ZK append-after-cursor Windows test passed")
