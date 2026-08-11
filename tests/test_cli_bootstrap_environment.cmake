if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${FREDPP_EXE}" bootstrap
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "fredpp bootstrap failed (${result})\nstdout:\n${output}\nstderr:\n${error}")
endif()

string(REPLACE "\r\n" "\n" output "${output}")

string(REGEX MATCH
    "^[0-9][0-9]/[0-9][0-9]/[0-9][0-9]\n[0-9][0-9]:[0-9][0-9]\n[^\n]+\n$"
    matched
    "${output}"
)

if(NOT matched STREQUAL output)
    message(FATAL_ERROR
        "historical bootstrap buffers have unexpected format:\n${output}")
endif()

message(STATUS "Sprint 2.21 Lot 2 bootstrap environment tests passed")
