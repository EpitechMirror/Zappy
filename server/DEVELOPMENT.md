# Zappy Server Development Guide

## 🚀 Quick Start

```bash
# Set up development environment
make dev-setup

# Build and test
make build
make test

# Run server
make run
```

## 🏗️ Build System

The server uses a dual build system approach:

### CMake (Primary)
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Make (Convenience)
```bash
make build          # Uses CMake internally
make rebuild         # Clean and build
make debug           # Debug build
make release         # Optimized build
```

## 🧪 Testing Framework

### Comprehensive Unit Tests
- **66+ unit tests** covering every server source file
- Custom lightweight testing framework
- Memory leak detection with Valgrind
- Code coverage analysis with gcov

### Test Commands
```bash
make test            # Run unit tests
make test-memory     # Run with memory leak detection
make test-coverage   # Run with code coverage
make test-all        # Comprehensive testing suite
```

### Test Structure
```
server/test/
├── test_framework.h/.c      # Testing framework
├── test_parse.c            # Tests for parse.c
├── test_create_server_socket.c  # Tests for socket creation
├── test_remove_client.c    # Tests for client management
├── test_handle_client_data.c    # Tests for data handling
├── test_connection.c       # Tests for connection handling
├── test_accept.c          # Tests for accept/polling
├── test_main.c            # Tests for main functionality
├── run_tests.sh           # Test runner script
├── Makefile              # Test build system
└── README.md             # Test documentation
```

## 📊 Quality Assurance

### Static Analysis
```bash
make analyze         # Run cppcheck static analysis
make style-check     # Check Epitech coding style
```

### Memory Safety
```bash
make test-memory     # Valgrind memory leak detection
```

### Code Coverage
```bash
make test-coverage   # Generate coverage reports
```

## 🏛️ Architecture

### Core Components

#### Main (`src/main.c`)
- Server initialization
- Command line argument handling
- Main server loop startup

#### Argument Parsing (`src/parse.c`)
- Command line flag processing
- Configuration validation
- Error handling for invalid arguments

#### Socket Management (`src/functional/create_server_socket.c`)
- TCP socket creation
- Port binding and listening
- Socket option configuration

#### Client Management (`src/functional/`)
- **accept.c** - Client acceptance and polling
- **connection.c** - New connection handling
- **remove_client.c** - Client removal and cleanup
- **handle_client_data.c** - Client data processing and authentication

### Data Structures

#### Server Configuration (`include/flag.h`)
```c
typedef struct server_config_s {
    int port;           // Server port
    int width;          // World width
    int height;         // World height
    int clients_nb;     // Clients per team
    int freq;           // Time frequency
    int team_count;     // Number of teams
    char **team_names;  // Team names array
} server_config_t;
```

#### Client Information (`include/Client_Info.h`)
```c
typedef struct client_s {
    int fd;                    // File descriptor
    bool connected;            // Connection status
    bool is_ready;            // Ready state
    bool is_alive;            // Alive state
    int x, y, id;             // Position and ID
    char *team_name;          // Team name
    bool is_graphic;          // Graphic client flag
    client_state_t state;     // Authentication state
    struct client_s *next;    // Linked list pointer
} client_t;
```

## 🔧 Development Workflow

### 1. Setup
```bash
git clone <repository>
cd server
make install-deps    # Install required tools
make dev-setup      # Build and run initial tests
```

### 2. Development Cycle
```bash
# Make changes to source code
make rebuild        # Rebuild server
make test          # Run unit tests
make test-memory   # Check for memory leaks
```

### 3. Quality Checks
```bash
make analyze       # Static code analysis
make style-check   # Coding style verification
make test-all      # Comprehensive testing
```

### 4. Debugging
```bash
make debug         # Build debug version
gdb ./build-debug/zappy_server
valgrind ./zappy_server -p 8080 -x 10 -y 10 -n team1 -c 5 -f 100
```

## 🚦 CI/CD Integration

### GitHub Actions Workflow
The server includes a comprehensive CI/CD pipeline:

1. **Build Verification**
   - CMake build on Ubuntu
   - Multi-threaded compilation
   - Build artifact generation

2. **Unit Testing**
   - 66+ comprehensive unit tests
   - Memory leak detection
   - Code coverage analysis

3. **Quality Assurance**
   - Static code analysis (cppcheck)
   - Epitech coding style compliance
   - Documentation verification

4. **Integration Testing**
   - Server startup verification
   - Error handling validation
   - Invalid argument testing

5. **Performance Testing** (main branch only)
   - Optimized builds
   - Performance benchmarks
   - Resource usage analysis

### Workflow Triggers
- Push to `dev` or `main` branches
- Pull requests targeting `dev` or `main`
- Changes to `server/**` or workflow files

## 📝 Coding Standards

### Epitech Style Guide
- No tabs (use 4 spaces)
- No trailing whitespace
- Proper header comments
- Function documentation
- Error handling for all system calls

### Best Practices
- **Memory Management**: Always free allocated memory
- **Error Handling**: Check return values of system calls
- **Threading**: Use thread-safe operations where needed
- **Security**: Validate all input data
- **Performance**: Optimize critical paths

## 🐛 Debugging Guide

### Common Issues

#### Build Failures
```bash
make clean rebuild  # Clean rebuild
make install-deps   # Ensure dependencies
```

#### Test Failures
```bash
cd test
./run_tests.sh      # Verbose test output
valgrind ./run_tests # Memory debugging
```

#### Runtime Issues
```bash
strace ./zappy_server -p 8080 -x 10 -y 10 -n team1 -c 5 -f 100
gdb --args ./zappy_server -p 8080 -x 10 -y 10 -n team1 -c 5 -f 100
```

### Memory Debugging
```bash
valgrind --leak-check=full --show-leak-kinds=all ./zappy_server <args>
```

### Network Debugging
```bash
netstat -tlnp | grep 8080  # Check port usage
lsof -i :8080             # List processes using port
tcpdump -i lo port 8080   # Monitor network traffic
```

## 📚 Testing Guidelines

### Writing New Tests
1. Create test file: `test/test_<module>.c`
2. Include framework: `#include "test_framework.h"`
3. Write test functions returning 1 (pass) or 0 (fail)
4. Use assertion macros: `TEST_ASSERT`, `TEST_ASSERT_EQUAL`, etc.
5. Create test suite array and runner function
6. Add runner to `run_all_tests.c`

### Test Categories
- **Unit Tests**: Individual function testing
- **Integration Tests**: Component interaction testing
- **Memory Tests**: Leak detection and memory safety
- **Edge Cases**: Boundary conditions and error scenarios

## 🔄 Maintenance

### Regular Tasks
- Run `make test-all` before commits
- Update tests when adding new features
- Monitor CI/CD pipeline status
- Review and update documentation

### Performance Monitoring
- Regular benchmarking with `make test-all`
- Memory usage profiling
- Network performance testing
- Scalability analysis

## 📖 Additional Resources

- [Server Architecture](server_architecture.md)
- [Test Documentation](test/README.md)
- [Test Implementation Summary](test/IMPLEMENTATION_SUMMARY.md)
- [GitHub Actions Workflow](.github/workflows/server-build.yml)

---

**Ready to develop! 🎉**

For questions or issues, refer to the comprehensive test suite and documentation, or check the CI/CD pipeline for automated quality assurance.
