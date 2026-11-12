# End-to-end, disk-backed batch reliability test.
# Runs 100 or 1000 real files through a FREDPP procedure. Never writes to
# the source tree and does not depend on locale-specific temporary paths.
if(NOT DEFINED FREDPP_EXE OR NOT DEFINED TEST_ROOT OR
   NOT DEFINED BATCH_COUNT)
    message(FATAL_ERROR "FREDPP_EXE, TEST_ROOT and BATCH_COUNT are required")
endif()

if(NOT BATCH_COUNT MATCHES "^(100|1000)$")
    message(FATAL_ERROR "Unsupported test batch size: ${BATCH_COUNT}")
endif()

# Every invocation owns a unique workspace beneath the CTest build tree.
# On failure, retain it for inspection; on success, remove only this child.
string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef suffix)
set(root "${TEST_ROOT}/run-${suffix}")
set(inputs "${root}/source files")
file(MAKE_DIRECTORY "${inputs}")

file(TO_CMAKE_PATH "${root}/combined output.txt" output_file)
file(TO_CMAKE_PATH "${root}/batch-procedure.fredpp" procedure_file)
set(procedure "B(batch-results)\n")
set(expected "")

foreach(index RANGE 1 ${BATCH_COUNT})
    # Spaces in filenames ensure the procedure's quoted R operands work.
    set(source_file "${inputs}/sample ${index}.txt")
    set(data "FREDPP-ROW-${index}|alpha-${index}\n")
    file(WRITE "${source_file}" "${data}")
    file(TO_CMAKE_PATH "${source_file}" source_for_fred)
    string(APPEND procedure "$R \"${source_for_fred}\"\n")
    string(APPEND expected "${data}")
endforeach()

string(APPEND procedure "WU \"${output_file}\"\n")
string(APPEND procedure "JM!FREDPP-BATCH-${BATCH_COUNT}-OK!\n")
file(WRITE "${procedure_file}" "${procedure}")

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "FREDPP_PROCEDURE_MAX_SECONDS=60"
        "${FREDPP_EXE}" "${procedure_file}"
    WORKING_DIRECTORY "${root}"
    RESULT_VARIABLE status
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error_output
    TIMEOUT 30
)
string(REPLACE "\r\n" "\n" output "${output}")

if(NOT "${status}" STREQUAL "0" OR
   NOT "${output}" STREQUAL "FREDPP-BATCH-${BATCH_COUNT}-OK\n" OR
   NOT "${error_output}" STREQUAL "")
    message(FATAL_ERROR
        "Batch of ${BATCH_COUNT} files failed (${status}).\n"
        "stdout: ${output}\nstderr: ${error_output}\n"
        "Workspace retained: ${root}")
endif()

if(NOT EXISTS "${output_file}")
    message(FATAL_ERROR "Batch did not create its output: ${root}")
endif()
file(READ "${output_file}" actual)
if(NOT "${actual}" STREQUAL "${expected}")
    message(FATAL_ERROR
        "Byte-for-byte batch output mismatch for ${BATCH_COUNT} files; "
        "workspace retained: ${root}")
endif()

# The input files must remain unchanged after a successful batch.
foreach(index RANGE 1 ${BATCH_COUNT})
    set(source_file "${inputs}/sample ${index}.txt")
    file(READ "${source_file}" actual_source)
    if(NOT "${actual_source}" STREQUAL
       "FREDPP-ROW-${index}|alpha-${index}\n")
        message(FATAL_ERROR "Source file ${index} was altered: ${root}")
    endif()
endforeach()

# Failure injection: a missing source must stop before the WU step,
# without creating an incomplete output file or changing existing inputs.
file(TO_CMAKE_PATH "${inputs}/missing source.txt" missing_file)
file(TO_CMAKE_PATH "${root}/must-not-exist.txt" failed_output)
file(TO_CMAKE_PATH "${root}/failure-procedure.fredpp" failed_procedure)
file(WRITE "${failed_procedure}"
    "B(failed)\n"
    "$R \"${source_for_fred}\"\n"
    "$R \"${missing_file}\"\n"
    "WU \"${failed_output}\"\n")
set(quit_input "${root}/quit.txt")
file(WRITE "${quit_input}" "QQ\n")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env
        "FREDPP_INIT="
        "FREDPP_PROCEDURE_MAX_SECONDS=10"
        "${FREDPP_EXE}" "${failed_procedure}"
    WORKING_DIRECTORY "${root}"
    INPUT_FILE "${quit_input}"
    RESULT_VARIABLE failed_status
    OUTPUT_VARIABLE failed_stdout
    ERROR_VARIABLE failed_stderr
    TIMEOUT 15
)
if(EXISTS "${failed_output}")
    message(FATAL_ERROR
        "Failed batch created an incomplete output file: ${failed_output}")
endif()
string(FIND "${failed_stderr}"
    "procedure stopped; entering interactive debug mode" diagnostic)
if(diagnostic EQUAL -1)
    message(FATAL_ERROR
        "Missing-source failure did not enter controlled debug mode.\n"
        "code: ${failed_status}\n"
        "stdout: ${failed_stdout}\n"
        "stderr: ${failed_stderr}\n"
        "Workspace: ${root}")
endif()
foreach(index RANGE 1 ${BATCH_COUNT})
    set(source_file "${inputs}/sample ${index}.txt")
    file(READ "${source_file}" actual_source)
    if(NOT "${actual_source}" STREQUAL
       "FREDPP-ROW-${index}|alpha-${index}\n")
        message(FATAL_ERROR
            "Failure handling altered input ${index}: ${root}")
    endif()
endforeach()

file(REMOVE_RECURSE "${root}")
message(STATUS
    "Batch reliability passed: ${BATCH_COUNT} source files, exact output, "
    "source integrity and controlled missing-file failure")
