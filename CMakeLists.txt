cmake_minimum_required(VERSION 3.16)

# Add custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ZappyConfig)

# Project definition
project(Zappy
    VERSION 1.0.0
    DESCRIPTION "Zappy - A network game with AI agents and GUI visualization"
    LANGUAGES C CXX
)

# Set policies for modern CMake
cmake_policy(SET CMP0077 NEW)

# Build configuration
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Set global properties
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Global compiler flags
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0 -fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# Global compile definitions
add_compile_definitions(_GNU_SOURCE)

# Find required packages
zappy_find_dependencies()

# Options
option(BUILD_SERVER "Build the Zappy server" ON)
option(BUILD_GUI "Build the Zappy GUI client" ON)
option(BUILD_AI "Build the Zappy AI client" ON)
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(CODE_COVERAGE "Enable code coverage" OFF)

# Code coverage setup
if(CODE_COVERAGE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

# Enable testing globally
if(BUILD_TESTS)
    enable_testing()
    include(CTest)
endif()

# Add subdirectories
if(BUILD_SERVER)
    add_subdirectory(server)
endif()

if(BUILD_GUI)
    add_subdirectory(gui)
endif()

if(BUILD_AI)
    add_subdirectory(ai)
endif()

# Custom targets for convenience
add_custom_target(all-tests
    COMMENT "Running all unit tests"
)

add_custom_target(install-deps
    COMMENT "Installing all dependencies"
)

# Install configuration
zappy_setup_install()

# Coverage configuration
zappy_create_coverage_targets()

# Package configuration
set(CPACK_PACKAGE_NAME "Zappy")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VENDOR "Epitech")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
include(CPack)

# Print configuration
message(STATUS "=== Zappy Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "CXX Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build server: ${BUILD_SERVER}")
message(STATUS "Build GUI: ${BUILD_GUI}")
message(STATUS "Build AI: ${BUILD_AI}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Code coverage: ${CODE_COVERAGE}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "=================================")
