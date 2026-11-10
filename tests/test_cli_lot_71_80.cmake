# End-to-end test for prompt overrides, input escapes, and last-error recall.
if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env "FREDPP_INIT=" "${FREDPP_EXE}"
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    INPUT_FILE "${FIXTURE_DIR}/cli_lot_71_80_input.txt"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)
string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR "Lot 71-80 CLI exited with ${result}: ${error}")
endif()

foreach(fragment IN ITEMS "CMD> " "TYPE> " "RETURN" "Minuscule\n" "mAJUSCULE\n"
                          "OIL and OIU cannot use the same escape")
    string(FIND "${output}" "${fragment}" found)
    if(found EQUAL -1)
        message(FATAL_ERROR
            "Missing CLI fragment '${fragment}' in output:\n${output}\nstderr:\n${error}")
    endif()
endforeach()
string(FIND "${error}" "OIL and OIU cannot use the same escape" expected_error)
if(expected_error EQUAL -1)
    message(FATAL_ERROR "Expected OIU conflict error on stderr: ${error}")
endif()

message(STATUS "FREDPP lot 71-80 interactive CLI test passed")
