cmake_minimum_required(VERSION 3.10)
project(zappy_gui)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use pkg-config to find Raylib
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
)
# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/client  # Inclut les headers du dossier client
)

link_directories(${RAYLIB_LIBRARY_DIRS})

# Collect all source files
file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/client/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/map/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer/Camera/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer/Light/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer/Shaders/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer/Player/*.cpp"
)

add_executable(zappy_gui ${SOURCES})

# Link against Raylib and pthread
target_link_libraries(zappy_gui raylib pthread)
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

enable_testing()
add_executable(unit_tests
    tests/test_client.cpp
    tests/test_map.cpp
    client/Client.cpp
    map/Map.cpp
    renderer/Player/Player.cpp
    client/ProtocolHandler.cpp
)
target_link_libraries(unit_tests
    criterion pthread
)
add_test(NAME client_tests COMMAND unit_tests)
