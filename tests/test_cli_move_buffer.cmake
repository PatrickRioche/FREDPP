if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}" move_buffer_cli first second
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

string(REPLACE "\r\n" "\n" output "${output}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "move_buffer_cli failed (${result})\n"
        "stdout:\n${output}\n"
        "stderr:\n${error}")
endif()

string(FIND "${output}" "first" first_position)
string(FIND "${output}" "second" second_position)

if(first_position EQUAL -1 OR second_position EQUAL -1)
    message(FATAL_ERROR
        "M(buffer) CLI output incomplete:\n${output}")
endif()

message(STATUS "Sprint 2.22 Lot 2 CLI M(buffer) test passed")
