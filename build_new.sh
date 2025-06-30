#!/bin/bash

# Zappy Project Build Script
# Professional enterprise-grade build automation script
# Usage: ./build.sh [options]

set -euo pipefail  # Exit on error, undefined variables, and pipe failures

# Script configuration
readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly PROJECT_NAME="Zappy"
readonly BUILD_DIR="${SCRIPT_DIR}/build"
readonly INSTALL_DIR="${SCRIPT_DIR}/install"

# Build configuration
CMAKE_BUILD_TYPE="Release"
BUILD_SERVER="ON"
BUILD_GUI="ON"
BUILD_AI="ON"
BUILD_TESTS="ON"
BUILD_DOCS="OFF"
CODE_COVERAGE="OFF"
CLEAN_BUILD="false"
VERBOSE="false"
PARALLEL_JOBS=$(nproc)

# Color codes for output
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $*"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $*"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $*"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*" >&2
}

# Help function
show_help() {
    cat << EOF
${PROJECT_NAME} Build Script

USAGE:
    $0 [OPTIONS]

OPTIONS:
    -h, --help              Show this help message
    -c, --clean             Clean build directory before building
    -d, --debug             Build in Debug mode (default: Release)
    -t, --tests             Enable tests (default: enabled)
    --no-tests              Disable tests
    -v, --verbose           Enable verbose output
    --coverage              Enable code coverage
    --docs                  Build documentation
    --server-only           Build only the server component
    --gui-only              Build only the GUI component
    --ai-only               Build only the AI component
    -j, --jobs N            Number of parallel build jobs (default: ${PARALLEL_JOBS})
    --install               Install after building
    --package               Create distribution package

EXAMPLES:
    $0                      # Build all components in Release mode
    $0 --clean --debug      # Clean build in Debug mode
    $0 --server-only -t     # Build only server with tests
    $0 --coverage --tests   # Build with code coverage
    $0 --install --package  # Build, install, and create package

EOF
}

# Parse command line arguments
parse_arguments() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -c|--clean)
                CLEAN_BUILD="true"
                shift
                ;;
            -d|--debug)
                CMAKE_BUILD_TYPE="Debug"
                shift
                ;;
            -t|--tests)
                BUILD_TESTS="ON"
                shift
                ;;
            --no-tests)
                BUILD_TESTS="OFF"
                shift
                ;;
            -v|--verbose)
                VERBOSE="true"
                shift
                ;;
            --coverage)
                CODE_COVERAGE="ON"
                CMAKE_BUILD_TYPE="Debug"
                shift
                ;;
            --docs)
                BUILD_DOCS="ON"
                shift
                ;;
            --server-only)
                BUILD_SERVER="ON"
                BUILD_GUI="OFF"
                BUILD_AI="OFF"
                shift
                ;;
            --gui-only)
                BUILD_SERVER="OFF"
                BUILD_GUI="ON"
                BUILD_AI="OFF"
                shift
                ;;
            --ai-only)
                BUILD_SERVER="OFF"
                BUILD_GUI="OFF"
                BUILD_AI="ON"
                shift
                ;;
            -j|--jobs)
                if [[ -n "${2:-}" ]] && [[ "$2" =~ ^[0-9]+$ ]]; then
                    PARALLEL_JOBS="$2"
                    shift 2
                else
                    log_error "Option --jobs requires a numeric argument"
                    exit 1
                fi
                ;;
            --install)
                INSTALL_AFTER_BUILD="true"
                shift
                ;;
            --package)
                CREATE_PACKAGE="true"
                shift
                ;;
            *)
                log_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
}

# Check system dependencies
check_dependencies() {
    log_info "Checking system dependencies..."
    
    local missing_deps=()
    
    # Check for required tools
    command -v cmake >/dev/null 2>&1 || missing_deps+=("cmake")
    command -v make >/dev/null 2>&1 || missing_deps+=("make")
    command -v gcc >/dev/null 2>&1 || missing_deps+=("gcc")
    command -v g++ >/dev/null 2>&1 || missing_deps+=("g++")
    
    if [[ "$BUILD_AI" == "ON" ]]; then
        command -v python3 >/dev/null 2>&1 || missing_deps+=("python3")
        command -v pip3 >/dev/null 2>&1 || missing_deps+=("pip3")
    fi
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        log_error "Missing dependencies: ${missing_deps[*]}"
        log_error "Please install the missing dependencies and try again"
        exit 1
    fi
    
    log_success "All dependencies are available"
}

