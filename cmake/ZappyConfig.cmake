# CMake configuration for Zappy project
# This file contains reusable CMake functions and settings

# Set minimum CMake version and policies
cmake_minimum_required(VERSION 3.16)

# Enable modern CMake policies
if(POLICY CMP0075)
    cmake_policy(SET CMP0075 NEW)  # Include file check honors CMAKE_REQUIRED_LIBRARIES
endif()
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)  # option() honors normal variables
endif()

# Utility function to set target properties consistently
function(zappy_set_target_properties target_name)
    set_target_properties(${target_name} PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
    )
    
    # Set compiler warnings
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive-)
        target_compile_definitions(${target_name} PRIVATE _CRT_SECURE_NO_WARNINGS)
    else()
        if(ZAPPY_ENABLE_COVERAGE)
            # Less strict warnings for coverage builds to avoid blocking test runs
            target_compile_options(${target_name} PRIVATE 
                -Wall -Wextra
                -Wno-unused-parameter -Wno-unused-function
                $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>
            )
        else()
            target_compile_options(${target_name} PRIVATE 
                -Wall -Wextra -Wpedantic -Werror
                $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>
            )
        endif()
        
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${target_name} PRIVATE -g -O0)
            if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
                target_compile_options(${target_name} PRIVATE 
                    -fsanitize=address,undefined
                    -fno-omit-frame-pointer
                )
                target_link_options(${target_name} PRIVATE 
                    -fsanitize=address,undefined
                )
            endif()
        endif()
    endif()
endfunction()

# Function to create a test executable
function(zappy_add_test test_name)
    cmake_parse_arguments(ZAPPY_TEST "" "" "SOURCES;LINKS" ${ARGN})
    
    add_executable(${test_name} ${ZAPPY_TEST_SOURCES})
    zappy_set_target_properties(${test_name})
    
    if(ZAPPY_TEST_LINKS)
        target_link_libraries(${test_name} PRIVATE ${ZAPPY_TEST_LINKS})
    endif()
    
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test/bin"
    )
    
    add_test(NAME ${test_name} COMMAND ${test_name})
    set_tests_properties(${test_name} PROPERTIES
        TIMEOUT 300
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endfunction()

# Function to find and configure dependencies
function(zappy_find_dependencies)
    find_package(Threads REQUIRED)
    
    # Find Raylib for GUI component
    if(BUILD_GUI)
        find_package(raylib QUIET)
        if(NOT raylib_FOUND)
            message(STATUS "Raylib not found, will be fetched during GUI build")
        else()
            message(STATUS "Found system raylib")
        endif()
    endif()
    
    # Find Python for AI component
    if(BUILD_AI)
        find_package(Python3 REQUIRED COMPONENTS Interpreter)
    endif()
    
    # Find development tools
    find_program(CLANG_FORMAT_EXECUTABLE clang-format)
    find_program(CLANG_TIDY_EXECUTABLE clang-tidy)
    find_program(CPPCHECK_EXECUTABLE cppcheck)
    find_program(VALGRIND_EXECUTABLE valgrind)
    
    # Coverage tools
    if(CODE_COVERAGE)
        find_program(GCOV_EXECUTABLE gcov)
        find_program(LCOV_EXECUTABLE lcov)
        find_program(GENHTML_EXECUTABLE genhtml)
    endif()
endfunction()

# Function to setup code analysis tools
function(zappy_setup_analysis target_name)
    if(CLANG_TIDY_EXECUTABLE)
        set_target_properties(${target_name} PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE};-checks=-*,readability-*,performance-*,modernize-*"
        )
    endif()
    
    if(CPPCHECK_EXECUTABLE)
        add_custom_target(${target_name}-cppcheck
            COMMAND ${CPPCHECK_EXECUTABLE}
                --enable=all
                --std=c11
                --verbose
                --quiet
                --force
                $<TARGET_PROPERTY:${target_name},SOURCE_DIR>
            COMMENT "Running cppcheck on ${target_name}"
        )
    endif()
