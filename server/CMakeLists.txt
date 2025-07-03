cmake_minimum_required(VERSION 3.10)
project(zappy_server VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_C_FLAGS_RELEASE "-O3 -Wall -Wextra")

# Include directories
include_directories(include)

# Find all source files
file(GLOB_RECURSE SOURCES 
    "src/*.c"
)

# Create executable
add_executable(zappy_server ${SOURCES})

# Link libraries if needed
find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(zappy_server ${MATH_LIBRARY})
endif()

# Find pthread library
find_package(Threads REQUIRED)
target_link_libraries(zappy_server Threads::Threads)

# Set output directory
set_target_properties(zappy_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Install target
install(TARGETS zappy_server DESTINATION bin)

# Testing
enable_testing()

# Find test files - only build tests if explicitly requested
if(BUILD_TESTS)
    file(GLOB TEST_SOURCES "test/*.c")
    
    # Create test executable if test files exist
    if(TEST_SOURCES)
        # Filter out main.c from test sources to avoid multiple main functions
        list(FILTER SOURCES EXCLUDE REGEX "main.c$")
        
        add_executable(test_runner ${TEST_SOURCES} ${SOURCES})
        target_link_libraries(test_runner ${MATH_LIBRARY})
        target_link_libraries(test_runner Threads::Threads)
        
        # Add test
        add_test(NAME server_tests COMMAND test_runner)
        
        # Code coverage for tests
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            if(CMAKE_COMPILER_IS_GNUCC)
                target_compile_options(test_runner PRIVATE --coverage)
                target_link_options(test_runner PRIVATE --coverage)
            endif()
        endif()
    endif()
endif()

# Code coverage
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_COMPILER_IS_GNUCC)
        target_compile_options(zappy_server PRIVATE --coverage)
        target_link_options(zappy_server PRIVATE --coverage)
    endif()
endif()
