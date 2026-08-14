if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}" numeric_jump_cli
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE r0
    OUTPUT_VARIABLE o0
    ERROR_VARIABLE e0
)

string(REPLACE "\r\n" "\n" o0 "${o0}")
string(REPLACE "\r\n" "\n" e0 "${e0}")

if(NOT r0 EQUAL 0)
    message(FATAL_ERROR
        "no-arg run failed (${r0})\nstdout:\n${o0}\nstderr:\n${e0}")
endif()

if(NOT o0 MATCHES "LOT4-NO-PARAM")
    message(FATAL_ERROR "false branch missing:\n${o0}")
endif()

if(o0 MATCHES "LOT4-HAS-PARAM")
    message(FATAL_ERROR "false branch jumped unexpectedly:\n${o0}")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}" numeric_jump_cli hello
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE r1
    OUTPUT_VARIABLE o1
    ERROR_VARIABLE e1
)

string(REPLACE "\r\n" "\n" o1 "${o1}")
string(REPLACE "\r\n" "\n" e1 "${e1}")

if(NOT r1 EQUAL 0)
    message(FATAL_ERROR
        "arg run failed (${r1})\nstdout:\n${o1}\nstderr:\n${e1}")
endif()

if(NOT o1 MATCHES "LOT4-HAS-PARAM")
    message(FATAL_ERROR "true branch missing:\n${o1}")
endif()

if(o1 MATCHES "LOT4-NO-PARAM")
    message(FATAL_ERROR "true branch executed false path:\n${o1}")
endif()

message(STATUS "Sprint 2.22 Lot 4 numeric jump test passed")
