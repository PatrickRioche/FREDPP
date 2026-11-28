# Exercise the terminal entry point used by the Windows PowerShell regression.
if(NOT DEFINED FREDPP_EXE)
    message(FATAL_ERROR "FREDPP_EXE is required")
endif()

set(input_file "${CMAKE_CURRENT_BINARY_DIR}/substitute_occurrences_input.txt")
file(WRITE "${input_file}"
    "a\nchat chat chat chat\n\\F\n1p\n1s1/chat/A/p\n1s-2/chat/B/p\nqq\n")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env "FREDPP_INIT=" "${FREDPP_EXE}"
    INPUT_FILE "${input_file}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)
file(REMOVE "${input_file}")
string(REPLACE "\r\n" "\n" output "${output}")
if(NOT result EQUAL 0 OR NOT error STREQUAL "")
    message(FATAL_ERROR "Numbered S CLI failed (${result}): ${error}\n${output}")
endif()

set(previous_position -1)
foreach(expected IN ITEMS "chat chat chat chat\n" "A chat chat chat\n" "A chat B chat\n")
    string(FIND "${output}" "${expected}" position)
    if(position LESS_EQUAL previous_position)
        message(FATAL_ERROR "Missing or out-of-order '${expected}' in:\n${output}")
    endif()
    set(previous_position ${position})
endforeach()
