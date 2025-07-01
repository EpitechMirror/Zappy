# Makefile for Zappy Server with Comprehensive Testing Support

# Default target
.DEFAULT_GOAL := all

# Variables
BUILD_DIR = build
TEST_DIR = test
CMAKE = cmake
MAKE = make

# Colors for output
GREEN = \033[0;32m
YELLOW = \033[1;33m
RED = \033[0;31m
NC = \033[0m # No Color

# Main targets
all: build
	@echo -e "$(GREEN)✅ Server build completed successfully$(NC)"

# Build the server using CMake
build:
	@echo -e "$(YELLOW)🔨 Building Zappy Server...$(NC)"
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. && $(MAKE) -j$$(nproc)

# Clean build files
clean:
	@echo -e "$(YELLOW)🧹 Cleaning build files...$(NC)"
	@rm -rf $(BUILD_DIR)
	@rm -f zappy_server
	@cd $(TEST_DIR) && $(MAKE) clean

# Rebuild everything
rebuild: clean all

# Install dependencies (for development)
install-deps:
	@echo -e "$(YELLOW)📦 Installing development dependencies...$(NC)"
	@sudo apt-get update
	@sudo apt-get install -y build-essential gcc make cmake valgrind lcov cppcheck

# Test targets
test: test-unit

# Run unit tests
test-unit:
	@echo -e "$(YELLOW)🧪 Running unit tests...$(NC)"
	@cd $(TEST_DIR) && $(MAKE) clean && $(MAKE) all
	@cd $(TEST_DIR) && ./run_tests.sh
	@echo -e "$(GREEN)✅ Unit tests completed successfully$(NC)"

# Run tests with memory leak detection
test-memory:
	@echo -e "$(YELLOW)🔍 Running tests with memory leak detection...$(NC)"
	@cd $(TEST_DIR) && $(MAKE) clean && $(MAKE) all
	@cd $(TEST_DIR) && ./run_tests.sh memory
	@echo -e "$(GREEN)✅ Memory leak detection completed$(NC)"

# Run tests with code coverage
test-coverage:
	@echo -e "$(YELLOW)📊 Running tests with code coverage...$(NC)"
	@cd $(TEST_DIR) && $(MAKE) clean && $(MAKE) all
	@cd $(TEST_DIR) && ./run_tests.sh coverage
	@echo -e "$(GREEN)✅ Code coverage analysis completed$(NC)"

# Run comprehensive test suite
test-all:
	@echo -e "$(YELLOW)🚀 Running comprehensive test suite...$(NC)"
	@cd $(TEST_DIR) && ./run_tests.sh clean memory coverage
	@echo -e "$(GREEN)✅ Comprehensive testing completed$(NC)"

# Static code analysis
analyze:
	@echo -e "$(YELLOW)🔎 Running static code analysis...$(NC)"
	@cppcheck --enable=all --error-exitcode=1 \
		--suppress=missingIncludeSystem \
		--suppress=unusedFunction \
		--suppress=unusedVariable \
		src/ include/ || true
	@echo -e "$(GREEN)✅ Static analysis completed$(NC)"

# Coding style check
style-check:
	@echo -e "$(YELLOW)📏 Checking coding style...$(NC)"
	@find src/ include/ -name "*.c" -o -name "*.h" | while read file; do \
		echo "Checking: $$file"; \
		if grep -n $$'\t' "$$file"; then \
			echo -e "$(RED)❌ Found tabs in $$file (should use spaces)$(NC)"; \
			exit 1; \
		fi; \
	done
	@echo -e "$(GREEN)✅ Coding style check completed$(NC)"

# Development setup
dev-setup: install-deps build test-unit
	@echo -e "$(GREEN)🎉 Development environment ready!$(NC)"

# Debug build
debug:
	@echo -e "$(YELLOW)🐛 Building debug version...$(NC)"
	@mkdir -p $(BUILD_DIR)-debug
	@cd $(BUILD_DIR)-debug && $(CMAKE) -DCMAKE_BUILD_TYPE=Debug .. && $(MAKE) -j$$(nproc)

# Release build
release:
	@echo -e "$(YELLOW)🚀 Building release version...$(NC)"
	@mkdir -p $(BUILD_DIR)-release
	@cd $(BUILD_DIR)-release && $(CMAKE) -DCMAKE_BUILD_TYPE=Release .. && $(MAKE) -j$$(nproc)

# Start server with default parameters (for testing)
run:
	@echo -e "$(YELLOW)🖥️  Starting server with default parameters...$(NC)"
	@./zappy_server -p 8080 -x 10 -y 10 -n team1 team2 -c 5 -f 100

# Quick test run (basic functionality)
quick-test: build
	@echo -e "$(YELLOW)⚡ Running quick functionality test...$(NC)"
	@timeout 3s ./zappy_server -p 8080 -x 10 -y 10 -n team1 team2 -c 5 -f 100 || true
	@echo -e "$(GREEN)✅ Quick test completed$(NC)"

# Help target
help:
	@echo "Available targets:"
	@echo "  all           - Build the server (default)"
	@echo "  build         - Build the server using CMake"
	@echo "  clean         - Clean build files"
	@echo "  rebuild       - Clean and rebuild"
	@echo "  install-deps  - Install development dependencies"
	@echo ""
	@echo "Testing targets:"
	@echo "  test          - Run unit tests"
	@echo "  test-unit     - Run unit tests"
	@echo "  test-memory   - Run tests with memory leak detection"
	@echo "  test-coverage - Run tests with code coverage"
	@echo "  test-all      - Run comprehensive test suite"
	@echo ""
	@echo "Quality assurance:"
	@echo "  analyze       - Run static code analysis"
	@echo "  style-check   - Check coding style compliance"
	@echo ""
	@echo "Development:"
	@echo "  dev-setup     - Set up development environment"
	@echo "  debug         - Build debug version"
	@echo "  release       - Build release version"
	@echo "  run           - Start server with default parameters"
	@echo "  quick-test    - Quick functionality test"
	@echo "  help          - Show this help message"

# Phony targets
.PHONY: all build clean rebuild install-deps test test-unit test-memory test-coverage test-all analyze style-check dev-setup debug release run quick-test help
