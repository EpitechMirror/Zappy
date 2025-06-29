# Installation Guide

This comprehensive guide covers installation of Zappy on various platforms, including dependencies, build tools, and troubleshooting common issues.

## 🖥️ Platform Support

Zappy supports the following platforms:

- **Linux** (Ubuntu 18.04+, CentOS 7+, Arch Linux)
- **macOS** (10.14+)  
- **Windows** (Windows 10+ with WSL or MinGW)

## 📋 Prerequisites

### System Requirements

**Minimum Requirements:**
- RAM: 512 MB
- Storage: 100 MB free space
- CPU: Any x64 processor
- Network: TCP/IP connectivity

**Recommended Requirements:**
- RAM: 2 GB
- Storage: 500 MB free space  
- CPU: Multi-core x64 processor
- GPU: OpenGL 3.3 support (for GUI)
- Network: Low-latency connection

### Required Dependencies

The following packages are required for building Zappy:

- **C Compiler** (GCC 7+ or Clang 6+)
- **C++ Compiler** (G++ 7+ or Clang++ 6+)  
- **CMake** (3.10+)
- **Git**
- **Python** (3.7+ for AI client)

### Graphics Dependencies (GUI Only)

- **SFML** (2.5+) or **raylib** (3.0+)
- **OpenGL** (3.3+)
- **GLFW** (3.2+)

## 🐧 Linux Installation

### Ubuntu/Debian

<!-- tabs:start -->

#### **Ubuntu 20.04/22.04**
```bash
# Update package database
sudo apt update

# Install build essentials
sudo apt install -y build-essential cmake git pkg-config

# Install graphics libraries
sudo apt install -y libsfml-dev libglfw3-dev libgl1-mesa-dev

# Install Python and pip (for AI client)
sudo apt install -y python3 python3-pip python3-venv

# Verify installations
cmake --version
gcc --version
python3 --version
```

#### **Ubuntu 18.04**
```bash
# Update package database
sudo apt update

# Install build essentials
sudo apt install -y build-essential cmake git pkg-config

# Install graphics libraries
sudo apt install -y libsfml-dev libglfw3-dev libgl1-mesa-dev

# Install newer Python if needed
sudo apt install -y python3.8 python3.8-pip python3.8-venv

# Create symlinks if needed
sudo ln -sf /usr/bin/python3.8 /usr/bin/python3
```

#### **Debian 10/11**
```bash
# Update package database
sudo apt update

# Install build essentials
sudo apt install -y build-essential cmake git pkg-config

# Install graphics libraries
sudo apt install -y libsfml-dev libglfw3-dev libgl1-mesa-dev

# Install Python
sudo apt install -y python3 python3-pip python3-venv
```

<!-- tabs:end -->

### CentOS/RHEL/Fedora

<!-- tabs:start -->

#### **CentOS 8/RHEL 8**
```bash
# Install development tools
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git pkg-config

# Install EPEL repository for additional packages
sudo dnf install epel-release

# Install graphics libraries
sudo dnf install SFML-devel glfw-devel mesa-libGL-devel

# Install Python
sudo dnf install python38 python38-pip
```

#### **Fedora 34+**
```bash
# Install development tools
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake git pkg-config

# Install graphics libraries
sudo dnf install SFML-devel glfw-devel mesa-libGL-devel

# Install Python
sudo dnf install python3 python3-pip
```

#### **CentOS 7**
```bash
# Install development tools
sudo yum groupinstall "Development Tools"
sudo yum install cmake3 git

# Install EPEL
sudo yum install epel-release

# Install Python 3.7+ from IUS repository
sudo yum install https://repo.ius.io/ius-release-el7.rpm
sudo yum install python37u python37u-pip

# Build SFML from source (CentOS 7 packages are too old)
# See building SFML section below
```

<!-- tabs:end -->

### Arch Linux

```bash
# Update system
sudo pacman -Syu

# Install base development packages
sudo pacman -S base-devel cmake git pkg-config

# Install graphics libraries
sudo pacman -S sfml glfw-x11 mesa

# Install Python
sudo pacman -S python python-pip
```

## 🍎 macOS Installation

### Using Homebrew (Recommended)

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Xcode command line tools
xcode-select --install

# Install dependencies
brew install cmake git pkg-config sfml glfw python3

