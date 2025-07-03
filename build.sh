#!/bin/bash
# Zappy Project - Professional Build System
# Enterprise-grade build, test, and deployment script

set -e

# Configuration
PROJECT_NAME="Zappy"
VERSION="1.0.0"
BUILD_DIR="build"
DIST_DIR="dist"
DOCS_DIR="docs"
COVERAGE_DIR="coverage"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Component directories
SERVER_DIR="server"
GUI_DIR="gui"
AI_DIR="ai"

# Logging
LOG_FILE="build.log"
ERROR_LOG="error.log"

# Utility functions
print_header() {
    echo -e "${CYAN}=================================================${NC}"
    echo -e "${CYAN}$1${NC}"
    echo -e "${CYAN}=================================================${NC}"
}

print_step() {
    echo -e "${BLUE}[STEP]${NC} $1"
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

print_info() {
    echo -e "${PURPLE}[INFO]${NC} $1"
}

# Logging functions
log_info() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') [INFO] $1" >> "$LOG_FILE"
}

log_error() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') [ERROR] $1" >> "$ERROR_LOG"
}

# Error handling
handle_error() {
    local exit_code=$?
    print_error "Build failed with exit code: $exit_code"
    log_error "Build failed with exit code: $exit_code"
    exit $exit_code
}

trap handle_error ERR

