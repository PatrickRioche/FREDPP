if(NOT DEFINED FREDPP_EXE)
    message(FATAL_ERROR "FREDPP_EXE is required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "FREDPP_PROCEDURE_MAX_STEPS="
        "FREDPP_PROCEDURE_MAX_SECONDS="
        "FREDPP_REPEAT_MAX_STEPS="
        "FREDPP_REPEAT_MAX_SECONDS="
        "${FREDPP_EXE}" --limits
    RESULT_VARIABLE status
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
)
if(NOT status EQUAL 0 OR NOT errors STREQUAL "")
    message(FATAL_ERROR "Default --limits failed: ${status}: ${errors}")
endif()
foreach(expected IN ITEMS
    "FREDPP_PROCEDURE_MAX_STEPS = 1000000 (default)"
    "FREDPP_PROCEDURE_MAX_SECONDS = 60 (default)"
    "FREDPP_REPEAT_MAX_STEPS = 1000000 (default)"
    "FREDPP_REPEAT_MAX_SECONDS = 60 (default)"
    "Maximum nested procedure levels = 256"
    "Maximum nested U levels = 16")
    string(FIND "${output}" "${expected}" at)
    if(at EQUAL -1)
        message(FATAL_ERROR "Missing default limit '${expected}':\n${output}")
    endif()
endforeach()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "FREDPP_PROCEDURE_MAX_STEPS=101"
        "FREDPP_PROCEDURE_MAX_SECONDS=20"
        "FREDPP_REPEAT_MAX_STEPS=5"
        "FREDPP_REPEAT_MAX_SECONDS=30"
        "${FREDPP_EXE}" --limits
    RESULT_VARIABLE status
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
)
if(NOT status EQUAL 0 OR NOT errors STREQUAL "")
    message(FATAL_ERROR "Overridden --limits failed: ${status}: ${errors}")
endif()
foreach(expected IN ITEMS
    "FREDPP_PROCEDURE_MAX_STEPS = 101 (environment)"
    "FREDPP_PROCEDURE_MAX_SECONDS = 20 (environment)"
    "FREDPP_REPEAT_MAX_STEPS = 5 (environment)"
    "FREDPP_REPEAT_MAX_SECONDS = 30 (environment)")
    string(FIND "${output}" "${expected}" at)
    if(at EQUAL -1)
        message(FATAL_ERROR "Missing overridden limit '${expected}':\n${output}")
    endif()
endforeach()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_REPEAT_MAX_SECONDS=86401"
        "${FREDPP_EXE}" --limits
    RESULT_VARIABLE status
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
)
if(status EQUAL 0)
    message(FATAL_ERROR "Invalid limit must fail: ${output}")
endif()
string(FIND "${errors}" "FREDPP_REPEAT_MAX_SECONDS must not exceed 86400" at)
if(at EQUAL -1)
    message(FATAL_ERROR "Invalid limit error was not reported: ${errors}")
endif()
message(STATUS "CLI runtime limits test passed")