# Clean build directory
clean_build() {
    if [[ "$CLEAN_BUILD" == "true" ]]; then
        log_info "Cleaning build directory..."
        rm -rf "$BUILD_DIR"
        rm -rf "$INSTALL_DIR"
        log_success "Build directory cleaned"
    fi
}

# Configure build
configure_build() {
    log_info "Configuring build..."
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    local cmake_args=(
        "-DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE"
        "-DBUILD_SERVER=$BUILD_SERVER"
        "-DBUILD_GUI=$BUILD_GUI"
        "-DBUILD_AI=$BUILD_AI"
        "-DBUILD_TESTS=$BUILD_TESTS"
        "-DBUILD_DOCS=$BUILD_DOCS"
        "-DCODE_COVERAGE=$CODE_COVERAGE"
        "-DCMAKE_INSTALL_PREFIX=$INSTALL_DIR"
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
    )
    
    if [[ "$VERBOSE" == "true" ]]; then
        cmake_args+=("--debug-output")
    fi
    
    cmake "${cmake_args[@]}" "$SCRIPT_DIR"
    
    log_success "Build configured successfully"
}

# Build project
build_project() {
    log_info "Building project with $PARALLEL_JOBS parallel jobs..."
    
    cd "$BUILD_DIR"
    
    local make_args=("-j$PARALLEL_JOBS")
    
    if [[ "$VERBOSE" == "true" ]]; then
        make_args+=("VERBOSE=1")
    fi
    
    make "${make_args[@]}"
    
    log_success "Build completed successfully"
}

# Run tests
run_tests() {
    if [[ "$BUILD_TESTS" == "ON" ]]; then
        log_info "Running tests..."
        
        cd "$BUILD_DIR"
        
        if [[ "$VERBOSE" == "true" ]]; then
            ctest --output-on-failure --verbose
        else
            ctest --output-on-failure
        fi
        
        log_success "All tests passed"
    fi
}

# Install project
install_project() {
    if [[ "${INSTALL_AFTER_BUILD:-false}" == "true" ]]; then
        log_info "Installing project..."
        
        cd "$BUILD_DIR"
        make install
        
        log_success "Installation completed"
    fi
}

# Create package
create_package() {
    if [[ "${CREATE_PACKAGE:-false}" == "true" ]]; then
        log_info "Creating distribution package..."
        
        cd "$BUILD_DIR"
        make package
        
        log_success "Package created successfully"
    fi
}

# Print build summary
print_summary() {
    log_info "Build Summary:"
    echo "  Project: $PROJECT_NAME"
    echo "  Build Type: $CMAKE_BUILD_TYPE"
    echo "  Server: $BUILD_SERVER"
    echo "  GUI: $BUILD_GUI"
    echo "  AI: $BUILD_AI"
    echo "  Tests: $BUILD_TESTS"
    echo "  Documentation: $BUILD_DOCS"
    echo "  Code Coverage: $CODE_COVERAGE"
    echo "  Parallel Jobs: $PARALLEL_JOBS"
    echo "  Build Directory: $BUILD_DIR"
    echo "  Install Directory: $INSTALL_DIR"
    
    if [[ -f "$BUILD_DIR/bin/zappy_server" ]]; then
        echo "  Server Executable: $BUILD_DIR/bin/zappy_server"
    fi
    
    if [[ -f "$BUILD_DIR/bin/zappy_gui" ]]; then
        echo "  GUI Executable: $BUILD_DIR/bin/zappy_gui"
    fi
}

# Main execution
main() {
    log_info "Starting $PROJECT_NAME build process..."
    
    parse_arguments "$@"
    check_dependencies
    clean_build
    configure_build
    build_project
    run_tests
    install_project
    create_package
    print_summary
    
    log_success "$PROJECT_NAME build completed successfully!"
}

# Trap errors and cleanup
trap 'log_error "Build failed on line $LINENO"' ERR

# Execute main function with all arguments
main "$@"