# System checks
check_system_requirements() {
    print_step "Checking system requirements..."
    
    # Check for required tools
    local required_tools=("gcc" "g++" "make" "cmake" "python3" "pip3")
    local missing_tools=()
    
    for tool in "${required_tools[@]}"; do
        if ! command -v "$tool" &> /dev/null; then
            missing_tools+=("$tool")
        fi
    done
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        print_error "Missing required tools: ${missing_tools[*]}"
        print_info "Please install missing tools and try again"
        exit 1
    fi
    
    # Check for optional tools
    local optional_tools=("valgrind" "gcov" "lcov" "cppcheck" "clang-format")
    local missing_optional=()
    
    for tool in "${optional_tools[@]}"; do
        if ! command -v "$tool" &> /dev/null; then
            missing_optional+=("$tool")
        fi
    done
    
    if [ ${#missing_optional[@]} -ne 0 ]; then
        print_warning "Optional tools not found: ${missing_optional[*]}"
        print_info "Install with: sudo apt-get install ${missing_optional[*]}"
    fi
    
    print_success "System requirements check completed"
}

# Clean function
clean_all() {
    print_step "Cleaning all build artifacts..."
    
    # Clean root directories
    rm -rf "$BUILD_DIR" "$DIST_DIR" "$COVERAGE_DIR"
    rm -f "$LOG_FILE" "$ERROR_LOG"
    
    # Clean server
    if [ -d "$SERVER_DIR" ]; then
        cd "$SERVER_DIR"
        make clean 2>/dev/null || true
        rm -rf build/
        cd ..
    fi
    
    # Clean GUI
    if [ -d "$GUI_DIR" ]; then
        cd "$GUI_DIR"
        rm -rf build/
        cd ..
    fi
    
    # Clean AI
    if [ -d "$AI_DIR" ]; then
        cd "$AI_DIR"
        rm -rf .pytest_cache/ __pycache__/ htmlcov/ .coverage
        find . -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true
        find . -name "*.pyc" -delete 2>/dev/null || true
        cd ..
    fi
    
    print_success "All build artifacts cleaned"
}

# Setup function
setup_environment() {
    print_step "Setting up build environment..."
    
    # Create build directories
    mkdir -p "$BUILD_DIR" "$DIST_DIR" "$COVERAGE_DIR"
    
    # Initialize logging
    echo "Build started at $(date)" > "$LOG_FILE"
    echo "Error log initialized at $(date)" > "$ERROR_LOG"
    
    # Setup Python virtual environment for AI
    if [ -d "$AI_DIR" ]; then
        cd "$AI_DIR"
        if [ ! -d ".venv" ]; then
            print_step "Creating Python virtual environment..."
            python3 -m venv .venv
        fi
        
        print_step "Activating virtual environment and installing dependencies..."
        source .venv/bin/activate
        pip install --upgrade pip
        pip install -r requirements.txt
        cd ..
    fi
    
    print_success "Environment setup completed"
}

# Build server
build_server() {
    print_step "Building server component..."
    
    if [ ! -d "$SERVER_DIR" ]; then
        print_error "Server directory not found"
        return 1
    fi
    
    cd "$SERVER_DIR"
    log_info "Building server..."
    
    # Clean and build
    make clean || true
    make build
    
    # Copy binary to dist
    cp zappy_server "../$DIST_DIR/"
    
    cd ..
    print_success "Server built successfully"
}

# Build GUI
build_gui() {
    print_step "Building GUI component..."
    
    if [ ! -d "$GUI_DIR" ]; then
        print_error "GUI directory not found"
        return 1
    fi
    
    cd "$GUI_DIR"
    log_info "Building GUI..."
    
    # Create build directory and build
    mkdir -p build
    cd build
    cmake ..
    make -j$(nproc)
    
    # Copy binary to dist
    cp zappy_gui "../../$DIST_DIR/"
    
    cd ../..
    print_success "GUI built successfully"
}

# Setup AI
setup_ai() {
    print_step "Setting up AI component..."
    
    if [ ! -d "$AI_DIR" ]; then
        print_error "AI directory not found"
        return 1
    fi
    
    cd "$AI_DIR"
    log_info "Setting up AI..."
    
    # Activate virtual environment
    source .venv/bin/activate
    
    # Install dependencies
    pip install -r requirements.txt
    
    # Create AI launcher script
    cat > "../$DIST_DIR/run_ai.sh" << 'EOF'
#!/bin/bash
# AI Component Launcher
cd "$(dirname "$0")/../ai"
source .venv/bin/activate
python3 src/main_ai.py "$@"
EOF
    chmod +x "../$DIST_DIR/run_ai.sh"
    
    cd ..
    print_success "AI setup completed"
}

# Run tests
run_tests() {
    print_step "Running comprehensive test suite..."
    
    local test_results=()
    
    # Test server
    if [ -d "$SERVER_DIR" ]; then
        print_step "Testing server component..."
        cd "$SERVER_DIR"
        if make test-all; then
            test_results+=("SERVER: PASSED")
        else
            test_results+=("SERVER: FAILED")
        fi
        cd ..
    fi
    
    # Test GUI
    if [ -d "$GUI_DIR" ]; then
        print_step "Testing GUI component..."
        cd "$GUI_DIR"
        if [ -f "build/unit_tests" ]; then
            if ./build/unit_tests; then
                test_results+=("GUI: PASSED")
            else
                test_results+=("GUI: FAILED")
            fi
        else
            test_results+=("GUI: SKIPPED (no tests)")
        fi
        cd ..
    fi
    
    # Test AI
    if [ -d "$AI_DIR" ]; then
        print_step "Testing AI component..."
        cd "$AI_DIR"
        source .venv/bin/activate
        if python3 run_tests.py; then
            test_results+=("AI: PASSED")
        else
            test_results+=("AI: FAILED")
        fi
        cd ..
    fi
    
    # Report results
    print_header "TEST RESULTS"
    for result in "${test_results[@]}"; do
        if [[ "$result" == *"PASSED"* ]]; then
            print_success "$result"
        elif [[ "$result" == *"FAILED"* ]]; then
            print_error "$result"
        else
            print_warning "$result"
        fi
    done
    
    print_success "Test suite completed"
}

# Generate coverage reports
generate_coverage() {
    print_step "Generating coverage reports..."
    
    mkdir -p "$COVERAGE_DIR"
    
    # Server coverage
    if [ -d "$SERVER_DIR" ]; then
        print_step "Generating server coverage..."
        cd "$SERVER_DIR"
        make test-coverage
        if [ -f "test/*.gcov" ]; then
            cp test/*.gcov "../$COVERAGE_DIR/"
        fi
        cd ..
    fi
    
    # AI coverage
    if [ -d "$AI_DIR" ]; then
        print_step "Generating AI coverage..."
        cd "$AI_DIR"
        source .venv/bin/activate
        python3 run_tests.py -c
        if [ -d "htmlcov" ]; then
            cp -r htmlcov "../$COVERAGE_DIR/ai_coverage"
        fi
        cd ..
    fi
    
    print_success "Coverage reports generated in $COVERAGE_DIR"
}

# Memory check
run_memory_check() {
    print_step "Running memory leak detection..."
    
    if ! command -v valgrind &> /dev/null; then
        print_warning "Valgrind not found, skipping memory check"
        return 0
    fi
    
    # Server memory check
    if [ -d "$SERVER_DIR" ]; then
        print_step "Checking server memory leaks..."
        cd "$SERVER_DIR"
        make test-memory
        cd ..
    fi
    
    print_success "Memory check completed"
}

# Static analysis
run_static_analysis() {
    print_step "Running static code analysis..."
    
    if ! command -v cppcheck &> /dev/null; then
        print_warning "cppcheck not found, skipping static analysis"
        return 0
    fi
    
    # Server static analysis
    if [ -d "$SERVER_DIR" ]; then
        print_step "Analyzing server code..."
        cd "$SERVER_DIR"
        make analyze
        cd ..
    fi
    
    # GUI static analysis
    if [ -d "$GUI_DIR" ]; then
        print_step "Analyzing GUI code..."
        cppcheck --enable=all --error-exitcode=1 \
            --suppress=missingIncludeSystem \
            --suppress=unusedFunction \
            "$GUI_DIR/"*.cpp "$GUI_DIR/"*/*.cpp || true
    fi
    
    print_success "Static analysis completed"
}

# Code formatting
format_code() {
    print_step "Formatting code..."
    
    # Format AI code
    if [ -d "$AI_DIR" ]; then
        cd "$AI_DIR"
        source .venv/bin/activate
        if command -v black &> /dev/null; then
            black src/ tests/
        fi
        if command -v isort &> /dev/null; then
            isort src/ tests/
        fi
        cd ..
    fi
    
    # Format C/C++ code
    if command -v clang-format &> /dev/null; then
        find "$SERVER_DIR" "$GUI_DIR" -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | \
        xargs clang-format -i
    fi
    
    print_success "Code formatting completed"
}

# Create release package
create_release() {
    print_step "Creating release package..."
    
    local release_dir="$DIST_DIR/zappy-$VERSION"
    mkdir -p "$release_dir"
    
    # Copy binaries
    cp "$DIST_DIR/zappy_server" "$release_dir/"
    cp "$DIST_DIR/zappy_gui" "$release_dir/"
    cp "$DIST_DIR/run_ai.sh" "$release_dir/"
    
    # Copy resources
    cp -r resources "$release_dir/"
    
    # Copy documentation
    cp -r docs "$release_dir/"
    
    # Create README
    cat > "$release_dir/README.md" << EOF
# Zappy v$VERSION

## Quick Start

1. Start the server:
   \`\`\`bash
   ./zappy_server -p 8080 -x 10 -y 10 -n team1 team2 -c 5 -f 100
   \`\`\`

2. Start the GUI:
   \`\`\`bash
   ./zappy_gui -p 8080 -h localhost
   \`\`\`

3. Start AI client:
   \`\`\`bash
   ./run_ai.sh -p 8080 -n team1 -h localhost
   \`\`\`

## Documentation

See the \`docs/\` directory for complete documentation.

Built on: $(date)
EOF
    
    # Create archive
    cd "$DIST_DIR"
    tar -czf "zappy-$VERSION.tar.gz" "zappy-$VERSION"
    cd ..
    
    print_success "Release package created: $DIST_DIR/zappy-$VERSION.tar.gz"
}

# Main function
main() {
    local command="${1:-help}"
    
    case "$command" in
        "clean")
            clean_all
            ;;
        "setup")
            check_system_requirements
            setup_environment
            ;;
        "build")
            check_system_requirements
            setup_environment
            build_server
            build_gui
            setup_ai
            ;;
        "test")
            run_tests
            ;;
        "coverage")
            generate_coverage
            ;;
        "memory")
            run_memory_check
            ;;
        "analyze")
            run_static_analysis
            ;;
        "format")
            format_code
            ;;
        "release")
            check_system_requirements
            clean_all
            setup_environment
            build_server
            build_gui
            setup_ai
            run_tests
            generate_coverage
            create_release
            ;;
        "ci")
            # Continuous Integration pipeline
            check_system_requirements
            clean_all
            setup_environment
            build_server
            build_gui
            setup_ai
            run_tests
            generate_coverage
            run_memory_check
            run_static_analysis
            ;;
        "help")
            print_header "$PROJECT_NAME Build System v$VERSION"
            echo "Professional build system for the Zappy project"
            echo ""
            echo "Usage: $0 [command]"
            echo ""
            echo "Commands:"
            echo "  clean     - Clean all build artifacts"
            echo "  setup     - Setup build environment"
            echo "  build     - Build all components"
            echo "  test      - Run all tests"
            echo "  coverage  - Generate coverage reports"
            echo "  memory    - Run memory leak detection"
            echo "  analyze   - Run static code analysis"
            echo "  format    - Format all code"
            echo "  release   - Create release package"
            echo "  ci        - Run CI pipeline"
            echo "  help      - Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0 build        # Build all components"
            echo "  $0 test         # Run all tests"
            echo "  $0 ci           # Full CI pipeline"
            echo "  $0 release      # Create release package"
            ;;
        *)
            print_error "Unknown command: $command"
            main help
            exit 1
            ;;
    esac
}

# Initialize
print_header "$PROJECT_NAME Build System v$VERSION"
main "$@"
