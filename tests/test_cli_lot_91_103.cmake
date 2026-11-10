# CLI smoke: named models, repeat, undo/redo and portable O options.
if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env "FREDPP_INIT=" "${FREDPP_EXE}"
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    INPUT_FILE "${FIXTURE_DIR}/cli_lot_91_103_input.txt"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)
string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Lot 91-103 CLI process failed (${result}): ${error}")
endif()

foreach(fragment IN ITEMS "e(word)/Alpha/" "LOOP103\nLOOP103\nLOOP103"
                          "Alpha\nBravo\n" "o-r")
    string(FIND "${output}" "${fragment}" found)
    if(found EQUAL -1)
        message(FATAL_ERROR "Missing '${fragment}' in CLI:\n${output}\nstderr:\n${error}")
    endif()
endforeach()
if(NOT error STREQUAL "")
    message(FATAL_ERROR "Unexpected CLI diagnostics: ${error}")
endif()

message(STATUS "FREDPP lot 91-103 CLI test passed")