endfunction()

# Function to setup installation
function(zappy_setup_install)
    include(GNUInstallDirs)
    
    # Install executables conditionally
    if(TARGET zappy_server)
        install(TARGETS zappy_server
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT Runtime
        )
    endif()
    
    if(TARGET zappy_gui)
        install(TARGETS zappy_gui
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT Runtime
        )
    endif()
    
    # Install resources
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/
        DESTINATION ${CMAKE_INSTALL_DATADIR}/zappy
    )
    
    # Install AI scripts
    if(BUILD_AI)
        install(DIRECTORY ${CMAKE_SOURCE_DIR}/ai/src/
            DESTINATION ${CMAKE_INSTALL_DATADIR}/zappy/ai
            FILES_MATCHING PATTERN "*.py"
        )
    endif()
    
    # Install documentation
    install(FILES ${CMAKE_SOURCE_DIR}/README.md
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
    )
endfunction()

# Function to setup coverage for C/C++ targets
function(zappy_setup_coverage target_name)
    if(CODE_COVERAGE)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${target_name} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
            target_link_options(${target_name} PRIVATE --coverage)
        endif()
    endif()
endfunction()

# Function to create coverage targets
function(zappy_create_coverage_targets)
    if(CODE_COVERAGE AND LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
        # Create coverage directory
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/coverage)
        
        # Reset coverage counters
        add_custom_target(coverage-reset
            COMMAND ${LCOV_EXECUTABLE} --directory . --zerocounters
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Resetting coverage counters"
        )
        
        # Generate coverage data for server
        add_custom_target(coverage-server
            COMMAND ${LCOV_EXECUTABLE} --directory . --capture --output-file coverage/server.info
            COMMAND ${LCOV_EXECUTABLE} --remove coverage/server.info '/usr/*' '*/test/*' '*/tests/*' --output-file coverage/server_clean.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS coverage-reset
            COMMENT "Generating server coverage data"
        )
        
        # Generate coverage data for GUI
        add_custom_target(coverage-gui
            COMMAND ${LCOV_EXECUTABLE} --directory . --capture --output-file coverage/gui.info
            COMMAND ${LCOV_EXECUTABLE} --remove coverage/gui.info '/usr/*' '*/test/*' '*/tests/*' --output-file coverage/gui_clean.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS coverage-reset
            COMMENT "Generating GUI coverage data"
        )
        
        # Combine all coverage data
        add_custom_target(coverage-combine
            COMMAND ${LCOV_EXECUTABLE} --add-tracefile coverage/server_clean.info --add-tracefile coverage/gui_clean.info --output-file coverage/combined.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS coverage-server coverage-gui
            COMMENT "Combining coverage data"
        )
        
        # Generate HTML report
        add_custom_target(coverage-html
            COMMAND ${GENHTML_EXECUTABLE} coverage/combined.info --output-directory coverage/html --title "Zappy Coverage Report" --show-details --legend
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS coverage-combine
            COMMENT "Generating HTML coverage report"
        )
        
        # Main coverage target
        add_custom_target(coverage
            DEPENDS coverage-html
            COMMENT "Generating complete coverage report"
        )
        
        # Coverage with tests
        add_custom_target(coverage-with-tests
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            DEPENDS coverage-reset
            COMMENT "Running tests with coverage"
        )
        
        add_dependencies(coverage coverage-with-tests)
        
        message(STATUS "Coverage targets created:")
        message(STATUS "  - coverage-reset: Reset coverage counters")
        message(STATUS "  - coverage: Generate complete coverage report")
        message(STATUS "  - coverage-with-tests: Run tests and generate coverage")
        message(STATUS "Coverage report will be available at: ${CMAKE_BINARY_DIR}/coverage/html/index.html")
    endif()
endfunction()
