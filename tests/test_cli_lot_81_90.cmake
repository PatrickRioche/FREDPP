# Interactive host-terminal test: inline A, alternate terminator, OC, OP and modes.
if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env "FREDPP_INIT=" "${FREDPP_EXE}"
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    INPUT_FILE "${FIXTURE_DIR}/cli_lot_81_90_input.txt"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)
string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")
if(NOT result EQUAL 0)
    message(FATAL_ERROR "FREDPP lot 81-90 CLI failed (${result}): ${error}")
endif()

foreach(fragment IN ITEMS "Inline\n" "Second\n" "Changed\n"
                          "-- text input; finish with !F --"
                          "-- page suivante --" "o-if" "RETOUR")
    string(FIND "${output}" "${fragment}" found)
    if(found EQUAL -1)
        message(FATAL_ERROR
            "Missing '${fragment}' in CLI transcript:\n${output}\nstderr:\n${error}")
    endif()
endforeach()
if(NOT error STREQUAL "")
    message(FATAL_ERROR "Unexpected CLI errors: ${error}")
endif()

message(STATUS "FREDPP lot 81-90 interactive CLI test passed")
