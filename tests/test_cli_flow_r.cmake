if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${FREDPP_EXE}" "${FIXTURE_DIR}/cli_flow_r.fredpp"
    INPUT_FILE "${FIXTURE_DIR}/cli_flow_r_input.txt"
    RESULT_VARIABLE status
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
    TIMEOUT 15
)

if(NOT status EQUAL 0)
    message(FATAL_ERROR "cli_flow_r failed: ${status}\n${output}\n${errors}")
endif()

string(FIND "${output}" "FIRST=slash/path!\\N(x)" first_at)
if(first_at EQUAL -1)
    message(FATAL_ERROR "command-input R was not literal: ${output}")
endif()

string(FIND "${output}" "raw\\W(literal)" text_at)
if(text_at EQUAL -1)
    message(FATAL_ERROR "procedure text-input R was not literal: ${output}")
endif()

if(NOT "${errors}" STREQUAL "")
    message(FATAL_ERROR "unexpected stderr from cli_flow_r: ${errors}")
endif()

message(STATUS "CLI terminal R flow test passed")
