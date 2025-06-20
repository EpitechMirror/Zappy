#!/bin/bash

set -e

# Install dependencies
sudo apt-get update
sudo apt-get install -y git build-essential cmake libasound2-dev libpulse-dev libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev

# Clone raylib repository
git clone https://github.com/raysan5/raylib.git
cd raylib

# Build and install raylib
mkdir build && cd build
cmake -DSHARED=ON ..
make -j$(nproc)
sudo make install

# Update shared library cache
sudo ldconfig

echo "raylib installation completed."