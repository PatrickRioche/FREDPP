# Check that fp.exe is identical to fredpp.exe (Debug and Release).
if(NOT DEFINED FREDPP_EXE OR NOT EXISTS "${FREDPP_EXE}")
    message(FATAL_ERROR "Canonical FREDPP executable missing")
endif()
get_filename_component(exe_dir "${FREDPP_EXE}" DIRECTORY)
set(alias_exe "${exe_dir}/fp.exe")
if(NOT EXISTS "${alias_exe}")
    message(FATAL_ERROR "Missing Windows fp.exe alias: ${alias_exe}")
endif()
file(SHA256 "${FREDPP_EXE}" canonical_sha)
file(SHA256 "${alias_exe}" alias_sha)
if(NOT canonical_sha STREQUAL alias_sha)
    message(FATAL_ERROR "fp.exe must exactly match fredpp.exe (SHA-256)")
endif()
execute_process(
    COMMAND "${FREDPP_EXE}" --version
    RESULT_VARIABLE canonical_status
    OUTPUT_VARIABLE canonical_output
    ERROR_VARIABLE canonical_error
)
execute_process(
    COMMAND "${alias_exe}" --version
    RESULT_VARIABLE alias_status
    OUTPUT_VARIABLE alias_output
    ERROR_VARIABLE alias_error
)
if(NOT "${canonical_status}" STREQUAL "0" OR
   NOT "${alias_status}" STREQUAL "0" OR
   NOT "${canonical_output}" STREQUAL "${alias_output}" OR
   NOT "${canonical_error}" STREQUAL "${alias_error}")
    message(FATAL_ERROR
        "Alias behaviour differs: fredpp=${canonical_status}, fp=${alias_status}")
endif()
message(STATUS "Windows fp.exe alias: identical SHA-256 and --version")
