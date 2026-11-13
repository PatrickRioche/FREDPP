# End-to-end test for the distribution library's ECFP batch procedure.
# Every test run gets a private workspace under the build tree.
if(NOT DEFINED FREDPP_EXE OR NOT DEFINED ECFP_PROCEDURE OR
   NOT DEFINED TEST_ROOT)
    message(FATAL_ERROR "FREDPP_EXE, ECFP_PROCEDURE and TEST_ROOT required")
endif()
if(NOT EXISTS "${ECFP_PROCEDURE}")
    message(FATAL_ERROR "Missing bundled library procedure: ${ECFP_PROCEDURE}")
endif()

string(RANDOM LENGTH 10 ALPHABET 0123456789abcdef suffix)
set(root "${TEST_ROOT}/ecfp-${suffix}")
file(MAKE_DIRECTORY "${root}")

# Paths deliberately contain whitespace to exercise real Windows paths.
set(first "${root}/source one.txt")
set(second "${root}/source two.txt")
set(list_file "${root}/file list.txt")
set(command_file "${root}/command script.fredpp")
set(quit_file "${root}/quit.txt")

file(WRITE "${first}" "alpha ONE\n")
file(WRITE "${second}" "alpha TWO\n")
file(TO_CMAKE_PATH "${first}" first_path)
file(TO_CMAKE_PATH "${second}" second_path)
file(WRITE "${list_file}" "${first_path}\n${second_path}\n")
file(WRITE "${command_file}" "S/alpha/omega/\n")
file(WRITE "${quit_file}" "QQ\n")

function(run_ecfp mode status_var stdout_var stderr_var)
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E env "FREDPP_INIT="
            "${FREDPP_EXE}" "${ECFP_PROCEDURE}"
            "${list_file}" "${command_file}" "${mode}"
        WORKING_DIRECTORY "${root}"
        INPUT_FILE "${quit_file}"
        RESULT_VARIABLE status
        OUTPUT_VARIABLE stdout
        ERROR_VARIABLE stderr
        TIMEOUT 20
    )
    string(REPLACE "\r\n" "\n" stdout "${stdout}")
    string(REPLACE "\r\n" "\n" stderr "${stderr}")
    set(${status_var} "${status}" PARENT_SCOPE)
    set(${stdout_var} "${stdout}" PARENT_SCOPE)
    set(${stderr_var} "${stderr}" PARENT_SCOPE)
endfunction()

run_ecfp("VOIR" status output errors)
if(NOT "${status}" STREQUAL "0" OR NOT "${errors}" STREQUAL "" OR
   NOT "${output}" MATCHES "omega ONE" OR
   NOT "${output}" MATCHES "omega TWO" OR
   NOT "${output}" MATCHES "nombre de fichiers traites : 2")
    message(FATAL_ERROR
        "ECFP VOIR failed: ${status}\nstdout: ${output}\nstderr: ${errors}\n"
        "Workspace: ${root}")
endif()
file(READ "${first}" actual_first)
file(READ "${second}" actual_second)
if(NOT "${actual_first}" STREQUAL "alpha ONE\n" OR
   NOT "${actual_second}" STREQUAL "alpha TWO\n")
    message(FATAL_ERROR "ECFP VOIR modified a source: ${root}")
endif()

run_ecfp("FAIRE" status output errors)
if(NOT "${status}" STREQUAL "0" OR NOT "${errors}" STREQUAL "" OR
   NOT "${output}" MATCHES "ECFP FAIRE : fichier enregistre." OR
   NOT "${output}" MATCHES "nombre de fichiers traites : 2")
    message(FATAL_ERROR
        "ECFP FAIRE failed: ${status}\nstdout: ${output}\nstderr: ${errors}\n"
        "Workspace: ${root}")
endif()
file(READ "${first}" actual_first)
file(READ "${second}" actual_second)
if(NOT "${actual_first}" STREQUAL "omega ONE\n" OR
   NOT "${actual_second}" STREQUAL "omega TWO\n")
    message(FATAL_ERROR
        "ECFP FAIRE did not write expected data:\n"
        "first: [${actual_first}] second: [${actual_second}]\n"
        "Workspace: ${root}")
endif()

run_ecfp("INVALID" status output errors)
if(NOT "${status}" STREQUAL "0" OR NOT "${errors}" STREQUAL "" OR
   NOT "${output}" MATCHES "troisieme argument invalide")
    message(FATAL_ERROR
        "ECFP invalid-mode guard failed: ${status}\n"
        "stdout: ${output}\nstderr: ${errors}\nWorkspace: ${root}")
endif()
file(READ "${first}" actual_first)
file(READ "${second}" actual_second)
if(NOT "${actual_first}" STREQUAL "omega ONE\n" OR
   NOT "${actual_second}" STREQUAL "omega TWO\n")
    message(FATAL_ERROR "Invalid mode modified a source: ${root}")
endif()

file(REMOVE_RECURSE "${root}")
message(STATUS "ECFP VOIR/FAIRE/invalid-mode and path-with-spaces checks passed")
