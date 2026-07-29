add_executable(test_version test_version.cpp)
target_link_libraries(test_version PRIVATE fredpp_core)
add_test(NAME version COMMAND test_version)

add_test(NAME cli_smoke COMMAND fredpp)
set_tests_properties(cli_smoke PROPERTIES
    PASS_REGULAR_EXPRESSION "FREDPP v0.0.1"
)
