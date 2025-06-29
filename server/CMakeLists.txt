cmake_minimum_required(VERSION 3.10)
project(zappy_server)

# Enable C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

link_directories()

# Collect all source files
file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/functional/*.c"
)

add_executable(zappy_server ${SOURCES})

# Link against Raylib and pthread
target_link_libraries(zappy_server pthread)

# Set output directory to project root (one level above build)
set_target_properties(zappy_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_NAME "zappy_server"
)

# Compiler warnings
if (MSVC)
    target_compile_options(zappy_server PRIVATE /W4 /permissive-)
else()
    target_compile_options(zappy_server PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Optional install
install(TARGETS zappy_server RUNTIME DESTINATION bin)

# Enable testing
enable_testing()

# Add custom target to run unit tests
add_custom_target(test-unit
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_CURRENT_SOURCE_DIR}/test all
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/run_tests.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
    COMMENT "Running unit tests"
    DEPENDS zappy_server
)

# Add custom target to run tests with memory checking
add_custom_target(test-memory
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_CURRENT_SOURCE_DIR}/test clean
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_CURRENT_SOURCE_DIR}/test all
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/run_tests.sh memory
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
    COMMENT "Running unit tests with memory leak detection"
    DEPENDS zappy_server
)

# Add custom target to run tests with coverage
add_custom_target(test-coverage
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_CURRENT_SOURCE_DIR}/test clean
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_CURRENT_SOURCE_DIR}/test all
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/run_tests.sh coverage
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
    COMMENT "Running unit tests with code coverage analysis"
    DEPENDS zappy_server
)

# Add custom target to run all tests
add_custom_target(test-all
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_CURRENT_SOURCE_DIR}/test clean
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_CURRENT_SOURCE_DIR}/test all
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/run_tests.sh clean memory coverage
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
    COMMENT "Running comprehensive test suite with all options"
    DEPENDS zappy_server
)

# Add subdirectory for tests (optional, if you want CMake to manage tests too)
# add_subdirectory(test)

# Development targets
add_custom_target(dev-setup
    COMMAND echo "Setting up development environment..."
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_CURRENT_SOURCE_DIR}/test clean
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_CURRENT_SOURCE_DIR}/test all
    COMMENT "Setting up development environment with tests"
    DEPENDS zappy_server
)
