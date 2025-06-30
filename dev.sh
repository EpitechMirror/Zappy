#!/bin/bash

# Zappy Development Script
# Utility script for common development tasks
# Usage: ./dev.sh [command]

set -euo pipefail

# Script configuration
readonly SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly BUILD_DIR="${SCRIPT_DIR}/build"

# Color codes
readonly RED='\033[0;31m'
readonly GREEN='\033[0;32m'
readonly YELLOW='\033[1;33m'
readonly BLUE='\033[0;34m'
readonly NC='\033[0m'

log_info() { echo -e "${BLUE}[INFO]${NC} $*"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $*"; }
log_warning() { echo -e "${YELLOW}[WARNING]${NC} $*"; }
log_error() { echo -e "${RED}[ERROR]${NC} $*" >&2; }

show_help() {
    cat << EOF
Zappy Development Script

USAGE:
    $0 [COMMAND]

COMMANDS:
    setup               Initial project setup
    build               Build all components
    test                Run all tests
    server              Run the server
    gui                 Run the GUI client
    ai                  Run the AI client
    clean               Clean build artifacts
    format              Format code
    lint                Run linters
    coverage            Generate coverage report
    docs                Generate documentation
    package             Create release package

EXAMPLES:
    $0 setup            # Initial setup
    $0 build            # Build everything
    $0 test             # Run tests
    $0 server           # Start server

EOF
}

cmd_setup() {
    log_info "Setting up Zappy development environment..."
    
    # Install system dependencies
    log_info "Installing system dependencies..."
    if command -v apt-get >/dev/null 2>&1; then
        sudo apt-get update
        sudo apt-get install -y cmake build-essential python3 python3-pip python3-venv \
                               libraylib-dev valgrind lcov
    elif command -v yum >/dev/null 2>&1; then
        sudo yum groupinstall -y "Development Tools"
        sudo yum install -y cmake python3 python3-pip raylib-devel valgrind lcov
    else
        log_warning "Unknown package manager. Please install dependencies manually:"
        log_warning "- cmake, build-essential, python3, python3-pip, python3-venv"
        log_warning "- libraylib-dev, valgrind, lcov"
    fi
    
    # Setup AI environment
    if [[ -d "${SCRIPT_DIR}/ai" ]]; then
        log_info "Setting up AI environment..."
        cd "${SCRIPT_DIR}/ai"
        python3 -m venv .venv
        source .venv/bin/activate
        pip install --upgrade pip
        pip install -r requirements.txt
        cd "${SCRIPT_DIR}"
    fi
    
    log_success "Setup completed successfully!"
}

cmd_build() {
    log_info "Building Zappy project..."
    "${SCRIPT_DIR}/build.sh" "$@"
}

cmd_test() {
    log_info "Running all tests..."
    "${SCRIPT_DIR}/build.sh" --tests
}

cmd_server() {
    if [[ -f "${BUILD_DIR}/bin/zappy_server" ]]; then
        log_info "Starting Zappy Server..."
        "${BUILD_DIR}/bin/zappy_server" "$@"
    else
        log_error "Server not built. Run './dev.sh build' first."
        exit 1
    fi
}

cmd_gui() {
    if [[ -f "${BUILD_DIR}/bin/zappy_gui" ]]; then
        log_info "Starting Zappy GUI..."
        "${BUILD_DIR}/bin/zappy_gui" "$@"
    else
        log_error "GUI not built. Run './dev.sh build' first."
        exit 1
    fi
}

cmd_ai() {
    if [[ -d "${SCRIPT_DIR}/ai" ]]; then
        log_info "Starting Zappy AI..."
        cd "${SCRIPT_DIR}/ai"
        if [[ -d ".venv" ]]; then
            source .venv/bin/activate
        fi
        python3 src/main_ai.py "$@"
    else
        log_error "AI directory not found."
        exit 1
    fi
}

cmd_clean() {
    log_info "Cleaning build artifacts..."
    "${SCRIPT_DIR}/build.sh" --clean
    
    # Additional cleanup
    find "${SCRIPT_DIR}" -name "*.o" -delete 2>/dev/null || true
    find "${SCRIPT_DIR}" -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true
    find "${SCRIPT_DIR}" -name "*.pyc" -delete 2>/dev/null || true
    
    log_success "Cleanup completed!"
}

cmd_format() {
    log_info "Formatting code..."
    
    # Format C/C++ code (if clang-format is available)
    if command -v clang-format >/dev/null 2>&1; then
        find "${SCRIPT_DIR}" -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | \
            xargs clang-format -i
        log_success "C/C++ code formatted"
    fi
    
    # Format Python code
    if [[ -d "${SCRIPT_DIR}/ai" ]]; then
        cd "${SCRIPT_DIR}/ai"
        if [[ -d ".venv" ]]; then
            source .venv/bin/activate
        fi
        if command -v black >/dev/null 2>&1; then
            black src/ tests/
            log_success "Python code formatted with black"
        fi
        if command -v isort >/dev/null 2>&1; then
            isort src/ tests/
            log_success "Python imports sorted with isort"
        fi
    fi
}

cmd_lint() {
    log_info "Running linters..."
    
    # Lint Python code
    if [[ -d "${SCRIPT_DIR}/ai" ]]; then
        cd "${SCRIPT_DIR}/ai"
        if [[ -d ".venv" ]]; then
            source .venv/bin/activate
        fi
        if command -v flake8 >/dev/null 2>&1; then
            flake8 src/ tests/
            log_success "Python linting completed"
        fi
    fi
}

cmd_coverage() {
    log_info "Generating coverage report..."
    "${SCRIPT_DIR}/build.sh" --coverage
}

cmd_docs() {
    log_info "Generating documentation..."
    # Add documentation generation logic here
    log_warning "Documentation generation not implemented yet"
}

cmd_package() {
    log_info "Creating release package..."
    "${SCRIPT_DIR}/build.sh" --package
}

# Main command dispatcher
main() {
    if [[ $# -eq 0 ]]; then
        show_help
        exit 0
    fi
    
    local command="$1"
    shift
    
    case "$command" in
        setup)      cmd_setup "$@" ;;
        build)      cmd_build "$@" ;;
        test)       cmd_test "$@" ;;
        server)     cmd_server "$@" ;;
        gui)        cmd_gui "$@" ;;
        ai)         cmd_ai "$@" ;;
        clean)      cmd_clean "$@" ;;
        format)     cmd_format "$@" ;;
        lint)       cmd_lint "$@" ;;
        coverage)   cmd_coverage "$@" ;;
        docs)       cmd_docs "$@" ;;
        package)    cmd_package "$@" ;;
        help|-h|--help) show_help ;;
        *)
            log_error "Unknown command: $command"
            show_help
            exit 1
            ;;
    esac
}

main "$@"
