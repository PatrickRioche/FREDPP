if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}"
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    INPUT_FILE "${FIXTURE_DIR}/command_chaining_interactive_input.txt"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "interactive chaining failed (${result})\n"
        "stdout:\n${output}\n"
        "stderr:\n${error}")
endif()

if(output MATCHES "unexpected token after command" OR
   error MATCHES "unexpected token after command")
    message(FATAL_ERROR
        "old single-command parser error remains:\n${output}\n${error}")
endif()

if(NOT output MATCHES "alpha")
    message(FATAL_ERROR
        "B(buff) A did not append/print alpha:\n${output}")
endif()

message(STATUS
    "Sprint 2.22 Lot 4.1 interactive command chaining passed")
