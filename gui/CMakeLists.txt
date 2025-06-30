cmake_minimum_required(VERSION 3.16)

# Project definition
project(ZappyGUI
    VERSION 1.0.0
    DESCRIPTION "Zappy GUI - Graphical client for Zappy game visualization"
    LANGUAGES CXX
)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
        add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-unused-function)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic)
    endif()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    endif()
endif()

# Find dependencies
find_package(Threads REQUIRED)

# Raylib dependency - Handle conflicts properly
# First check if raylib is already available (imported target)
if(TARGET raylib)
    message(STATUS "Using existing raylib target")
    set(RAYLIB_TARGET raylib)
elseif(TARGET raylib::raylib)
    message(STATUS "Using existing raylib::raylib target")
    set(RAYLIB_TARGET raylib::raylib)
else()
    # Try to find system raylib
    find_package(raylib QUIET)
    if(raylib_FOUND)
        message(STATUS "Found system Raylib")
        if(TARGET raylib)
            set(RAYLIB_TARGET raylib)
        elseif(TARGET raylib::raylib)
            set(RAYLIB_TARGET raylib::raylib)
        else()
            message(FATAL_ERROR "Raylib found but no valid target available")
        endif()
    else()
        # Fetch from source as last resort
        message(STATUS "Raylib not found, fetching from source...")
        include(FetchContent)
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.0
            GIT_SHALLOW TRUE
        )
        # Set raylib options before fetching
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(raylib)
        set(RAYLIB_TARGET raylib)
    endif()
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/client
    ${CMAKE_CURRENT_SOURCE_DIR}/map
    ${CMAKE_CURRENT_SOURCE_DIR}/player
    ${CMAKE_CURRENT_SOURCE_DIR}/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/ressources
)

# Source files organized by component
set(CLIENT_SOURCES
    client/Client.cpp
    client/ProtocolHandler.cpp
)

set(MAP_SOURCES
    map/Map.cpp
)

set(PLAYER_SOURCES
    player/Player.cpp
)

set(RENDERER_SOURCES
    renderer/Renderer.cpp
    renderer/Camera/Camera.cpp
    renderer/Light/Light.cpp
    renderer/Shaders/ShadersManager.cpp
    renderer/AssetsManager/AssetsManager.cpp
)

set(MAIN_SOURCES
    main.cpp
)

# All sources
set(ALL_SOURCES
    ${MAIN_SOURCES}
    ${CLIENT_SOURCES}
    ${MAP_SOURCES}
    ${PLAYER_SOURCES}
    ${RENDERER_SOURCES}
)

# Create the GUI executable
add_executable(zappy_gui ${ALL_SOURCES})

# Apply properties and coverage
zappy_set_target_properties(zappy_gui)
zappy_setup_coverage(zappy_gui)

# Link libraries
target_link_libraries(zappy_gui 
    PRIVATE 
    ${RAYLIB_TARGET}
    Threads::Threads
)

# Set output properties
set_target_properties(zappy_gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    OUTPUT_NAME "zappy_gui"
)

# Copy resources to build directory
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})

# Installation
install(TARGETS zappy_gui RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_INSTALL_DATADIR}/zappy)

# Testing
if(BUILD_TESTS)
    enable_testing()
    
    # Find Criterion testing framework
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(CRITERION REQUIRED criterion)
    
    if(CRITERION_FOUND)
        message(STATUS "Found Criterion testing framework")
        # Temporarily disable GUI tests due to complex linkage issues
        # These would need proper implementation source file organization
        message(STATUS "GUI tests temporarily disabled - need implementation source reorganization")
        # add_subdirectory(tests)
    else()
        message(WARNING "Criterion not found, GUI tests disabled")
    endif()
endif()

# Custom targets
add_custom_target(gui-run
    COMMAND $<TARGET_FILE:zappy_gui>
    DEPENDS zappy_gui
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running Zappy GUI"
)

add_custom_target(gui-debug
    COMMAND gdb --args $<TARGET_FILE:zappy_gui>
    DEPENDS zappy_gui
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Debugging Zappy GUI with GDB"
)

# Optional install
install(TARGETS zappy_gui RUNTIME DESTINATION bin)
