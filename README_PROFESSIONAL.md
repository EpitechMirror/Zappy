# 🎮 Zappy Project

[![Build Status](https://github.com/epitech/zappy/workflows/CI/badge.svg)](https://github.com/epitech/zappy/actions)
[![Code Coverage](https://codecov.io/gh/epitech/zappy/branch/main/graph/badge.svg)](https://codecov.io/gh/epitech/zappy)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![CMake](https://img.shields.io/badge/CMake-3.16+-blue.svg)](https://cmake.org/)

A sophisticated network game featuring AI agents, real-time visualization, and advanced game mechanics. Zappy demonstrates enterprise-level software architecture with modular design, comprehensive testing, and professional development practices.

## 🚀 Quick Start

```bash
# Clone the repository
git clone https://github.com/epitech/zappy.git
cd zappy

# Build all components
./build.sh

# Run with Docker Compose
docker-compose up
```

## 📋 Table of Contents

- [Features](#-features)
- [Architecture](#-architecture)
- [Prerequisites](#-prerequisites)
- [Installation](#-installation)
- [Usage](#-usage)
- [Development](#-development)
- [Testing](#-testing)
- [Deployment](#-deployment)
- [Documentation](#-documentation)
- [Contributing](#-contributing)
- [License](#-license)

## ✨ Features

### 🖥️ Server Component
- **High-performance network server** with multi-client support
- **Real-time game state management** with optimized data structures
- **Protocol-based communication** with clients and GUI
- **Configurable game parameters** (map size, teams, frequency)
- **Memory-safe implementation** with comprehensive error handling

### 🎨 GUI Component  
- **Real-time 3D visualization** using Raylib graphics engine
- **Interactive camera controls** and smooth animations
- **Resource and player visualization** with modern UI elements
- **Performance monitoring** and debug information display
- **Cross-platform compatibility** (Linux, Windows, macOS)

### 🤖 AI Component
- **Advanced AI algorithms** with neural network integration
- **Multiple AI strategies** (aggressive, defensive, balanced)
- **Machine learning capabilities** with training data collection
- **Adaptive behavior** based on game state analysis
- **Extensible architecture** for custom AI implementations

## 🏗️ Architecture

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Zappy Server  │◄──►│   Zappy GUI     │    │   Zappy AI      │
│                 │    │                 │    │                 │
│ • Game Logic    │    │ • 3D Rendering  │    │ • ML Algorithms │
│ • Network Comm  │    │ • User Interface│    │ • Strategy Logic│
│ • State Mgmt    │    │ • Real-time Viz │    │ • Adaptation    │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         └───────────────────────┼───────────────────────┘
                                 │
                    ┌─────────────────┐
                    │  Network Layer  │
                    │                 │
                    │ • TCP Protocol  │
                    │ • Message Queue │
                    │ • Event System  │
                    └─────────────────┘
```

## 🔧 Prerequisites

### System Requirements
- **Operating System**: Linux (Ubuntu 20.04+), macOS (10.15+), Windows 10+
- **Memory**: 4GB RAM minimum, 8GB recommended
- **Storage**: 2GB available space
- **Network**: TCP/IP connectivity

### Build Dependencies
- **CMake** 3.16 or higher
- **GCC** 9.0+ or **Clang** 10.0+
- **Python** 3.8+ (for AI component)
- **Git** for version control

### Runtime Dependencies
- **Raylib** 5.0 (automatically fetched by CMake)
- **pthread** library
- **Python packages** (see ai/requirements.txt)

## 📦 Installation

### Method 1: Automated Build Script (Recommended)

```bash
# Clone repository
git clone https://github.com/epitech/zappy.git
cd zappy

# Run setup (installs dependencies)
./dev.sh setup

# Build all components
./build.sh

# Build specific components
./build.sh --server-only
./build.sh --gui-only  
./build.sh --ai-only
```

### Method 2: Manual CMake Build

```bash
# Configure build
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build project
cmake --build build -j$(nproc)

# Install (optional)
cmake --install build --prefix ./install
```

### Method 3: Docker Build

```bash
# Build all services
docker-compose build

# Run complete environment
docker-compose up

# Development environment
docker-compose --profile dev up
```

## 🎯 Usage

### Starting the Server

```bash
# Basic usage
./build/bin/zappy_server -p 4242 -x 10 -y 10 -n team1 team2 -c 2 -f 100

# With custom parameters
./build/bin/zappy_server \
    --port 4242 \
    --width 20 \
    --height 20 \
    --names team1 team2 team3 \
    --clients-nb 3 \
    --freq 200
```

### Running the GUI

```bash
# Connect to local server
./build/bin/zappy_gui -p 4242 -h localhost

# Connect to remote server
./build/bin/zappy_gui -p 4242 -h game.server.com
```

### Launching AI Clients

```bash
# Activate AI environment
cd ai && source .venv/bin/activate

# Run AI client
python src/main_ai.py -p 4242 -n team1 -h localhost

# Run multiple AI instances
./ai/run_tests.py --team team1 --instances 3
```

## 💻 Development

### Development Scripts

```bash
# Development utility script
./dev.sh [command]

# Available commands:
./dev.sh setup      # Initial setup
./dev.sh build      # Build project
./dev.sh test       # Run tests
./dev.sh server     # Start server
./dev.sh gui        # Start GUI
./dev.sh ai         # Start AI
./dev.sh clean      # Clean artifacts
./dev.sh format     # Format code
./dev.sh lint       # Run linters
```

### Build Options

```bash
# Debug build with sanitizers
./build.sh --debug --coverage

# Release build with optimizations
./build.sh --clean

# Build with specific components
./build.sh --server-only --tests

# Parallel build
./build.sh -j8

# Install after build
./build.sh --install --package
```

### Code Quality Tools

```bash
# Format code
./dev.sh format

# Run linters
./dev.sh lint

# Pre-commit hooks
pre-commit install
pre-commit run --all-files

# Static analysis
cmake --build build --target server-cppcheck
```

## 🧪 Testing

### Unit Tests

```bash
# Run all tests
./build.sh --tests

# Run specific component tests
cmake --build build --target server-test
cmake --build build --target gui-test
./dev.sh ai-test

# Memory testing with Valgrind
cmake --build build --target server-test-memory
```

### Integration Tests

```bash
# Full integration test suite
./dev.sh test

# AI integration tests
cd ai && python run_tests.py

# Network protocol tests
./test/integration/protocol_test.sh
```

### Performance Testing

```bash
# Load testing
./test/performance/load_test.sh

# Memory profiling
valgrind --tool=massif ./build/bin/zappy_server

# CPU profiling
perf record ./build/bin/zappy_server
perf report
```

### Coverage Reports

```bash
# Generate coverage report
./build.sh --coverage

# View HTML coverage report
open build/coverage_html/index.html

# Upload to codecov
curl -s https://codecov.io/bash | bash
```

## 🚀 Deployment

### Docker Deployment

```bash
# Production deployment
docker-compose up -d

# Scale AI clients
docker-compose up --scale ai-team1=5 --scale ai-team2=5

# Update deployment
docker-compose pull && docker-compose up -d
```

### Kubernetes Deployment

```bash
# Apply Kubernetes manifests
kubectl apply -f k8s/

# Monitor deployment
kubectl get pods -l app=zappy

# Scale deployment
kubectl scale deployment zappy-server --replicas=3
```

### Cloud Deployment

```bash
# AWS ECS deployment
aws ecs create-service --cli-input-json file://aws/ecs-service.json

# Google Cloud Run
gcloud run deploy zappy-server --source .

# Azure Container Instances
az container create --resource-group zappy --file azure/container.yaml
```

## 📚 Documentation

- **[User Guide](docs/user-guide.md)** - Complete usage instructions
- **[API Documentation](docs/api.md)** - Protocol and API reference
- **[Developer Guide](docs/developer-guide.md)** - Development setup and guidelines
- **[Architecture Guide](docs/architecture.md)** - System design and components
- **[Deployment Guide](docs/deployment.md)** - Production deployment instructions

### Generating Documentation

```bash
# Build documentation
./dev.sh docs

# Serve documentation locally
cd docs && python -m http.server 8000

# Deploy documentation
./scripts/deploy-docs.sh
```

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md).

### Development Workflow

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Install** pre-commit hooks (`pre-commit install`)
4. **Make** your changes with tests
5. **Commit** your changes (`git commit -m 'Add amazing feature'`)
6. **Push** to the branch (`git push origin feature/amazing-feature`)
7. **Open** a Pull Request

### Code Standards

- **C/C++**: Follow Google C++ Style Guide
- **Python**: Follow PEP 8 with Black formatting
- **CMake**: Use modern CMake practices
- **Git**: Use Conventional Commits format

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Epitech** for the project specifications
- **Raylib** community for the graphics engine
- **CMake** team for the build system
- **Open source** contributors and maintainers

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/epitech/zappy/issues)
- **Discussions**: [GitHub Discussions](https://github.com/epitech/zappy/discussions)
- **Documentation**: [Project Wiki](https://github.com/epitech/zappy/wiki)
- **Email**: team@epitech.eu

---

<div align="center">
  <p>Made with ❤️ by the Epitech Team</p>
  <p>
    <a href="https://github.com/epitech/zappy">🌟 Star us on GitHub</a> •
    <a href="https://twitter.com/epitech">🐦 Follow us on Twitter</a> •
    <a href="https://epitech.eu">🌐 Visit our website</a>
  </p>
</div>
