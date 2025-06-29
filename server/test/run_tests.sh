#!/bin/bash

# Zappy Server Test Runner Script

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_colored() {
    color=$1
    message=$2
    echo -e "${color}${message}${NC}"
}

# Function to print section header
print_header() {
    echo
    print_colored $BLUE "=================================================="
    print_colored $BLUE "$1"
    print_colored $BLUE "=================================================="
    echo
}

# Change to test directory
cd "$(dirname "$0")"

print_header "ZAPPY SERVER UNIT TESTS"

# Check if we should clean first
if [[ "$1" == "clean" ]] || [[ "$1" == "rebuild" ]]; then
    print_colored $YELLOW "Cleaning previous build..."
    make clean
    echo
fi

# Build the tests
print_colored $YELLOW "Building test suite..."
if make all; then
    print_colored $GREEN "✓ Build successful"
else
    print_colored $RED "✗ Build failed"
    exit 1
fi

echo

# Run the tests
print_colored $YELLOW "Running unit tests..."
if ./run_tests; then
    print_colored $GREEN "✓ All tests completed"
    test_result=0
else
    print_colored $RED "✗ Some tests failed"
    test_result=1
fi

echo

# Check for memory leaks if valgrind is available and requested
if [[ "$1" == "memory" ]] || [[ "$2" == "memory" ]]; then
    if command -v valgrind &> /dev/null; then
        print_header "MEMORY LEAK DETECTION"
        print_colored $YELLOW "Running tests with valgrind..."
        if valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1 ./run_tests > valgrind_output.txt 2>&1; then
            print_colored $GREEN "✓ No memory leaks detected"
        else
            print_colored $RED "✗ Memory leaks detected"
            echo "Valgrind output:"
            cat valgrind_output.txt
            test_result=1
        fi
        rm -f valgrind_output.txt
    else
        print_colored $YELLOW "⚠ Valgrind not available, skipping memory leak detection"
    fi
fi

# Generate coverage report if gcov is available and requested
if [[ "$1" == "coverage" ]] || [[ "$2" == "coverage" ]] || [[ "$3" == "coverage" ]]; then
    if command -v gcov &> /dev/null; then
        print_header "CODE COVERAGE ANALYSIS"
        print_colored $YELLOW "Generating coverage report..."
        
        # Rebuild with coverage flags
        make clean
        make CFLAGS="-Wall -Wextra -Wpedantic -std=c11 -g --coverage" LDFLAGS="-lpthread --coverage"
        ./run_tests > /dev/null 2>&1
        
        # Generate coverage report
        gcov *.c ../src/*.c ../src/functional/*.c > /dev/null 2>&1
        
        # Find .gcov files and show summary
        if ls *.gcov > /dev/null 2>&1; then
            print_colored $GREEN "✓ Coverage report generated"
            echo "Coverage files created: $(ls *.gcov | wc -l)"
            
            # Show brief coverage summary
            echo
            print_colored $BLUE "Coverage Summary:"
            for file in *.gcov; do
                lines=$(grep -c '^[[:space:]]*[0-9]' "$file" 2>/dev/null || echo 0)
                covered=$(grep -c '^[[:space:]]*[1-9]' "$file" 2>/dev/null || echo 0)
                if [ $lines -gt 0 ]; then
                    percentage=$((covered * 100 / lines))
                    echo "  $(basename $file .gcov): ${covered}/${lines} lines (${percentage}%)"
                fi
            done
        else
            print_colored $YELLOW "⚠ No coverage files generated"
        fi
        
        # Clean up coverage files
        rm -f *.gcov *.gcda *.gcno
    else
        print_colored $YELLOW "⚠ gcov not available, skipping coverage analysis"
    fi
fi

# Final result
echo
if [ $test_result -eq 0 ]; then
    print_colored $GREEN "🎉 All tests passed successfully!"
else
    print_colored $RED "❌ Some tests failed!"
fi

print_header "TEST SUMMARY"
echo "For more options, run:"
echo "  $0 clean       - Clean and rebuild"
echo "  $0 memory      - Run with memory leak detection"
echo "  $0 coverage    - Generate code coverage report"
echo "  $0 clean memory coverage - All options combined"
echo

exit $test_result