# Verify installations
cmake --version
clang --version
python3 --version
```

### Using MacPorts

```bash
# Install MacPorts dependencies
sudo port install cmake git pkgconfig sfml glfw python38

# Set Python 3.8 as default
sudo port select --set python3 python38
```

### Manual Installation

If you prefer manual installation or package managers are not available:

1. **Install Xcode Command Line Tools:**
   ```bash
   xcode-select --install
   ```

2. **Install CMake:**
   - Download from https://cmake.org/download/
   - Install the .dmg package

3. **Build SFML from source:**
   ```bash
   git clone https://github.com/SFML/SFML.git
   cd SFML
   mkdir build && cd build
   cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
   make -j$(sysctl -n hw.ncpu)
   sudo make install
   ```

## 🪟 Windows Installation

### Using Windows Subsystem for Linux (WSL) - Recommended

1. **Install WSL2:**
   ```powershell
   # Run as Administrator
   wsl --install
   # Reboot system
   ```

2. **Install Ubuntu 20.04 from Microsoft Store**

3. **Follow Ubuntu installation steps above in WSL**

### Using MinGW-w64/MSYS2

1. **Install MSYS2:**
   - Download from https://www.msys2.org/
   - Run the installer

2. **Open MSYS2 terminal and update:**
   ```bash
   pacman -Syu
   ```

3. **Install development packages:**
   ```bash
   # Install base development tools
   pacman -S mingw-w64-x86_64-toolchain
   pacman -S mingw-w64-x86_64-cmake
   pacman -S mingw-w64-x86_64-git
   
   # Install graphics libraries
   pacman -S mingw-w64-x86_64-sfml
   pacman -S mingw-w64-x86_64-glfw
   
   # Install Python
   pacman -S mingw-w64-x86_64-python
   pacman -S mingw-w64-x86_64-python-pip
   ```

4. **Add to PATH:**
   Add `C:\msys64\mingw64\bin` to your Windows PATH environment variable.

### Using Visual Studio

1. **Install Visual Studio 2019+ with C++ support**

2. **Install vcpkg:**
   ```cmd
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg integrate install
   ```

3. **Install dependencies:**
   ```cmd
   .\vcpkg install sfml:x64-windows
   .\vcpkg install glfw3:x64-windows
   ```

## 🔧 Building SFML from Source

If your distribution doesn't have recent SFML packages:

```bash
# Install SFML dependencies
# Ubuntu/Debian:
sudo apt install libfreetype6-dev libx11-dev libxrandr-dev libudev-dev libgl1-mesa-dev libflac-dev libogg-dev libvorbis-dev libvorbisenc2 libvorbisfile3 libopenal-dev libpthread-stubs0-dev

# CentOS/RHEL:
sudo yum install freetype-devel libX11-devel libXrandr-devel libudev-devel mesa-libGL-devel flac-devel libogg-devel libvorbis-devel openal-soft-devel

# Clone and build SFML
git clone https://github.com/SFML/SFML.git
cd SFML
git checkout 2.5.1  # Use stable version

mkdir build && cd build
cmake .. \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=TRUE

make -j$(nproc)
sudo make install
sudo ldconfig  # Update library cache
```

## 🐍 Python Environment Setup

### Virtual Environment (Recommended)

```bash
# Create virtual environment
python3 -m venv zappy_env

# Activate virtual environment
# Linux/macOS:
source zappy_env/bin/activate
# Windows:
zappy_env\Scripts\activate

# Install Python dependencies
pip install --upgrade pip
pip install numpy torch torchvision matplotlib pytest coverage
```

### System-wide Installation

```bash
# Install Python packages system-wide
pip3 install numpy torch torchvision matplotlib pytest coverage

# Or use distribution packages
# Ubuntu/Debian:
sudo apt install python3-numpy python3-torch python3-matplotlib python3-pytest

# CentOS/RHEL:
sudo dnf install python3-numpy python3-matplotlib python3-pytest
```

## ✅ Installation Verification

### Test Build Environment

Create a simple test to verify your installation:

```bash
# Create test directory
mkdir zappy_test && cd zappy_test

# Create simple CMakeLists.txt
cat > CMakeLists.txt << EOF
cmake_minimum_required(VERSION 3.10)
project(ZappyTest)

set(CMAKE_CXX_STANDARD 17)

find_package(sfml REQUIRED COMPONENTS graphics window system)

