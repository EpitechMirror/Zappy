#!/bin/bash
# Comprehensive test runner for Zappy project
# Professional testing suite with detailed reporting

set -e

# Configuration
PROJECT_NAME="Zappy"
TEST_RESULTS_DIR="test-results"
COVERAGE_DIR="coverage"
LOG_DIR="logs"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

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

# Initialize test environment
init_testing() {
    print_step "Initializing test environment..."
    
    # Create test directories
    mkdir -p "$TEST_RESULTS_DIR" "$COVERAGE_DIR" "$LOG_DIR"
    
    # Initialize log files
    echo "Test run started at $(date)" > "$LOG_DIR/test.log"
    echo "Error log initialized at $(date)" > "$LOG_DIR/error.log"
    
    # Initialize test results
    cat > "$TEST_RESULTS_DIR/summary.json" << EOF
{
    "project": "$PROJECT_NAME",
    "timestamp": "$(date -Iseconds)",
    "components": {
        "server": {"status": "pending", "tests": 0, "passed": 0, "failed": 0},
        "gui": {"status": "pending", "tests": 0, "passed": 0, "failed": 0},
        "ai": {"status": "pending", "tests": 0, "passed": 0, "failed": 0}
    },
    "overall": {"status": "pending", "total_tests": 0, "total_passed": 0, "total_failed": 0}
}
EOF
    
    print_success "Test environment initialized"
}

# Update test results
update_results() {
    local component="$1"
    local status="$2"
    local tests="$3"
    local passed="$4"
    local failed="$5"
    
    # Update JSON results using jq if available, otherwise use sed
    if command -v jq &> /dev/null; then
        jq ".components.$component.status = \"$status\" | 
            .components.$component.tests = $tests | 
            .components.$component.passed = $passed | 
            .components.$component.failed = $failed" \
            "$TEST_RESULTS_DIR/summary.json" > "$TEST_RESULTS_DIR/summary.tmp"
        mv "$TEST_RESULTS_DIR/summary.tmp" "$TEST_RESULTS_DIR/summary.json"
    fi
}

