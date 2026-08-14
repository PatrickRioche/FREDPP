if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}" jump_label_cli
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "jump_label_cli failed (${result})\n"
        "stdout:\n${output}\n"
        "stderr:\n${error}")
endif()

if(NOT output MATCHES "LOT3-BEFORE")
    message(FATAL_ERROR "pre-jump message missing:\n${output}")
endif()

if(output MATCHES "LOT3-SHOULD-NOT-PRINT")
    message(FATAL_ERROR "J(label) did not skip input:\n${output}")
endif()

if(NOT output MATCHES "LOT3-AFTER")
    message(FATAL_ERROR "target label continuation missing:\n${output}")
endif()

message(STATUS
    "Sprint 2.22 Lot 3 J(label)/@(label) CLI test passed")
