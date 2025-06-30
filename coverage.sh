#!/bin/bash
# Coverage report generation script for Zappy project
# This script generates comprehensive coverage reports for all components

set -e

# Configuration
BUILD_DIR="build"
COVERAGE_DIR="$BUILD_DIR/coverage"
REPORT_DIR="$COVERAGE_DIR/reports"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Helper functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check dependencies
check_dependencies() {
    log_info "Checking coverage dependencies..."
    
    # Check for lcov and genhtml (for C/C++)
    if ! command -v lcov &> /dev/null; then
        log_error "lcov is required for C/C++ coverage. Install with: sudo apt-get install lcov"
        exit 1
    fi
    
    if ! command -v genhtml &> /dev/null; then
        log_error "genhtml is required for C/C++ coverage. Install with: sudo apt-get install lcov"
        exit 1
    fi
    
    # Check Python coverage tools
    if [ -f ".venv/bin/python3" ]; then
        if ! .venv/bin/python3 -c "import coverage" &> /dev/null; then
            log_warning "Python coverage module not found, installing..."
            .venv/bin/pip install coverage pytest-cov
        fi
    fi
    
    log_success "All dependencies are available"
}

# Build with coverage
build_with_coverage() {
    log_info "Building project with coverage enabled..."
    
    # Clean and build with coverage
    rm -rf "$BUILD_DIR"
    mkdir -p "$COVERAGE_DIR" "$REPORT_DIR"
    
    ./build.sh --debug --coverage --clean
    
    log_success "Build completed with coverage instrumentation"
}

# Run tests with coverage
run_tests_with_coverage() {
    log_info "Running all tests with coverage..."
    
    cd "$BUILD_DIR"
    
    # Reset coverage counters
    lcov --directory . --zerocounters
    
    # Run tests
    ctest --output-on-failure
    
    cd ..
    
    log_success "Tests completed"
}

# Generate C/C++ coverage
generate_cpp_coverage() {
    log_info "Generating C/C++ coverage reports..."
    
    cd "$BUILD_DIR"
    
    # Capture coverage data
    lcov --directory . --capture --output-file coverage/total.info
    
    # Remove external and test files
    lcov --remove coverage/total.info \
        '/usr/*' \
        '*/test/*' \
        '*/tests/*' \
        '*/build/*' \
        '*/.venv/*' \
        '*/CMakeFiles/*' \
        --output-file coverage/project.info
    
    # Generate HTML report
    genhtml coverage/project.info \
        --output-directory coverage/cpp_html \
        --title "Zappy C/C++ Coverage Report" \
        --show-details \
        --legend \
        --frames \
        --highlight
    
    # Generate summary
    lcov --summary coverage/project.info > coverage/cpp_summary.txt
    
    cd ..
    
    log_success "C/C++ coverage report generated"
}

# Generate Python/AI coverage
generate_python_coverage() {
    log_info "Generating Python/AI coverage reports..."
    
    cd ai
    
    # Run tests with coverage
    if [ -f "../.venv/bin/python3" ]; then
        ../.venv/bin/python3 -m pytest tests/ \
            --cov=src \
            --cov-report=html:../$COVERAGE_DIR/python_html \
            --cov-report=term \
            --cov-report=xml:../$COVERAGE_DIR/python_coverage.xml \
            --cov-report=json:../$COVERAGE_DIR/python_coverage.json
    else
        log_warning "Python virtual environment not found, skipping Python coverage"
        cd ..
        return
    fi
    
    cd ..
    
    log_success "Python/AI coverage report generated"
}

