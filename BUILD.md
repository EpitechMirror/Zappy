# Zappy Project - Professional Build System

[![CI/CD Pipeline](https://github.com/your-username/zappy/actions/workflows/ci-cd.yml/badge.svg)](https://github.com/your-username/zappy/actions/workflows/ci-cd.yml)
[![Coverage Status](https://codecov.io/gh/your-username/zappy/branch/main/graph/badge.svg)](https://codecov.io/gh/your-username/zappy)
[![Docker Build](https://img.shields.io/docker/build/your-username/zappy.svg)](https://hub.docker.com/r/your-username/zappy)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

## 🚀 Quick Start

### Prerequisites

- **Linux/macOS**: Ubuntu 20.04+, macOS 11+
- **Build Tools**: GCC 9+, Make 4.0+, CMake 3.16+
- **Python**: Python 3.8+, pip
- **Optional**: Docker, Valgrind, lcov, cppcheck

### One-Command Build

```bash
# Clone and build everything
git clone https://github.com/your-username/zappy.git
cd zappy
./build.sh build
```

### Quick Start with Docker

```bash
# Build and run with Docker
docker-compose up
```

## 🏗️ Build System Overview

The Zappy project uses a **professional, enterprise-grade build system** with multiple interfaces:

### 1. Shell Script Interface (`build.sh`)
```bash
./build.sh build     # Build all components
./build.sh test      # Run all tests
./build.sh ci        # Full CI pipeline
./build.sh release   # Create release package
```

### 2. Makefile Interface
```bash
make all             # Build all components
make test            # Run all tests
make coverage        # Generate coverage reports
make release         # Create release package
```

### 3. NPM Scripts Interface
```bash
npm run build        # Build all components
npm run test         # Run all tests
npm run ci           # Full CI pipeline
npm run dev          # Development cycle
```

### 4. Docker Interface
```bash
docker-compose up                    # Run full stack
docker build -t zappy .              # Build image
docker run -p 8080:8080 zappy        # Run container
```

## 📊 Components

### Server Component (C)
- **Location**: `server/`
- **Build**: CMake + Make
- **Tests**: Custom test framework, 66+ unit tests
- **Coverage**: gcov integration
- **Memory**: Valgrind integration

### GUI Component (C++)
- **Location**: `gui/`
- **Build**: CMake + Raylib
- **Tests**: Criterion test framework
- **Graphics**: OpenGL/Raylib

### AI Component (Python)
- **Location**: `ai/`
- **Build**: Python virtual environment
- **Tests**: pytest with coverage
- **Dependencies**: NumPy, pytest, coverage tools

## 🔧 Available Commands

### Build Commands

| Command | Description | Example |
|---------|-------------|---------|
| `make all` | Build all components | `make all` |
| `make build-server` | Build server only | `make build-server` |
| `make build-gui` | Build GUI only | `make build-gui` |
| `make setup-ai` | Setup AI environment | `make setup-ai` |
| `make clean` | Clean build artifacts | `make clean` |

### Test Commands

| Command | Description | Example |
|---------|-------------|---------|
| `make test` | Run all tests | `make test` |
| `make test-server` | Test server only | `make test-server` |
| `make test-gui` | Test GUI only | `make test-gui` |
| `make test-ai` | Test AI only | `make test-ai` |
| `./test.sh all` | Comprehensive test suite | `./test.sh all` |

### Quality Assurance

| Command | Description | Example |
|---------|-------------|---------|
| `make coverage` | Generate coverage reports | `make coverage` |
| `make memory` | Memory leak detection | `make memory` |
| `make analyze` | Static code analysis | `make analyze` |
| `make format` | Code formatting | `make format` |

### Release & Deployment

| Command | Description | Example |
|---------|-------------|---------|
| `make release` | Create release package | `make release` |
| `make docker-build` | Build Docker image | `make docker-build` |
| `make install` | Install system-wide | `sudo make install` |

## 📋 Build Modes

### Debug Mode
```bash
make debug
# or
BUILD_TYPE=Debug make build
```

### Release Mode (Default)
```bash
make release
# or
BUILD_TYPE=Release make build
```

## 📊 Test Coverage

The build system provides comprehensive test coverage:

### Server Tests
- **Unit Tests**: 66+ tests covering all functions
- **Integration Tests**: Socket, protocol, game logic
- **Memory Tests**: Valgrind leak detection
- **Coverage**: gcov reports

### GUI Tests
- **Unit Tests**: Component testing
- **Rendering Tests**: Graphics pipeline
- **Input Tests**: User interaction

### AI Tests
- **Unit Tests**: pytest with 85%+ coverage
- **Integration Tests**: Server communication
- **Performance Tests**: Decision-making algorithms

## 🔄 Continuous Integration

### GitHub Actions Pipeline
```yaml
# Automated on every push/PR
- Code quality checks (linting, formatting)
- Multi-platform builds (Linux, macOS)
- Comprehensive test suite
- Security scanning
- Docker image building
- Performance benchmarking
```

### Local CI Simulation
```bash
./build.sh ci  # Run full CI pipeline locally
```

## 📦 Release Process

### Automatic Release
```bash
make release
```

Creates:
- `dist/zappy-1.0.0.tar.gz` - Release package
- `dist/zappy_server` - Server binary
- `dist/zappy_gui` - GUI binary
- `dist/run_ai.sh` - AI launcher script

### Manual Release Steps
1. Update version in `package.json`
2. Run `make release`
3. Test the release package
4. Create GitHub release
5. Upload artifacts

## 🐳 Docker Deployment

### Development Environment
```bash
docker-compose -f docker-compose.dev.yml up
```

### Production Environment
```bash
docker-compose -f docker-compose.prod.yml up
```

### Multi-Stage Builds
- **Base**: System dependencies
- **Builder**: Build tools and compilation
- **Runtime**: Minimal production image
- **Development**: Full development environment

## 🛠️ Development Workflow

### Initial Setup
```bash
# 1. Install dependencies
make install-deps

# 2. Setup environment
make setup

# 3. Build everything
make build

# 4. Run tests
make test
```

### Development Cycle
```bash
# 1. Make changes
# 2. Quick build and test
make dev

# 3. Full quality check
make ci

# 4. Watch mode (if available)
make watch
```

### Before Committing
```bash
# 1. Format code
make format

# 2. Run quality checks
make analyze

# 3. Run full test suite
make test

# 4. Check memory leaks
make memory
```

## 📁 Directory Structure

```
zappy/
├── build.sh              # Main build script
├── Makefile              # Professional Makefile
├── package.json          # NPM scripts and metadata
├── Dockerfile            # Multi-stage Docker build
├── docker-compose.yml    # Container orchestration
├── test.sh               # Comprehensive test runner
├── .github/workflows/    # CI/CD pipelines
├── server/               # C server component
│   ├── CMakeLists.txt
│   ├── Makefile
│   └── test/
├── gui/                  # C++ GUI component
│   ├── CMakeLists.txt
│   └── tests/
├── ai/                   # Python AI component
│   ├── requirements.txt
│   ├── run_tests.py
│   └── tests/
├── build/                # Build artifacts
├── dist/                 # Distribution files
├── coverage/             # Coverage reports
├── test-results/         # Test results
└── docs/                 # Documentation
```

## 🔍 Troubleshooting

### Common Issues

**Build fails with missing dependencies**
```bash
# Install dependencies
make install-deps
# or
sudo apt-get install build-essential cmake python3-dev
```

**Tests fail with permission errors**
```bash
# Fix permissions
chmod +x build.sh test.sh
```

**Docker build fails**
```bash
# Clean Docker cache
docker system prune -a
# Rebuild
make docker-build
```

**Memory leaks detected**
```bash
# Run memory check
make memory
# Check logs
cat logs/server_memory.log
```

### Debug Mode

**Enable verbose output**
```bash
export VERBOSE=1
make build
```

**Debug build**
```bash
make debug
gdb ./dist/zappy_server
```

## 📈 Performance Benchmarks

### Build Performance
- **Full build**: ~3-5 minutes
- **Incremental build**: ~30 seconds
- **Test suite**: ~2-3 minutes

### Runtime Performance
- **Server startup**: <1 second
- **GUI initialization**: <2 seconds
- **AI client connection**: <500ms

## 🤝 Contributing

### Development Setup
```bash
# 1. Fork the repository
# 2. Clone your fork
git clone https://github.com/your-username/zappy.git

# 3. Setup development environment
make dev-setup

# 4. Create feature branch
git checkout -b feature/new-feature

# 5. Make changes and test
make dev

# 6. Submit pull request
```

### Code Quality Standards
- **C/C++**: Follow Epitech coding standard
- **Python**: PEP 8 with black formatting
- **Tests**: Minimum 85% coverage
- **Documentation**: Update README and docs

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Built with modern DevOps practices
- Inspired by enterprise-grade build systems
- Follows industry standards for multi-component projects
- Designed for scalability and maintainability

---

**Built with ❤️ by the Zappy Team**

For more information, see the [full documentation](docs/) or contact the maintainers.
