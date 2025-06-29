#!/bin/bash
##
## test.sh for Zappy in /home/adrien/Epitech/B-YEP-410/Zappy/ai
##
## Made by adrien.marette@epitech.eu
## Login   <adrien.marette@epitech.eu>
##
## Started on  Sat Jun 29 2025
## Last update Mon Jun 29 2:39:35 PM 2025 adrien.marette@epitech.eu
##

# Zappy AI Test Runner Script
# This script provides various testing options for the AI components

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Get the directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd "$SCRIPT_DIR"

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if required packages are installed
check_dependencies() {
    print_status "Checking dependencies..."
    
    if ! python3 -c "import numpy" 2>/dev/null; then
        print_error "numpy is not installed. Installing..."
        pip install numpy
    fi
    
    if ! python3 -c "import pytest" 2>/dev/null; then
        print_error "pytest is not installed. Installing..."
        pip install pytest pytest-cov
    fi
    
    print_success "All dependencies are available"
}

# Function to run all tests
run_all_tests() {
    print_status "Running all AI unit tests..."
    echo "=================================="
    
    if python3 run_tests.py -v; then
        print_success "All tests passed!"
        return 0
    else
        print_error "Some tests failed!"
        return 1
    fi
}

# Function to run tests with coverage
run_tests_with_coverage() {
    print_status "Running tests with coverage analysis..."
    echo "======================================="
    
    if python3 run_tests.py -c -v; then
        print_success "Tests completed with coverage report!"
        print_status "Coverage report available in htmlcov/index.html"
        return 0
    else
        print_error "Tests failed during coverage run!"
        return 1
    fi
}

# Function to run specific test file
run_specific_test() {
    local test_file="$1"
    print_status "Running specific test file: $test_file"
    echo "=================================="
    
    if [[ ! -f "tests/$test_file" ]]; then
        print_error "Test file 'tests/$test_file' not found!"
        return 1
    fi
    
    if python3 run_tests.py -f "$test_file" -v; then
        print_success "Test file $test_file passed!"
        return 0
    else
        print_error "Test file $test_file failed!"
        return 1
    fi
}

# Function to run fast tests only (exclude slow tests)
run_fast_tests() {
    print_status "Running fast tests only..."
    echo "=========================="
    
    if python3 -m pytest tests/ -m "not slow" -v; then
        print_success "Fast tests passed!"
        return 0
    else
        print_error "Fast tests failed!"
        return 1
    fi
}

# Function to run linting and style checks
run_lint() {
    print_status "Running code quality checks..."
    echo "=============================="
    
    # Check if pylint is available
    if command -v pylint &> /dev/null; then
        print_status "Running pylint on source code..."
        pylint src/ || print_warning "Pylint found some issues"
    else
        print_warning "pylint not installed, skipping lint check"
    fi
    
    # Check if black is available for formatting
    if command -v black &> /dev/null; then
        print_status "Checking code formatting with black..."
        black --check src/ tests/ || print_warning "Code formatting issues found"
    else
        print_warning "black not installed, skipping format check"
    fi
    
    print_success "Code quality checks completed"
}

# Function to clean up test artifacts
clean_test_artifacts() {
    print_status "Cleaning up test artifacts..."
    
    # Remove coverage files
    rm -rf htmlcov/
    rm -f .coverage
    
    # Remove pytest cache
    rm -rf .pytest_cache/
    
    # Remove Python cache
    find . -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true
    find . -name "*.pyc" -delete 2>/dev/null || true
    
    # Remove test model files
    rm -f *.pkl
    
    print_success "Test artifacts cleaned up"
}

# Function to set up test environment
setup_test_environment() {
    print_status "Setting up test environment..."
    
    # Create virtual environment if it doesn't exist
    if [[ ! -d "../.venv" ]]; then
        print_status "Creating virtual environment..."
        python3 -m venv ../.venv
    fi
    
    # Activate virtual environment
    source ../.venv/bin/activate
    
    # Upgrade pip
    pip install --upgrade pip
    
    # Install dependencies
    pip install numpy pytest pytest-cov
    
    print_success "Test environment set up successfully"
}

# Function to display help
show_help() {
    echo "Zappy AI Test Runner"
    echo "==================="
    echo ""
    echo "Usage: $0 [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  test           Run all unit tests"
    echo "  coverage       Run tests with coverage analysis"
    echo "  fast           Run fast tests only (exclude slow tests)"
    echo "  lint           Run code quality checks"
    echo "  clean          Clean up test artifacts"
    echo "  setup          Set up test environment"
    echo "  check          Check dependencies"
    echo "  list           List available test files"
    echo "  help           Show this help message"
    echo ""
    echo "Specific test files:"
    echo "  classes        Run tests for Classes.py"
    echo "  memory         Run tests for MemorySystem.py"
    echo "  neural         Run tests for NeuralNetwork.py"
    echo "  thinking       Run tests for ThinkingModule.py"
    echo "  main           Run tests for main_ai.py"
    echo "  cli            Run tests for zappy_ai.py"
    echo ""
    echo "Examples:"
    echo "  $0 test                    # Run all tests"
    echo "  $0 coverage                # Run with coverage"
    echo "  $0 classes                 # Run only Classes tests"
    echo "  $0 clean && $0 test        # Clean and test"
}

# Function to list available tests
list_tests() {
    print_status "Available test files:"
    echo "===================="
    
    for test_file in tests/test_*.py; do
        if [[ -f "$test_file" ]]; then
            filename=$(basename "$test_file")
            echo "  📁 $filename"
            
            # Extract test classes
            grep -n "^class Test" "$test_file" | while read -r line; do
                class_name=$(echo "$line" | sed 's/.*class \([^(]*\).*/\1/')
                line_num=$(echo "$line" | cut -d: -f1)
                echo "    🧪 $class_name (line $line_num)"
            done
            echo ""
        fi
    done
}

# Main execution logic
case "${1:-test}" in
    "test")
        check_dependencies
        run_all_tests
        ;;
    "coverage")
        check_dependencies
        run_tests_with_coverage
        ;;
    "fast")
        check_dependencies
        run_fast_tests
        ;;
    "lint")
        run_lint
        ;;
    "clean")
        clean_test_artifacts
        ;;
    "setup")
        setup_test_environment
        ;;
    "check")
        check_dependencies
        ;;
    "list")
        list_tests
        ;;
    "classes")
        check_dependencies
        run_specific_test "test_classes.py"
        ;;
    "memory")
        check_dependencies
        run_specific_test "test_memory_system.py"
        ;;
    "neural")
        check_dependencies
        run_specific_test "test_neural_network.py"
        ;;
    "thinking")
        check_dependencies
        run_specific_test "test_thinking_module.py"
        ;;
    "main")
        check_dependencies
        run_specific_test "test_main_ai.py"
        ;;
    "cli")
        check_dependencies
        run_specific_test "test_zappy_ai.py"
        ;;
    "help"|"-h"|"--help")
        show_help
        ;;
    *)
        print_error "Unknown command: $1"
        echo ""
        show_help
        exit 1
        ;;
esac
