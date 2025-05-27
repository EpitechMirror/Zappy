cmake_minimum_required(VERSION 3.10)
project(zappy_gui)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use pkg-config to find Raylib
find_package(PkgConfig REQUIRED)
pkg_check_modules(RAYLIB raylib)
if(NOT RAYLIB_FOUND)
    message(FATAL_ERROR "Raylib not found. Install raylib and ensure pkg-config can find it.")
endif()

# Include directories
include_directories(
    ${RAYLIB_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/client  # Inclut les headers du dossier client
)

link_directories(${RAYLIB_LIBRARY_DIRS})

# Collect all source files
file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/client/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer/Camera/*.cpp"
)

add_executable(zappy_gui ${SOURCES})

# Link against Raylib and pthread
target_link_libraries(zappy_gui ${RAYLIB_LIBRARIES} pthread)

# Set output directory to project root (one level above build)
set_target_properties(zappy_gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_NAME "zappy_gui"
)

# Compiler warnings
if (MSVC)
    target_compile_options(zappy_gui PRIVATE /W4 /permissive-)
else()
    target_compile_options(zappy_gui PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Optional install
install(TARGETS zappy_gui RUNTIME DESTINATION bin)
