function(fredpp_add_test target source)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE fredpp_core)
    if(MSVC)
        target_compile_options(${target} PRIVATE /FS)
    endif()
    add_test(NAME ${target} COMMAND ${target})
endfunction()

fredpp_add_test(test_version test_version.cpp)
target_compile_definitions(test_version PRIVATE FREDPP_EXPECTED_VERSION="${PROJECT_VERSION}")
fredpp_add_test(test_buffer test_buffer.cpp)
fredpp_add_test(test_flow test_flow.cpp)
fredpp_add_test(test_character_stream test_character_stream.cpp)
fredpp_add_test(test_lexer test_lexer.cpp)
fredpp_add_test(test_token_stream test_token_stream.cpp)
fredpp_add_test(test_address_parser test_address_parser.cpp)
fredpp_add_test(test_pattern_parser test_pattern_parser.cpp)
fredpp_add_test(test_pattern_matcher test_pattern_matcher.cpp)
fredpp_add_test(test_command_parser test_command_parser.cpp)
fredpp_add_test(test_runtime_context test_runtime_context.cpp)
fredpp_add_test(test_procedure_jump test_procedure_jump.cpp)
fredpp_add_test(test_procedure_numeric test_procedure_numeric.cpp)
fredpp_add_test(test_command_executor test_command_executor.cpp)
fredpp_add_test(test_append test_append.cpp)
fredpp_add_test(test_insert test_insert.cpp)
fredpp_add_test(test_change test_change.cpp)
fredpp_add_test(test_move test_move.cpp)
fredpp_add_test(test_transfer test_transfer.cpp)
fredpp_add_test(test_buffer_command test_buffer_command.cpp)
fredpp_add_test(test_options test_options.cpp)
fredpp_add_test(test_facts test_facts.cpp)
fredpp_add_test(test_messages test_messages.cpp)
fredpp_add_test(test_monitor test_monitor.cpp)
fredpp_add_test(test_procedure_runner test_procedure_runner.cpp)
fredpp_add_test(test_system_zg test_system_zg.cpp)
fredpp_add_test(test_help_manager test_help_manager.cpp)
fredpp_add_test(test_global test_global.cpp)
fredpp_add_test(test_zap test_zap.cpp)
fredpp_add_test(test_star_alias test_star_alias.cpp)
fredpp_add_test(test_substitute test_substitute.cpp)
fredpp_add_test(test_quit test_quit.cpp)
fredpp_add_test(test_file_io test_file_io.cpp)

add_test(NAME cli_smoke COMMAND fredpp --version)
set_tests_properties(cli_smoke PROPERTIES
    PASS_REGULAR_EXPRESSION "FREDPP v${PROJECT_VERSION}"
)

add_test(NAME cli_script
    COMMAND fredpp "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/cli_bootstrap.fredpp"
)
set_tests_properties(cli_script PROPERTIES
    PASS_REGULAR_EXPRESSION "CLI OK"
)

add_test(NAME cli_procedure_bootstrap
    COMMAND ${CMAKE_COMMAND}
        "-DFREDPP_EXE=$<TARGET_FILE:fredpp>"
        "-DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_cli_procedure.cmake"
)

add_test(NAME cli_bootstrap_environment
    COMMAND ${CMAKE_COMMAND}
        "-DFREDPP_EXE=$<TARGET_FILE:fredpp>"
        "-DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_cli_bootstrap_environment.cmake"
)

add_test(NAME cli_user_init
    COMMAND ${CMAKE_COMMAND}
        "-DFREDPP_EXE=$<TARGET_FILE:fredpp>"
        "-DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_cli_user_init.cmake"
)

add_test(NAME cli_bootstrap_facts
    COMMAND ${CMAKE_COMMAND}
        "-DFREDPP_EXE=$<TARGET_FILE:fredpp>"
        "-DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_cli_bootstrap_facts.cmake"
)

add_test(NAME cli_move_buffer
    COMMAND ${CMAKE_COMMAND}
        "-DFREDPP_EXE=$<TARGET_FILE:fredpp>"
        "-DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_cli_move_buffer.cmake"
)

add_test(NAME cli_flow_s_system
    COMMAND ${CMAKE_COMMAND}
        "-DFREDPP_EXE=$<TARGET_FILE:fredpp>"
        "-DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_cli_flow_s_system.cmake"
)

add_test(NAME cli_procedure_debug
    COMMAND ${CMAKE_COMMAND}
        "-DFREDPP_EXE=$<TARGET_FILE:fredpp>"
        "-DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_cli_procedure_debug.cmake"
)

add_test(NAME cli_flow_s_global
    COMMAND ${CMAKE_COMMAND}
        "-DFREDPP_EXE=$<TARGET_FILE:fredpp>"
        "-DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_cli_flow_s_global.cmake"
)

add_test(NAME cli_flow_s_global_interactive
    COMMAND ${CMAKE_COMMAND}
        "-DFREDPP_EXE=$<TARGET_FILE:fredpp>"
        "-DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_cli_flow_s_global_interactive.cmake"
)

add_test(NAME cli_jump_label
    COMMAND ${CMAKE_COMMAND}
        "-DFREDPP_EXE=$<TARGET_FILE:fredpp>"
        "-DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_cli_jump_label.cmake"
)

add_test(NAME cli_numeric_jump
    COMMAND ${CMAKE_COMMAND}
        "-DFREDPP_EXE=$<TARGET_FILE:fredpp>"
        "-DFIXTURE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test_cli_numeric_jump.cmake"
)

add_test(NAME test_release_manifest
    COMMAND ${CMAKE_COMMAND}
        -DFREDPP_SOURCE_DIR=${CMAKE_SOURCE_DIR}
        -DFREDPP_VERSION=${PROJECT_VERSION}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/test_release_manifest.cmake
)
