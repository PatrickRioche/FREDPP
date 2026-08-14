if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}"
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    INPUT_FILE "${FIXTURE_DIR}/message_delimiters_interactive_input.txt"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "message delimiter test failed (${result})\n"
        "stdout:\n${output}\n"
        "stderr:\n${error}")
endif()

if(error MATCHES "error:")
    message(FATAL_ERROR "unexpected error:\n${error}")
endif()

foreach(expected "LOT5-JM" "LOT5-JP" "LOT5-END")
    if(NOT output MATCHES "${expected}")
        message(FATAL_ERROR
            "missing ${expected}:\n${output}")
    endif()
endforeach()

message(STATUS
    "Sprint 2.22 Lot 5 JM/JP arbitrary delimiters passed")