add_executable(test_sfml test.cpp)
target_link_libraries(test_sfml sfml-graphics sfml-window sfml-system)
EOF

# Create simple test program
cat > test.cpp << EOF
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    std::cout << "SFML Version: " << SFML_VERSION_MAJOR << "." 
              << SFML_VERSION_MINOR << "." << SFML_VERSION_PATCH << std::endl;
    
    sf::RenderWindow window(sf::VideoMode(200, 200), "Test");
    std::cout << "SFML test successful!" << std::endl;
    return 0;
}
EOF

# Build test
mkdir build && cd build
cmake ..
make

# Run test
./test_sfml
```

Expected output:
```
SFML Version: 2.5.1
SFML test successful!
```

### Verify Python Setup

```python
# Test Python environment
python3 -c "
import numpy as np
import torch
print(f'NumPy version: {np.__version__}')
print(f'PyTorch version: {torch.__version__}')
print('Python environment OK!')
"
```

## 🐛 Troubleshooting

### Common Issues

#### SFML Not Found

**Error:** `Could not find SFML`

**Solutions:**
```bash
# Option 1: Install development packages
sudo apt install libsfml-dev  # Ubuntu/Debian
sudo dnf install SFML-devel   # Fedora/CentOS

# Option 2: Set CMAKE_PREFIX_PATH
cmake .. -DCMAKE_PREFIX_PATH=/usr/local

# Option 3: Build SFML from source (see above)
```

#### OpenGL Issues

**Error:** `OpenGL 3.3 not supported`

**Solutions:**
```bash
# Check OpenGL version
glxinfo | grep "OpenGL version"

# Install mesa drivers
sudo apt install mesa-utils libgl1-mesa-dri

# For older hardware, use software rendering
LIBGL_ALWAYS_SOFTWARE=1 ./zappy_gui
```

#### Python Import Errors

**Error:** `ModuleNotFoundError: No module named 'torch'`

**Solutions:**
```bash
# Verify Python version
python3 --version

# Check if virtual environment is activated
which python3

# Reinstall packages
pip install torch torchvision

# Use distribution packages if pip fails
sudo apt install python3-torch  # Ubuntu/Debian
```

#### CMake Version Issues

**Error:** `CMake 3.10 or higher is required`

**Solutions:**
```bash
# Ubuntu 18.04 - install from Kitware repository
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'
sudo apt update
sudo apt install cmake

# CentOS 7 - use cmake3
sudo yum install cmake3
alias cmake=cmake3

# Or build from source
wget https://cmake.org/files/v3.20/cmake-3.20.0.tar.gz
tar xzf cmake-3.20.0.tar.gz
cd cmake-3.20.0
./bootstrap && make && sudo make install
```

### Platform-Specific Issues

#### macOS

**Issue:** `ld: library not found for -lsfml-graphics`

**Solution:**
```bash
# Ensure Homebrew libraries are in path
export LIBRARY_PATH=/opt/homebrew/lib:$LIBRARY_PATH
export CPATH=/opt/homebrew/include:$CPATH

# Or specify explicitly in CMake
cmake .. -DCMAKE_PREFIX_PATH=/opt/homebrew
```

#### Windows/MinGW

**Issue:** Missing DLLs at runtime

**Solution:**
```bash
# Copy required DLLs to executable directory
cp /mingw64/bin/libsfml-*.dll ./
cp /mingw64/bin/libgcc_s_seh-1.dll ./
cp /mingw64/bin/libstdc++-6.dll ./
cp /mingw64/bin/libwinpthread-1.dll ./
```

### Getting Help

If you encounter issues not covered here:

1. **Check the FAQ:** [FAQ Documentation](faq.md)
2. **Search Issues:** [GitHub Issues](https://github.com/EpitechMirror/Zappy/issues)
3. **Ask for Help:** [GitHub Discussions](https://github.com/EpitechMirror/Zappy/discussions)
4. **Contact Team:** Send an email with your system details and error messages

## 🔗 Next Steps

After successful installation:

1. **Build Zappy:** Follow the [Building Guide](building.md)
2. **Quick Start:** Try the [Quick Start Guide](quickstart.md)  
3. **Development:** Read the [Contributing Guide](contributing.md)

---

You're now ready to build and run Zappy! The installation process sets up all necessary dependencies for the complete Zappy experience across server, GUI, and AI components.
