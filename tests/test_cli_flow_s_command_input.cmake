if(NOT DEFINED FREDPP_EXE OR NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FREDPP_EXE and FIXTURE_DIR are required")
endif()

set(output_file "${FIXTURE_DIR}/flow_s_io_written.txt")
file(REMOVE "${output_file}")

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "${FREDPP_EXE}"
    WORKING_DIRECTORY "${FIXTURE_DIR}"
    INPUT_FILE "${FIXTURE_DIR}/flow_s_command_input.txt"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

string(REPLACE "\r\n" "\n" output "${output}")
string(REPLACE "\r\n" "\n" error "${error}")

if(NOT result EQUAL 0)
    file(REMOVE "${output_file}")
    message(FATAL_ERROR
        "command-input flow-S returned ${result}\n"
        "stdout:\n${output}\n"
        "stderr:\n${error}")
endif()

string(FIND "${output}" "BUFFER_TARGET" buffer_pos)
if(buffer_pos EQUAL -1)
    file(REMOVE "${output_file}")
    message(FATAL_ERROR
        "B(\\S(nombuf)) did not select the resolved buffer:\n${output}")
endif()

string(FIND "${output}" "FLOW_IO_SOURCE" read_pos)
if(read_pos EQUAL -1)
    file(REMOVE "${output_file}")
    message(FATAL_ERROR
        "R \\S(procedure).txt did not read the resolved filename:\n${output}")
endif()

if(NOT EXISTS "${output_file}")
    message(FATAL_ERROR
        "W \\S(outname).txt did not create the resolved output file")
endif()

file(READ "${output_file}" written)
file(REMOVE "${output_file}")

string(FIND "${written}" "FLOW_IO_SOURCE" write_pos)
if(write_pos EQUAL -1)
    message(FATAL_ERROR
        "W \\S(outname).txt wrote unexpected content:\n${written}")
endif()

message(STATUS
    "v0.1 command-input flow-S B/R/W test passed")
