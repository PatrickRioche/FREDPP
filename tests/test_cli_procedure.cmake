if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(COMMAND "${FREDPP_EXE}" hello
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE r OUTPUT_VARIABLE o ERROR_VARIABLE e)
string(REPLACE "\r\n" "\n" o "${o}")
if(NOT r EQUAL 0 OR NOT o STREQUAL "HELLO PROCEDURE\n")
    message(FATAL_ERROR "fredpp hello failed (${r})\nstdout:${o}\nstderr:${e}")
endif()

execute_process(COMMAND "${FREDPP_EXE}" args alpha beta "gamma delta"
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE r OUTPUT_VARIABLE o ERROR_VARIABLE e)
string(REPLACE "\r\n" "\n" o "${o}")
if(NOT r EQUAL 0 OR NOT o STREQUAL "alpha\nbeta\ngamma delta\n")
    message(FATAL_ERROR "B(0) arguments failed (${r})\nstdout:${o}\nstderr:${e}")
endif()

execute_process(COMMAND "${FREDPP_EXE}" __fredpp_missing_sprint_221__
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE r OUTPUT_VARIABLE o ERROR_VARIABLE e)
if(r EQUAL 0 OR NOT e MATCHES "procedure not found")
    message(FATAL_ERROR "missing procedure diagnostic failed\n${e}")
endif()

message(STATUS "Sprint 2.21 CLI procedure bootstrap tests passed")
