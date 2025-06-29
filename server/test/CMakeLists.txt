cmake_minimum_required(VERSION 3.10)
project(zappy_server_tests)

# Enable C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

# Collect all source files from the main project (excluding main.c)
file(GLOB SERVER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/parse.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/functional/*.c"
)

# Collect all test source files
file(GLOB TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/test_framework.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/run_all_tests.c"
)

# Create the test executable
add_executable(zappy_server_tests ${SERVER_SOURCES} ${TEST_SOURCES})

# Link against pthread (required for server functionality)
target_link_libraries(zappy_server_tests pthread)

# Set output directory
set_target_properties(zappy_server_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    OUTPUT_NAME "run_tests"
)

# Compiler warnings
if (MSVC)
    target_compile_options(zappy_server_tests PRIVATE /W4 /permissive-)
else()
    target_compile_options(zappy_server_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Add custom target to run tests
add_custom_target(test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run_tests
    DEPENDS zappy_server_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running unit tests"
)

# Optional install
install(TARGETS zappy_server_tests RUNTIME DESTINATION bin)
