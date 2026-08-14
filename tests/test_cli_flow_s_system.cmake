if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}" flow_s_system_cli
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

string(REPLACE "\r\n" "\n" output "${output}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "flow_s_system_cli failed (${result})\n"
        "stdout:\n${output}\nstderr:\n${error}")
endif()

string(FIND "${output}" "FREDPP_FLOW_SYSTEM" found)
if(found EQUAL -1)
    message(FATAL_ERROR
        "\\S(buffer) not expanded in ZG system command:\n${output}")
endif()

message(STATUS "Sprint 2.22 Lot 2.1 system S test passed")
