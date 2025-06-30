cmake_minimum_required(VERSION 3.16)

# Test configuration
project(ZappyServerTests
    VERSION 1.0.0
    DESCRIPTION "Unit tests for Zappy Server"
    LANGUAGES C
)

# Test framework sources
set(TEST_FRAMEWORK_SOURCES
    test_framework.c
    test_stubs.c
)

# Test sources
set(TEST_SOURCES
    test_parse.c
    test_create_server_socket.c
    test_remove_client.c
    test_handle_client_data.c
    test_connection.c
    test_accept.c
    test_main.c
    run_all_tests.c
)

# Create test framework library
add_library(test_framework STATIC ${TEST_FRAMEWORK_SOURCES})
target_include_directories(test_framework PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Create the main test executable
add_executable(server_unit_tests ${TEST_SOURCES})
target_link_libraries(server_unit_tests 
    PRIVATE 
    test_framework
    zappy_server_lib
    Threads::Threads
)

target_include_directories(server_unit_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Set test executable properties
set_target_properties(server_unit_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
    OUTPUT_NAME "server_unit_tests"
)

# Add individual test cases
add_test(NAME ServerUnitTests COMMAND server_unit_tests)

# Set test properties
set_tests_properties(ServerUnitTests PROPERTIES
    TIMEOUT 300
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Custom test targets
add_custom_target(server-test
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS server_unit_tests
    COMMENT "Running server unit tests"
)

add_custom_target(server-test-verbose
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS server_unit_tests
    COMMENT "Running server unit tests (verbose)"
)

# Memory testing with Valgrind (if available)
find_program(VALGRIND_EXECUTABLE valgrind)
if(VALGRIND_EXECUTABLE)
    add_custom_target(server-test-memory
        COMMAND ${VALGRIND_EXECUTABLE} 
            --leak-check=full 
            --show-leak-kinds=all 
            --track-origins=yes 
            --error-exitcode=1
            $<TARGET_FILE:server_unit_tests>
        DEPENDS server_unit_tests
        COMMENT "Running server unit tests with memory checking"
    )
endif()

# Code coverage (if enabled)
if(CODE_COVERAGE)
    find_program(GCOV_EXECUTABLE gcov)
    find_program(LCOV_EXECUTABLE lcov)
    find_program(GENHTML_EXECUTABLE genhtml)
    
    if(GCOV_EXECUTABLE AND LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
        add_custom_target(server-coverage
            COMMAND ${LCOV_EXECUTABLE} --directory . --capture --output-file coverage.info
            COMMAND ${LCOV_EXECUTABLE} --remove coverage.info '/usr/*' --output-file coverage.info
            COMMAND ${LCOV_EXECUTABLE} --list coverage.info
            COMMAND ${GENHTML_EXECUTABLE} coverage.info --output-directory coverage_html
            DEPENDS server_unit_tests
            COMMENT "Generating code coverage report"
        )
    endif()
endif()

# Add to parent test target
if(TARGET all-tests)
    add_dependencies(all-tests server-test)
endif()
