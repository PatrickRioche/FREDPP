if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}"
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    INPUT_FILE "${FIXTURE_DIR}/flow_s_global_interactive_input.txt"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "interactive G flow-S returned ${result}\n"
        "stdout:\n${output}\n"
        "stderr:\n${error}")
endif()

if(error MATCHES "current buffer is empty")
    message(FATAL_ERROR
        "interactive G~/\\S(model)/D emptied the buffer:\n"
        "stdout:\n${output}\n"
        "stderr:\n${error}")
endif()

string(FIND "${output}" "C:\\FREDPP\\fredpp.exe" exe_pos)
if(exe_pos EQUAL -1)
    message(FATAL_ERROR
        "interactive G~/\\S(model)/D did not preserve .exe line:\n${output}")
endif()

string(FIND "${output}" "C:\\FREDPP\\LICENSE" license_pos)
if(NOT license_pos EQUAL -1)
    message(FATAL_ERROR
        "interactive G~/\\S(model)/D did not delete LICENSE line:\n${output}")
endif()

string(FIND "${output}" "hello.fredpp" fredpp_pos)
if(NOT fredpp_pos EQUAL -1)
    message(FATAL_ERROR
        "interactive G~/\\S(model)/D did not delete fredpp line:\n${output}")
endif()

message(STATUS
    "Sprint 2.22 Lot 2.3 v3 interactive G flow-S test passed")
