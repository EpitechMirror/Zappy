# Makefile for Zappy Server Unit Tests

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g -D_GNU_SOURCE
LDFLAGS = -lpthread

# Directories
SRC_DIR = ../src
FUNC_DIR = $(SRC_DIR)/functional
INCLUDE_DIR = ../include
TEST_DIR = .

# Include paths
INCLUDES = -I$(INCLUDE_DIR) -I$(TEST_DIR)

# Source files
SERVER_SOURCES = $(SRC_DIR)/parse.c \
                 $(FUNC_DIR)/accept.c \
                 $(FUNC_DIR)/connection.c \
                 $(FUNC_DIR)/create_server_socket.c \
                 $(FUNC_DIR)/handle_client_data.c \
                 $(FUNC_DIR)/remove_client.c

TEST_SOURCES = test_framework.c \
               test_parse.c \
               test_create_server_socket.c \
               test_remove_client.c \
               test_handle_client_data.c \
               test_connection.c \
               test_accept.c \
               test_main.c \
               test_stubs.c \
               run_all_tests.c

# Object files
SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)
ALL_OBJECTS = $(SERVER_OBJECTS) $(TEST_OBJECTS)

# Target executable
TARGET = run_tests

# Default target
all: $(TARGET)

# Build the test executable
$(TARGET): $(ALL_OBJECTS)
	$(CC) $(ALL_OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile server source files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(FUNC_DIR)/%.o: $(FUNC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run tests
test: $(TARGET)
	./$(TARGET)

# Run tests with valgrind for memory checking
test-memory: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

# Build with coverage flags and run tests
test-coverage: clean
	$(MAKE) CFLAGS="$(CFLAGS) --coverage" LDFLAGS="$(LDFLAGS) --coverage" all
	./$(TARGET)

# Clean up
clean:
	rm -f $(ALL_OBJECTS) $(TARGET)
	rm -f $(SRC_DIR)/*.o $(FUNC_DIR)/*.o
	rm -f *.gcov *.gcda *.gcno

# Clean and rebuild
rebuild: clean all

# Show help
help:
	@echo "Available targets:"
	@echo "  all           - Build the test executable"
	@echo "  test          - Build and run tests"
	@echo "  test-memory   - Run tests with valgrind memory checking"
	@echo "  test-coverage - Build with coverage and run tests"
	@echo "  clean         - Remove object files and executable"
	@echo "  rebuild       - Clean and rebuild"
	@echo "  help          - Show this help message"

# Phony targets
.PHONY: all test test-memory test-coverage clean rebuild help
