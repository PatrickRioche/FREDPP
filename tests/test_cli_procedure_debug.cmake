if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}" procedure_debug_error
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    INPUT_FILE "${FIXTURE_DIR}/procedure_debug_input.txt"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "interactive recovery returned ${result}\n"
        "stdout:\n${output}\n"
        "stderr:\n${error}")
endif()

if(NOT error MATCHES "error: current buffer is empty")
    message(FATAL_ERROR
        "expected procedure error was not reported:\n${error}")
endif()

if(NOT error MATCHES "entering interactive debug mode")
    message(FATAL_ERROR
        "interactive recovery diagnostic missing:\n${error}")
endif()

if(NOT output MATCHES "procedure stopped; remaining input:")
    message(FATAL_ERROR
        "historical remaining-input preview missing:\n${output}")
endif()

if(NOT output MATCHES "1m\\(x\\)")
    message(FATAL_ERROR
        "failing procedure line missing from preview:\n${output}")
endif()

if(NOT output MATCHES "\\.\\.\\.")
    message(FATAL_ERROR
        "preview did not abbreviate remaining input:\n${output}")
endif()

if(NOT output MATCHES "KEPT_AFTER_ERROR")
    message(FATAL_ERROR
        "buffer state was not preserved for interactive debugging:\n${output}")
endif()

message(STATUS
    "Sprint 2.22 Lot 2.2 interactive procedure recovery test passed")