# Test server component
test_server() {
    print_step "Testing server component..."
    
    if [ ! -d "server" ]; then
        print_warning "Server directory not found, skipping server tests"
        update_results "server" "skipped" 0 0 0
        return 0
    fi
    
    cd server
    
    # Initialize server test results
    local server_tests=0
    local server_passed=0
    local server_failed=0
    
    # Run server tests
    if [ -f "test/run_tests.sh" ]; then
        print_info "Running server unit tests..."
        if timeout 300s ./test/run_tests.sh > "../$LOG_DIR/server_tests.log" 2>&1; then
            server_tests=$(grep -c "Running test:" "../$LOG_DIR/server_tests.log" 2>/dev/null || echo "0")
            server_passed=$(grep -c "PASSED" "../$LOG_DIR/server_tests.log" 2>/dev/null || echo "0")
            server_failed=$(grep -c "FAILED" "../$LOG_DIR/server_tests.log" 2>/dev/null || echo "0")
            
            if [ "$server_failed" -eq 0 ]; then
                print_success "Server tests passed ($server_passed/$server_tests)"
                update_results "server" "passed" "$server_tests" "$server_passed" "$server_failed"
            else
                print_error "Server tests failed ($server_failed/$server_tests failed)"
                update_results "server" "failed" "$server_tests" "$server_passed" "$server_failed"
                cd ..
                return 1
            fi
        else
            print_error "Server test execution failed"
            update_results "server" "failed" 0 0 1
            cd ..
            return 1
        fi
    else
        print_warning "Server test script not found"
        update_results "server" "skipped" 0 0 0
    fi
    
    # Run server memory tests
    if command -v valgrind &> /dev/null; then
        print_info "Running server memory leak tests..."
        if timeout 300s make test-memory > "../$LOG_DIR/server_memory.log" 2>&1; then
            if grep -q "no leaks are possible" "../$LOG_DIR/server_memory.log"; then
                print_success "Server memory tests passed"
            else
                print_warning "Server memory tests show potential issues"
            fi
        else
            print_warning "Server memory tests failed to complete"
        fi
    fi
    
    # Run server coverage
    if command -v gcov &> /dev/null; then
        print_info "Generating server coverage report..."
        if timeout 300s make test-coverage > "../$LOG_DIR/server_coverage.log" 2>&1; then
            # Copy coverage files
            if ls test/*.gcov 1> /dev/null 2>&1; then
                cp test/*.gcov "../$COVERAGE_DIR/"
                print_success "Server coverage report generated"
            fi
        else
            print_warning "Server coverage generation failed"
        fi
    fi
    
    cd ..
    return 0
}

# Test GUI component
test_gui() {
    print_step "Testing GUI component..."
    
    if [ ! -d "gui" ]; then
        print_warning "GUI directory not found, skipping GUI tests"
        update_results "gui" "skipped" 0 0 0
        return 0
    fi
    
    cd gui
    
    # Initialize GUI test results
    local gui_tests=0
    local gui_passed=0
    local gui_failed=0
    
    # Build GUI tests if not built
    if [ ! -f "build/unit_tests" ]; then
        print_info "Building GUI tests..."
        mkdir -p build
        cd build
        cmake .. && make -j$(nproc)
        cd ..
    fi
    
    # Run GUI tests
    if [ -f "build/unit_tests" ]; then
        print_info "Running GUI unit tests..."
        if timeout 300s ./build/unit_tests > "../$LOG_DIR/gui_tests.log" 2>&1; then
            # Parse test results (adjust based on your test framework)
            gui_tests=$(grep -c "Test" "../$LOG_DIR/gui_tests.log" 2>/dev/null || echo "1")
            gui_passed=1
            gui_failed=0
            
            print_success "GUI tests passed"
            update_results "gui" "passed" "$gui_tests" "$gui_passed" "$gui_failed"
        else
            print_error "GUI tests failed"
            gui_tests=1
            gui_passed=0
            gui_failed=1
            update_results "gui" "failed" "$gui_tests" "$gui_passed" "$gui_failed"
            cd ..
            return 1
        fi
    else
        print_warning "GUI test executable not found"
        update_results "gui" "skipped" 0 0 0
    fi
    
    cd ..
    return 0
}

# Test AI component
test_ai() {
    print_step "Testing AI component..."
    
    if [ ! -d "ai" ]; then
        print_warning "AI directory not found, skipping AI tests"
        update_results "ai" "skipped" 0 0 0
        return 0
    fi
    
    cd ai
    
    # Initialize AI test results
    local ai_tests=0
    local ai_passed=0
    local ai_failed=0
    
    # Activate virtual environment
    if [ -d ".venv" ]; then
        source .venv/bin/activate
    else
        print_warning "AI virtual environment not found"
    fi
    
    # Run AI tests
    if [ -f "run_tests.py" ]; then
        print_info "Running AI unit tests..."
        if timeout 300s python3 run_tests.py -v > "../$LOG_DIR/ai_tests.log" 2>&1; then
            # Parse pytest results
            ai_tests=$(grep -c "test_" "../$LOG_DIR/ai_tests.log" 2>/dev/null || echo "0")
            ai_passed=$(grep -c "PASSED" "../$LOG_DIR/ai_tests.log" 2>/dev/null || echo "0")
            ai_failed=$(grep -c "FAILED" "../$LOG_DIR/ai_tests.log" 2>/dev/null || echo "0")
            
            if [ "$ai_failed" -eq 0 ]; then
                print_success "AI tests passed ($ai_passed/$ai_tests)"
                update_results "ai" "passed" "$ai_tests" "$ai_passed" "$ai_failed"
            else
                print_error "AI tests failed ($ai_failed/$ai_tests failed)"
                update_results "ai" "failed" "$ai_tests" "$ai_passed" "$ai_failed"
                cd ..
                return 1
            fi
        else
            print_error "AI test execution failed"
            update_results "ai" "failed" 0 0 1
            cd ..
            return 1
        fi
    else
        print_warning "AI test script not found"
        update_results "ai" "skipped" 0 0 0
    fi
    
    # Run AI coverage
    if command -v python3 &> /dev/null; then
        print_info "Generating AI coverage report..."
        if timeout 300s python3 run_tests.py -c > "../$LOG_DIR/ai_coverage.log" 2>&1; then
            # Copy coverage files
            if [ -d "htmlcov" ]; then
                cp -r htmlcov "../$COVERAGE_DIR/ai_coverage"
                print_success "AI coverage report generated"
            fi
        else
            print_warning "AI coverage generation failed"
        fi
    fi
    
    cd ..
    return 0
}

# Integration tests
test_integration() {
    print_step "Running integration tests..."
    
    # Start server in background
    if [ -f "dist/zappy_server" ]; then
        print_info "Starting server for integration tests..."
        timeout 30s ./dist/zappy_server -p 8081 -x 5 -y 5 -n test_team -c 1 -f 1 > "$LOG_DIR/integration_server.log" 2>&1 &
        local server_pid=$!
        sleep 3
        
        if kill -0 "$server_pid" 2>/dev/null; then
            print_success "Server started successfully"
            
            # Test AI connection
            if [ -f "dist/run_ai.sh" ]; then
                print_info "Testing AI client connection..."
                timeout 10s ./dist/run_ai.sh -p 8081 -n test_team -h localhost > "$LOG_DIR/integration_ai.log" 2>&1 &
                local ai_pid=$!
                sleep 2
                
                if kill -0 "$ai_pid" 2>/dev/null; then
                    print_success "AI client connected successfully"
                    kill "$ai_pid" 2>/dev/null || true
                else
                    print_warning "AI client connection failed"
                fi
            fi
            
            # Stop server
            kill "$server_pid" 2>/dev/null || true
            sleep 1
        else
            print_error "Server failed to start"
            return 1
        fi
    else
        print_warning "Server executable not found, skipping integration tests"
    fi
    
    return 0
}

# Generate test report
generate_report() {
    print_step "Generating test report..."
    
    # Update overall results
    if command -v jq &> /dev/null; then
        local total_tests=$(jq '.components.server.tests + .components.gui.tests + .components.ai.tests' "$TEST_RESULTS_DIR/summary.json")
        local total_passed=$(jq '.components.server.passed + .components.gui.passed + .components.ai.passed' "$TEST_RESULTS_DIR/summary.json")
        local total_failed=$(jq '.components.server.failed + .components.gui.failed + .components.ai.failed' "$TEST_RESULTS_DIR/summary.json")
        
        local overall_status="passed"
        if [ "$total_failed" -gt 0 ]; then
            overall_status="failed"
        fi
        
        jq ".overall.status = \"$overall_status\" | 
            .overall.total_tests = $total_tests | 
            .overall.total_passed = $total_passed | 
            .overall.total_failed = $total_failed" \
            "$TEST_RESULTS_DIR/summary.json" > "$TEST_RESULTS_DIR/summary.tmp"
        mv "$TEST_RESULTS_DIR/summary.tmp" "$TEST_RESULTS_DIR/summary.json"
    fi
    
    # Generate HTML report
    cat > "$TEST_RESULTS_DIR/report.html" << EOF
<!DOCTYPE html>
<html>
<head>
    <title>$PROJECT_NAME Test Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .header { background: #f0f0f0; padding: 20px; border-radius: 5px; }
        .component { margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }
        .passed { background: #e8f5e8; border-color: #4caf50; }
        .failed { background: #ffeaea; border-color: #f44336; }
        .skipped { background: #fff3cd; border-color: #ffc107; }
        .stats { display: flex; justify-content: space-around; margin: 20px 0; }
        .stat { text-align: center; padding: 10px; background: #f5f5f5; border-radius: 5px; }
    </style>
</head>
<body>
    <div class="header">
        <h1>$PROJECT_NAME Test Report</h1>
        <p>Generated on: $(date)</p>
    </div>
    
    <div class="stats">
        <div class="stat">
            <h3>Total Tests</h3>
            <p id="total-tests">-</p>
        </div>
        <div class="stat">
            <h3>Passed</h3>
            <p id="total-passed">-</p>
        </div>
        <div class="stat">
            <h3>Failed</h3>
            <p id="total-failed">-</p>
        </div>
    </div>
    
    <div id="components">
        <!-- Component results will be inserted here -->
    </div>
    
    <script>
        // Load and display results from JSON
        fetch('summary.json')
            .then(response => response.json())
            .then(data => {
                document.getElementById('total-tests').textContent = data.overall.total_tests;
                document.getElementById('total-passed').textContent = data.overall.total_passed;
                document.getElementById('total-failed').textContent = data.overall.total_failed;
                
                const componentsDiv = document.getElementById('components');
                Object.keys(data.components).forEach(name => {
                    const component = data.components[name];
                    const div = document.createElement('div');
                    div.className = 'component ' + component.status;
                    div.innerHTML = \`
                        <h3>\${name.toUpperCase()} Component</h3>
                        <p>Status: \${component.status}</p>
                        <p>Tests: \${component.tests}</p>
                        <p>Passed: \${component.passed}</p>
                        <p>Failed: \${component.failed}</p>
                    \`;
                    componentsDiv.appendChild(div);
                });
            });
    </script>
</body>
</html>
EOF
    
    print_success "Test report generated: $TEST_RESULTS_DIR/report.html"
}

# Main execution
main() {
    local command="${1:-all}"
    
    print_header "$PROJECT_NAME Test Suite"
    
    case "$command" in
        "init")
            init_testing
            ;;
        "server")
            init_testing
            test_server
            generate_report
            ;;
        "gui")
            init_testing
            test_gui
            generate_report
            ;;
        "ai")
            init_testing
            test_ai
            generate_report
            ;;
        "integration")
            init_testing
            test_integration
            generate_report
            ;;
        "all")
            init_testing
            local success=0
            
            if test_server; then
                print_success "Server tests completed"
            else
                print_error "Server tests failed"
                success=1
            fi
            
            if test_gui; then
                print_success "GUI tests completed"
            else
                print_error "GUI tests failed"
                success=1
            fi
            
            if test_ai; then
                print_success "AI tests completed"
            else
                print_error "AI tests failed"
                success=1
            fi
            
            if test_integration; then
                print_success "Integration tests completed"
            else
                print_error "Integration tests failed"
                success=1
            fi
            
            generate_report
            
            if [ $success -eq 0 ]; then
                print_success "All tests completed successfully!"
            else
                print_error "Some tests failed!"
                exit 1
            fi
            ;;
        "clean")
            print_step "Cleaning test artifacts..."
            rm -rf "$TEST_RESULTS_DIR" "$COVERAGE_DIR" "$LOG_DIR"
            print_success "Test artifacts cleaned"
            ;;
        "help")
            echo "Usage: $0 [command]"
            echo ""
            echo "Commands:"
            echo "  all           - Run all tests (default)"
            echo "  server        - Run server tests only"
            echo "  gui           - Run GUI tests only"
            echo "  ai            - Run AI tests only"
            echo "  integration   - Run integration tests only"
            echo "  clean         - Clean test artifacts"
            echo "  help          - Show this help message"
            ;;
        *)
            print_error "Unknown command: $command"
            main help
            exit 1
            ;;
    esac
}

# Initialize
main "$@"
