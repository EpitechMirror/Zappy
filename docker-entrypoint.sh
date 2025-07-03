#!/bin/bash
# Docker entrypoint script for Zappy project

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_info() {
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

# Default configuration
DEFAULT_PORT=8080
DEFAULT_WIDTH=10
DEFAULT_HEIGHT=10
DEFAULT_TEAMS="team1 team2"
DEFAULT_CLIENTS=5
DEFAULT_FREQ=100
DEFAULT_HOST="localhost"

# Parse environment variables
PORT=${ZAPPY_PORT:-$DEFAULT_PORT}
WIDTH=${ZAPPY_WIDTH:-$DEFAULT_WIDTH}
HEIGHT=${ZAPPY_HEIGHT:-$DEFAULT_HEIGHT}
TEAMS=${ZAPPY_TEAMS:-$DEFAULT_TEAMS}
CLIENTS=${ZAPPY_CLIENTS:-$DEFAULT_CLIENTS}
FREQ=${ZAPPY_FREQ:-$DEFAULT_FREQ}
HOST=${ZAPPY_HOST:-$DEFAULT_HOST}

# Function to start server
start_server() {
    print_info "Starting Zappy server..."
    print_info "Configuration:"
    print_info "  Port: $PORT"
    print_info "  Map size: ${WIDTH}x${HEIGHT}"
    print_info "  Teams: $TEAMS"
    print_info "  Clients per team: $CLIENTS"
    print_info "  Frequency: $FREQ"
    
    exec ./dist/zappy_server \
        -p "$PORT" \
        -x "$WIDTH" \
        -y "$HEIGHT" \
        -n $TEAMS \
        -c "$CLIENTS" \
        -f "$FREQ"
}

# Function to start GUI
start_gui() {
    print_info "Starting Zappy GUI..."
    print_info "Connecting to server at $HOST:$PORT"
    
    exec ./dist/zappy_gui -p "$PORT" -h "$HOST"
}

# Function to start AI
start_ai() {
    local team_name="${1:-team1}"
    print_info "Starting Zappy AI client..."
    print_info "Team: $team_name"
    print_info "Connecting to server at $HOST:$PORT"
    
    exec ./dist/run_ai.sh -p "$PORT" -n "$team_name" -h "$HOST"
}

# Function to run tests
run_tests() {
    print_info "Running Zappy tests..."
    
    # Since we're in a runtime container, we can't run the full test suite
    # Instead, we'll run basic functionality tests
    
    # Test server startup
    print_info "Testing server startup..."
    timeout 5s ./dist/zappy_server -p 8081 -x 5 -y 5 -n test_team -c 1 -f 1 &
    server_pid=$!
    sleep 2
    
    if kill -0 "$server_pid" 2>/dev/null; then
        print_success "Server startup test passed"
        kill "$server_pid"
    else
        print_error "Server startup test failed"
        exit 1
    fi
    
    # Test AI script
    print_info "Testing AI script..."
    if [ -f "./dist/run_ai.sh" ] && [ -x "./dist/run_ai.sh" ]; then
        print_success "AI script test passed"
    else
        print_error "AI script test failed"
        exit 1
    fi
    
    print_success "All tests passed"
}

# Function to show help
show_help() {
    echo "Zappy Docker Container"
    echo ""
    echo "Usage: docker run [OPTIONS] zappy [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  server          Start the Zappy server (default)"
    echo "  gui             Start the Zappy GUI"
    echo "  ai [team]       Start the Zappy AI client"
    echo "  test            Run basic functionality tests"
    echo "  bash            Start bash shell"
    echo "  help            Show this help message"
    echo ""
    echo "Environment Variables:"
    echo "  ZAPPY_PORT      Server port (default: 8080)"
    echo "  ZAPPY_WIDTH     Map width (default: 10)"
    echo "  ZAPPY_HEIGHT    Map height (default: 10)"
    echo "  ZAPPY_TEAMS     Team names (default: 'team1 team2')"
    echo "  ZAPPY_CLIENTS   Clients per team (default: 5)"
    echo "  ZAPPY_FREQ      Frequency (default: 100)"
    echo "  ZAPPY_HOST      Server host for GUI/AI (default: localhost)"
    echo ""
    echo "Examples:"
    echo "  docker run -p 8080:8080 zappy"
    echo "  docker run -p 8080:8080 -e ZAPPY_TEAMS='alpha beta' zappy server"
    echo "  docker run --network host zappy gui"
    echo "  docker run --network host zappy ai team1"
}

# Function to check health
check_health() {
    # Simple health check - verify binaries exist
    if [ -f "./dist/zappy_server" ] && [ -x "./dist/zappy_server" ]; then
        exit 0
    else
        exit 1
    fi
}

# Main execution
case "${1:-server}" in
    "server")
        start_server
        ;;
    "gui")
        start_gui
        ;;
    "ai")
        start_ai "${2:-team1}"
        ;;
    "test")
        run_tests
        ;;
    "health")
        check_health
        ;;
    "bash")
        exec /bin/bash
        ;;
    "help")
        show_help
        ;;
    *)
        print_error "Unknown command: $1"
        show_help
        exit 1
        ;;
esac
