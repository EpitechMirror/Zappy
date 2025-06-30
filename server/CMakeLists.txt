cmake_minimum_required(VERSION 3.16)

# Project definition
project(ZappyServer
    VERSION 1.0.0
    DESCRIPTION "Zappy Server - Game server component"
    LANGUAGES C
)

# C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    if(ZAPPY_ENABLE_COVERAGE)
        # Less strict warnings for coverage builds to avoid blocking test runs
        add_compile_options(-Wall -Wextra -std=c11 -Wno-unused-parameter -Wno-unused-function)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic -std=c11)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    endif()
endif()

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Source files for the library (excluding main.c)
set(SERVER_LIB_SOURCES
    src/parse.c
    src/functional/accept.c
    src/functional/connection.c
    src/functional/create_server_socket.c
    src/functional/handle_client_data.c
    src/functional/remove_client.c
)

# Create a static library for the server logic
add_library(zappy_server_lib STATIC ${SERVER_LIB_SOURCES})
target_include_directories(zappy_server_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(zappy_server_lib PUBLIC Threads::Threads)

# Main executable
add_executable(zappy_server src/main.c)
target_link_libraries(zappy_server PRIVATE zappy_server_lib)

# Apply properties and coverage
zappy_set_target_properties(zappy_server_lib)
zappy_setup_coverage(zappy_server_lib)
zappy_set_target_properties(zappy_server)
zappy_setup_coverage(zappy_server)

# Set output properties
set_target_properties(zappy_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    OUTPUT_NAME "zappy_server"
)

# Installation
install(TARGETS zappy_server zappy_server_lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/zappy_server)

# Testing
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# Custom targets
add_custom_target(server-run
    COMMAND $<TARGET_FILE:zappy_server>
    DEPENDS zappy_server
    COMMENT "Running Zappy Server"
)

add_custom_target(server-debug
    COMMAND gdb --args $<TARGET_FILE:zappy_server>
    DEPENDS zappy_server
    COMMENT "Debugging Zappy Server with GDB"
)


