if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}" "${FIXTURE_DIR}/cli_bootstrap_q_error.fredpp"
    INPUT_FILE "${FIXTURE_DIR}/cli_bootstrap_q_input.txt"
    RESULT_VARIABLE status
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
)

if(NOT status EQUAL 0)
    message(FATAL_ERROR "Q after procedure error did not exit cleanly: ${status}\n${output}\n${errors}")
endif()
string(FIND "${output}" "procedure stopped; entering interactive debug mode" at)
if(at EQUAL -1)
    message(FATAL_ERROR "Fixture did not trigger debug mode: ${output}")
endif()
string(FIND "${errors}" "modified buffer(s):" at)
if(NOT at EQUAL -1)
    message(FATAL_ERROR "Q falsely reports bootstrap buffers as dirty: ${errors}")
endif()
message(STATUS "CLI bootstrap Q test passed")