# Generate combined report
generate_combined_report() {
    log_info "Generating combined coverage report..."
    
    # Create combined HTML report
    cat > "$REPORT_DIR/index.html" << 'EOF'
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Zappy Project Coverage Report</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background-color: #f5f5f5; }
        .container { max-width: 1200px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        h1 { color: #333; text-align: center; border-bottom: 2px solid #007acc; padding-bottom: 10px; }
        h2 { color: #007acc; margin-top: 30px; }
        .component { margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }
        .component h3 { margin-top: 0; color: #555; }
        .links { margin: 10px 0; }
        .links a { display: inline-block; margin: 5px 10px 5px 0; padding: 8px 15px; background: #007acc; color: white; text-decoration: none; border-radius: 4px; }
        .links a:hover { background: #005a99; }
        .summary { background: #f8f9fa; padding: 15px; border-left: 4px solid #007acc; margin: 20px 0; }
        .coverage-badge { display: inline-block; padding: 2px 8px; border-radius: 3px; color: white; font-weight: bold; margin-left: 10px; }
        .high { background-color: #28a745; }
        .medium { background-color: #ffc107; color: #212529; }
        .low { background-color: #dc3545; }
        .footer { text-align: center; margin-top: 30px; color: #666; border-top: 1px solid #ddd; padding-top: 15px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🎯 Zappy Project Coverage Report</h1>
        <div class="summary">
            <h2>📊 Coverage Summary</h2>
            <p>This report provides comprehensive code coverage analysis for all components of the Zappy project.</p>
            <p><strong>Generated:</strong> <span id="timestamp"></span></p>
        </div>

        <h2>📋 Component Reports</h2>
        
        <div class="component">
            <h3>🖥️ Server & GUI (C/C++)</h3>
            <p>Coverage analysis for the Zappy server and GUI components written in C/C++.</p>
            <div class="links">
                <a href="../cpp_html/index.html">📊 View C/C++ Coverage Report</a>
                <a href="../cpp_summary.txt">📄 Summary Text</a>
            </div>
        </div>
        
        <div class="component">
            <h3>🤖 AI Components (Python)</h3>
            <p>Coverage analysis for the AI system with 148+ unit tests.</p>
            <div class="links">
                <a href="../python_html/index.html">📊 View Python Coverage Report</a>
                <a href="../python_coverage.xml">📄 XML Report</a>
                <a href="../python_coverage.json">📄 JSON Report</a>
            </div>
        </div>
        
        <h2>🚀 Quick Actions</h2>
        <div class="component">
            <h3>Re-generate Coverage</h3>
            <p>To update these reports with the latest code changes:</p>
            <div class="links">
                <a href="#" onclick="alert('Run: ./coverage.sh in your terminal')">🔄 Regenerate Reports</a>
            </div>
        </div>
        
        <div class="footer">
            <p>Generated by Zappy Coverage System | <strong>Professional Enterprise Build</strong></p>
        </div>
    </div>
    
    <script>
        document.getElementById('timestamp').textContent = new Date().toLocaleString();
    </script>
</body>
</html>
EOF
    
    log_success "Combined coverage report generated"
}

# Print results
print_results() {
    log_info "Coverage analysis complete!"
    echo
    log_success "📊 Coverage Reports Generated:"
    echo "  🌐 Combined Report: $REPORT_DIR/index.html"
    echo "  🖥️  C/C++ Report:   $COVERAGE_DIR/cpp_html/index.html"
    echo "  🤖 Python Report:  $COVERAGE_DIR/python_html/index.html"
    echo
    
    # Print summary if available
    if [ -f "$BUILD_DIR/coverage/cpp_summary.txt" ]; then
        log_info "C/C++ Coverage Summary:"
        echo "────────────────────────────────────"
        cat "$BUILD_DIR/coverage/cpp_summary.txt" | grep -E "(lines|functions|branches)" || echo "Summary not available"
        echo "────────────────────────────────────"
        echo
    fi
    
    log_info "🚀 To view reports:"
    echo "  xdg-open $REPORT_DIR/index.html  # Open combined report"
    echo "  firefox $REPORT_DIR/index.html   # Or use Firefox"
    echo
    log_success "Coverage analysis completed successfully!"
}

# Main execution
main() {
    log_info "🎯 Starting Zappy Coverage Analysis"
    echo "═══════════════════════════════════════════════════════════"
    
    check_dependencies
    build_with_coverage
    run_tests_with_coverage
    generate_cpp_coverage
    generate_python_coverage
    generate_combined_report
    print_results
    
    echo "═══════════════════════════════════════════════════════════"
    log_success "🎉 Zappy Coverage Analysis Complete!"
}

# Handle arguments
case "${1:-}" in
    --help|-h)
        echo "Zappy Coverage Analysis Script"
        echo "Usage: $0 [options]"
        echo ""
        echo "Options:"
        echo "  --help, -h     Show this help message"
        echo "  --clean        Clean previous coverage data"
        echo "  --cpp-only     Generate only C/C++ coverage"
        echo "  --python-only  Generate only Python coverage"
        echo ""
        echo "Output:"
        echo "  Combined report: $REPORT_DIR/index.html"
        echo "  C/C++ report:    $COVERAGE_DIR/cpp_html/index.html"  
        echo "  Python report:   $COVERAGE_DIR/python_html/index.html"
        exit 0
        ;;
    --clean)
        log_info "Cleaning previous coverage data..."
        rm -rf "$BUILD_DIR"
        log_success "Coverage data cleaned"
        exit 0
        ;;
    --cpp-only)
        check_dependencies
        build_with_coverage
        run_tests_with_coverage
        generate_cpp_coverage
        log_success "C/C++ coverage analysis complete: $COVERAGE_DIR/cpp_html/index.html"
        exit 0
        ;;
    --python-only)
        check_dependencies
        generate_python_coverage
        log_success "Python coverage analysis complete: $COVERAGE_DIR/python_html/index.html"
        exit 0
        ;;
esac

# Run main function
main
