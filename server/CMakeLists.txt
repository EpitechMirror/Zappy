cmake_minimum_required(VERSION 3.10)
project(zappy_server)

# Enable C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

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
