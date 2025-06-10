#!/bin/bash

set -e

# Paths
SERVER_DIR="server"
GUI_DIR="gui"
AI_SCRIPT="ai/src/ai.py"

# Run cmake for server
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

# Run cmake for gui
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

# Check if AI python script can be run
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

echo "Build and checks completed successfully."