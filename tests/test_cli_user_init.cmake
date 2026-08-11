if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT=${FIXTURE_DIR}/user_init.fredpp"
        "${FREDPP_EXE}" init_target after-init
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)
string(REPLACE "\r\n" "\n" output "${output}")
if(NOT result EQUAL 0)
    message(FATAL_ERROR
        "user init execution failed (${result})\nstdout:\n${output}\nstderr:\n${error}")
endif()
if(NOT output STREQUAL "from-init\nafter-init\n")
    message(FATAL_ERROR
        "user init did not run before main program:\n${output}")
endif()

file(MAKE_DIRECTORY "${FIXTURE_DIR}/profile-without-init")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        --unset=FREDPP_INIT
        "USERPROFILE=${FIXTURE_DIR}/profile-without-init"
        "HOME=${FIXTURE_DIR}/profile-without-init"
        "${FREDPP_EXE}" hello
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)
string(REPLACE "\r\n" "\n" output "${output}")
if(NOT result EQUAL 0 OR NOT output STREQUAL "HELLO PROCEDURE\n")
    message(FATAL_ERROR
        "missing default init must be ignored (${result})\nstdout:\n${output}\nstderr:\n${error}")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT=${FIXTURE_DIR}/bad_init.fredpp"
        "${FREDPP_EXE}" hello
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)
if(result EQUAL 0)
    message(FATAL_ERROR "invalid user init unexpectedly succeeded")
endif()
if(NOT error MATCHES "user init failed")
    message(FATAL_ERROR
        "invalid user init diagnostic is unexpected:\n${error}")
endif()
if(output MATCHES "HELLO PROCEDURE")
    message(FATAL_ERROR
        "main program ran even though user init failed")
endif()

message(STATUS "Sprint 2.21 Lot 3 user init tests passed")
