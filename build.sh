#!/bin/bash

set -e

# Paths
SERVER_DIR="server"
GUI_DIR="gui"
AI_SCRIPT="ai/src/ai.py"

# Parse arguments
BUILD_SERVER=false
BUILD_GUI=false
CHECK_AI=false

for arg in "$@"; do
    case $arg in
        -server)
            BUILD_SERVER=true
            ;;
        -gui)
            BUILD_GUI=true
            ;;
        -ai)
            CHECK_AI=true
            ;;
        *)
            echo "Unknown argument: $arg"
            echo "Usage: $0 [-server] [-gui] [-ai]"
            exit 1
            ;;
    esac
done

# Default to building all if no arguments are provided
if [ "$BUILD_SERVER" = false ] && [ "$BUILD_GUI" = false ] && [ "$CHECK_AI" = false ]; then
    BUILD_SERVER=true
    BUILD_GUI=true
    CHECK_AI=true
fi

# Run cmake for server
if [ "$BUILD_SERVER" = true ]; then
    echo "Building server..."
    if [ -d "$SERVER_DIR" ]; then
        cmake -S "$SERVER_DIR" -B "$SERVER_DIR/build" && cmake --build "$SERVER_DIR/build"
        if [ $? -ne 0 ]; then
            echo "Error: Server build failed."
            exit 1
        fi
    else
        echo "Error: Server directory not found."
        exit 1
    fi
fi

# Run cmake for gui
if [ "$BUILD_GUI" = true ]; then
    echo "Building gui..."
    if [ -d "$GUI_DIR" ]; then
        cmake -S "$GUI_DIR" -B "$GUI_DIR/build" && cmake --build "$GUI_DIR/build"
        if [ $? -ne 0 ]; then
            echo "Error: GUI build failed."
            exit 1
        fi
    else
        echo "Error: GUI directory not found."
        exit 1
    fi
fi

# Check if AI python script can be run
if [ "$CHECK_AI" = true ]; then
    echo "Checking AI script..."
    if [ -f "$AI_SCRIPT" ]; then
        python3 "$AI_SCRIPT" --help >/dev/null 2>&1
        if [ $? -ne 0 ]; then
            echo "Error: AI script cannot be run."
            exit 1
        fi
    else
        echo "Error: AI script not found."
        exit 1
    fi
fi

echo "Build and checks completed successfully."