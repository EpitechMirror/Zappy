# Zappy Server Unit Tests

This directory contains comprehensive unit tests for all Zappy server components.

## Overview

The test suite covers all server source files:
- `main.c` - Main function and server startup
- `parse.c` - Command line argument parsing
- `functional/accept.c` - Client acceptance and polling
- `functional/connection.c` - Connection handling
- `functional/create_server_socket.c` - Socket creation
- `functional/handle_client_data.c` - Client data processing
- `functional/remove_client.c` - Client removal and management

## Test Structure

```
test/
├── test_framework.h         # Simple testing framework
├── test_framework.c         # Framework implementation
├── test_parse.c            # Tests for parse.c
├── test_create_server_socket.c  # Tests for socket creation
├── test_remove_client.c    # Tests for client management
├── test_handle_client_data.c    # Tests for data handling
├── test_connection.c       # Tests for connection handling
├── test_accept.c          # Tests for accept and polling
├── test_main.c            # Tests for main functionality
├── run_all_tests.c        # Main test runner
├── CMakeLists.txt         # CMake build configuration
├── Makefile              # Make build configuration
├── run_tests.sh          # Test runner script
└── README.md             # This file
```

## Running Tests

### Quick Start
```bash
# Simple test run
./run_tests.sh

# Clean build and test
./run_tests.sh clean

# Test with memory leak detection
./run_tests.sh memory

# Test with code coverage
./run_tests.sh coverage

# All options combined
./run_tests.sh clean memory coverage
```

### Using Make
```bash
# Build tests
make all

# Run tests
make test

# Run with valgrind
make test-memory

# Clean build files
make clean

# Rebuild from scratch
make rebuild
```

### Using CMake
```bash
# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run tests
./run_tests

# Or use the custom target
make test
```

## Test Framework

The test suite uses a custom lightweight testing framework with the following macros:

- `TEST_ASSERT(condition)` - Assert a condition is true
- `TEST_ASSERT_EQUAL(expected, actual)` - Assert two values are equal
- `TEST_ASSERT_STRING_EQUAL(expected, actual)` - Assert two strings are equal
- `TEST_ASSERT_NULL(ptr)` - Assert pointer is NULL
- `TEST_ASSERT_NOT_NULL(ptr)` - Assert pointer is not NULL

## Test Coverage

### Parse Functions (`test_parse.c`)
- ✅ `is_number()` - Valid and invalid number validation
- ✅ `handle_int()` - Integer argument handling
- ✅ `handle_teams()` - Team name parsing
- ✅ `parse()` - Complete command line parsing

### Socket Functions (`test_create_server_socket.c`)
- ✅ `create_server_socket()` - Socket creation and configuration
- ✅ Socket properties validation (TCP, REUSEADDR)
- ✅ Port binding and listening state
- ✅ Multiple socket creation

### Client Management (`test_remove_client.c`)
- ✅ `remove_client()` - Client removal from linked list
- ✅ `remove_client_fd()` - File descriptor array management
- ✅ `add_client_fd()` - Adding clients to poll array
- ✅ Edge cases (empty list, non-existent clients)

### Data Handling (`test_handle_client_data.c`)
- ✅ `handle_auth()` - Client authentication
- ✅ Team name processing (including GRAPHIC)
- ✅ Client state transitions
- ✅ Buffer handling and edge cases

### Connection Handling (`test_connection.c`)
- ✅ Client structure initialization
- ✅ Poll file descriptor management
- ✅ Client list management
- ✅ Server configuration usage

### Accept and Polling (`test_accept.c`)
- ✅ `set_non_blocking()` - File descriptor flags
- ✅ Poll setup and management
- ✅ Client polling and event handling
- ✅ Maximum client limits

### Main Functions (`test_main.c`)
- ✅ `print_help()` - Help function execution
- ✅ Server configuration initialization
- ✅ Argument validation
- ✅ Configuration edge cases

## Test Results

The test framework provides detailed output:
- Individual test results (PASSED/FAILED)
- Test suite summaries
- Overall statistics (tests run, passed, failed, success rate)
- Memory leak detection (with valgrind)
- Code coverage analysis (with gcov)

## Error Codes

- `0` - All tests passed
- `1` - Some tests failed or memory leaks detected

## Dependencies

Required tools:
- `gcc` - C compiler
- `make` - Build system
- `cmake` (optional) - Alternative build system
- `valgrind` (optional) - Memory leak detection
- `gcov` (optional) - Code coverage analysis

## Integration

These tests can be integrated into CI/CD pipelines:

```bash
# In your CI script
cd server/test
./run_tests.sh clean memory coverage
exit_code=$?
if [ $exit_code -ne 0 ]; then
    echo "Tests failed!"
    exit 1
fi
```

## Adding New Tests

To add tests for new functions:

1. Create a new test file `test_<module>.c`
2. Include the test framework: `#include "test_framework.h"`
3. Write test functions returning `int` (1 for pass, 0 for fail)
4. Create a test suite array and runner function
5. Add the runner to `run_all_tests.c`
6. Update the Makefile to include the new test file

Example:
```c
int test_my_function(void)
{
    int result = my_function(5);
    TEST_ASSERT_EQUAL(10, result);
    return 1;
}

test_case_t my_tests[] = {
    {"test_my_function", test_my_function}
};

void run_my_tests(void)
{
    run_test_suite("My Functions", my_tests, 
                   sizeof(my_tests) / sizeof(my_tests[0]));
}
```

## Troubleshooting

### Build Issues
- Ensure all source files exist in the server directory
- Check that include paths are correct
- Verify pthread library is available

### Test Failures
- Check server source code for bugs
- Verify test assumptions match implementation
- Use valgrind to check for memory issues

### Permission Issues
- Ensure test scripts are executable: `chmod +x run_tests.sh`
- Check file permissions for source files

## Contributing

When adding new server functionality:
1. Write tests first (TDD approach)
2. Ensure all tests pass
3. Aim for high code coverage
4. Test edge cases and error conditions
5. Update this README if needed
