if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}" bootstrap_facts
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

string(REPLACE "\r\n" "\n" output "${output}")

if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "fredpp bootstrap_facts failed (${result})\n"
        "stdout:\n${output}\nstderr:\n${error}")
endif()

foreach(name IN ITEMS d t u)
    string(FIND "${output}" "b(${name}) " position)
    if(position EQUAL -1)
        message(FATAL_ERROR
            "FB does not list bootstrap buffer B(${name}):\n${output}")
    endif()
endforeach()

message(STATUS
    "Sprint 2.22 Lot 1 FB bootstrap visibility test passed")
