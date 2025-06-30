cmake_minimum_required(VERSION 3.16)

# Test configuration
project(ZappyGUITests
    VERSION 1.0.0
    DESCRIPTION "Unit tests for Zappy GUI"
    LANGUAGES CXX
)

# Find testing framework
find_package(PkgConfig REQUIRED)
pkg_check_modules(CRITERION REQUIRED criterion)

# Define test executable configurations
# Format: test_name:implementation_sources
set(GUI_TESTS
    "test_client:../client/Client.cpp;../client/ProtocolHandler.cpp;../map/Map.cpp"
    "test_map:../map/Map.cpp"
    "test_player:../player/Player.cpp"
    "test_assets_manager:../renderer/AssetsManager/AssetsManager.cpp"
    "test_camera:../renderer/Camera/Camera.cpp"
    "test_light:../renderer/Light/Light.cpp"
    "test_renderer:../renderer/Renderer.cpp;../renderer/Camera/Camera.cpp;../renderer/Light/Light.cpp;../renderer/AssetsManager/AssetsManager.cpp;../renderer/Shaders/ShadersManager.cpp"
    "test_shader:../renderer/Shaders/ShadersManager.cpp"
)

# Create individual test executables
foreach(TEST_CONFIG ${GUI_TESTS})
    string(REPLACE ":" ";" TEST_PARTS ${TEST_CONFIG})
    list(GET TEST_PARTS 0 TEST_NAME)
    list(GET TEST_PARTS 1 IMPL_SOURCES)
    string(REPLACE ";" " " IMPL_SOURCES ${IMPL_SOURCES})
    string(REPLACE " " ";" IMPL_SOURCES ${IMPL_SOURCES})
    
    # Check if test source exists
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}.cpp")
        add_executable(${TEST_NAME} 
            ${TEST_NAME}.cpp 
            ${IMPL_SOURCES}
        )
        
        target_include_directories(${TEST_NAME} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/..
            ${CMAKE_CURRENT_SOURCE_DIR}/../client
            ${CMAKE_CURRENT_SOURCE_DIR}/../map
            ${CMAKE_CURRENT_SOURCE_DIR}/../player
            ${CMAKE_CURRENT_SOURCE_DIR}/../renderer
            ${CMAKE_CURRENT_SOURCE_DIR}/../renderer/Camera
            ${CMAKE_CURRENT_SOURCE_DIR}/../renderer/Light
            ${CMAKE_CURRENT_SOURCE_DIR}/../renderer/Shaders
            ${CMAKE_CURRENT_SOURCE_DIR}/../renderer/AssetsManager
            ${CMAKE_CURRENT_SOURCE_DIR}/../ressources
        )
        
        target_link_libraries(${TEST_NAME} 
            PRIVATE 
            ${RAYLIB_TARGET}
            ${CRITERION_LIBRARIES}
            Threads::Threads
        )
        
        target_compile_options(${TEST_NAME} PRIVATE ${CRITERION_CFLAGS_OTHER})
        target_include_directories(${TEST_NAME} PRIVATE ${CRITERION_INCLUDE_DIRS})
        target_link_directories(${TEST_NAME} PRIVATE ${CRITERION_LIBRARY_DIRS})
        
        # Set test properties
        set_target_properties(${TEST_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
        )
        
        # Add to CTest
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
        set_tests_properties(${TEST_NAME} PROPERTIES
            TIMEOUT 60
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        
        message(STATUS "Added GUI test: ${TEST_NAME}")
    else()
        message(WARNING "Test source ${TEST_NAME}.cpp not found, skipping")
    endif()
endforeach()

# Custom test targets
add_custom_target(gui-test
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMENT "Running GUI unit tests"
)

add_custom_target(gui-test-verbose
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    COMMENT "Running GUI unit tests (verbose)"
)

# Add to parent test target
if(TARGET all-tests)
    add_dependencies(all-tests gui-test)
endif()
