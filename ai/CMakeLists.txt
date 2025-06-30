cmake_minimum_required(VERSION 3.16)

# Project definition
project(ZappyAI
    VERSION 1.0.0
    DESCRIPTION "Zappy AI - Artificial Intelligence client for Zappy game"
    LANGUAGES NONE
)

# Find Python interpreter
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Set Python paths
set(AI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(AI_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(AI_REQUIREMENTS ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt)

# Check if virtual environment exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.venv")
    set(PYTHON_VENV_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.venv")
    if(WIN32)
        set(PYTHON_VENV_BIN "${PYTHON_VENV_PATH}/Scripts")
        set(PYTHON_VENV_PYTHON "${PYTHON_VENV_BIN}/python.exe")
        set(PYTHON_VENV_PIP "${PYTHON_VENV_BIN}/pip.exe")
    else()
        set(PYTHON_VENV_BIN "${PYTHON_VENV_PATH}/bin")
        set(PYTHON_VENV_PYTHON "${PYTHON_VENV_BIN}/python")
        set(PYTHON_VENV_PIP "${PYTHON_VENV_BIN}/pip")
    endif()
else()
    set(PYTHON_VENV_PYTHON ${Python3_EXECUTABLE})
    set(PYTHON_VENV_PIP ${Python3_EXECUTABLE} -m pip)
endif()

# Custom targets for AI development
add_custom_target(ai-setup
    COMMAND ${Python3_EXECUTABLE} -m venv .venv
    COMMAND ${PYTHON_VENV_PIP} install --upgrade pip
    COMMAND ${PYTHON_VENV_PIP} install -r ${AI_REQUIREMENTS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Setting up Python virtual environment and dependencies"
)

add_custom_target(ai-install-deps
    COMMAND ${PYTHON_VENV_PIP} install -r ${AI_REQUIREMENTS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Installing AI dependencies"
)

add_custom_target(ai-run
    COMMAND ${PYTHON_VENV_PYTHON} ${AI_SOURCE_DIR}/main_ai.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running Zappy AI"
)

add_custom_target(ai-test
    COMMAND ${PYTHON_VENV_PYTHON} -m pytest ${AI_TEST_DIR} -v
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running AI unit tests"
)

add_custom_target(ai-test-coverage
    COMMAND ${PYTHON_VENV_PYTHON} -m pytest ${AI_TEST_DIR} --cov=${AI_SOURCE_DIR} --cov-report=html:${CMAKE_BINARY_DIR}/coverage/ai_html --cov-report=term --cov-report=xml:${CMAKE_BINARY_DIR}/coverage/ai_coverage.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running AI tests with coverage"
)

add_custom_target(ai-lint
    COMMAND ${PYTHON_VENV_PYTHON} -m flake8 ${AI_SOURCE_DIR} ${AI_TEST_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running AI code linting"
)

add_custom_target(ai-format
    COMMAND ${PYTHON_VENV_PYTHON} -m black ${AI_SOURCE_DIR} ${AI_TEST_DIR}
    COMMAND ${PYTHON_VENV_PYTHON} -m isort ${AI_SOURCE_DIR} ${AI_TEST_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Formatting AI code"
)

add_custom_target(ai-clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory __pycache__
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${AI_SOURCE_DIR}/__pycache__
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${AI_TEST_DIR}/__pycache__
    COMMAND ${CMAKE_COMMAND} -E remove_directory .pytest_cache
    COMMAND ${CMAKE_COMMAND} -E remove_directory htmlcov
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Cleaning AI build artifacts"
)

# Add to global test target
if(BUILD_TESTS)
    enable_testing()
    
    # Add AI tests to CTest
    add_test(NAME AIUnitTests
        COMMAND ${PYTHON_VENV_PYTHON} -m pytest ${AI_TEST_DIR} -v --tb=short
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    add_test(NAME AIIntegrationTests
        COMMAND ${PYTHON_VENV_PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # Set test properties
    set_tests_properties(AIUnitTests PROPERTIES
        TIMEOUT 300
        ENVIRONMENT "PYTHONPATH=${AI_SOURCE_DIR}:$ENV{PYTHONPATH}"
    )
    
    set_tests_properties(AIIntegrationTests PROPERTIES
        TIMEOUT 600
        ENVIRONMENT "PYTHONPATH=${AI_SOURCE_DIR}:$ENV{PYTHONPATH}"
    )
    
    # Legacy custom targets for manual execution
    add_custom_target(ai-integration-test
        COMMAND ${PYTHON_VENV_PYTHON} ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running AI integration tests"
    )
    
    if(TARGET all-tests)
        add_dependencies(all-tests ai-test)
    endif()
endif()

# Installation
install(DIRECTORY ${AI_SOURCE_DIR}/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/zappy/ai
    FILES_MATCHING PATTERN "*.py"
)

install(FILES ${AI_REQUIREMENTS}
    DESTINATION ${CMAKE_INSTALL_DATADIR}/zappy/ai
)
