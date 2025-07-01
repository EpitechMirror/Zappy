# Zappy AI Unit Test Suite

This directory contains comprehensive unit tests for all AI components of the Zappy project.

## Test Files Overview

### 🧪 test_classes.py
Tests for the fundamental data classes and enums:
- **Direction Enum**: Tests direction values and count
- **ResourceType Enum**: Tests resource type values and count  
- **ActionType Enum**: Tests action type values and count
- **Position Class**: Tests position creation, distance calculations, and coordinate handling
- **GameState Class**: Tests state initialization, default values, and custom configurations
- **Experience Class**: Tests experience data structure for reinforcement learning

### 🧠 test_memory_system.py
Tests for the memory and experience management system:
- **Memory Initialization**: Tests capacity limits and data structure setup
- **Experience Management**: Tests adding, retrieving, and batching experiences
- **Strategy Storage**: Tests successful and failed strategy recording
- **Best Strategy Retrieval**: Tests finding optimal strategies based on reward and frequency
- **Capacity Management**: Tests memory limits and overflow handling

### 🤖 test_neural_network.py
Tests for the neural network implementation:
- **Network Architecture**: Tests layer initialization and dimensions
- **Activation Functions**: Tests sigmoid, ReLU, and their derivatives
- **Forward Pass**: Tests data flow through the network
- **Backward Pass**: Tests weight updates and gradient calculation
- **Training**: Tests learning and loss reduction
- **Model Persistence**: Tests saving and loading trained models
- **Edge Cases**: Tests extreme values and error handling

### 🤔 test_thinking_module.py
Tests for the strategic thinking and planning system:
- **Situation Analysis**: Tests threat detection, opportunity identification
- **Resource Analysis**: Tests resource availability and need calculation
- **Elevation Planning**: Tests level progression requirements
- **Goal Planning**: Tests action sequence generation for different objectives
- **Meta-Knowledge**: Tests learning from experience and pattern recognition
- **Strategic Decisions**: Tests plan creation for survival, gathering, and elevation

### 🎯 test_main_ai.py
Tests for the main AI orchestration system:
- **AI Initialization**: Tests component setup and configuration
- **Network Communication**: Tests server connection and message handling
- **State Management**: Tests game state updates and tracking
- **Decision Making**: Tests action selection and exploration/exploitation
- **Reward Calculation**: Tests reinforcement learning reward system
- **Performance Tracking**: Tests metrics collection and analysis
- **Learning Parameters**: Tests epsilon decay and adaptation

### 🚀 test_zappy_ai.py
Tests for the main entry point and command-line interface:
- **Command Line Parsing**: Tests argument validation and parsing
- **Application Initialization**: Tests AI setup with different configurations
- **Help System**: Tests help text and usage information
- **Error Handling**: Tests invalid arguments and error conditions
- **Learning Configuration**: Tests enabling/disabling learning features

## Running Tests

### Basic Test Execution
```bash
# Run all tests
python run_tests.py

# Run with verbose output
python run_tests.py -v

# Run specific test file
python run_tests.py -f test_classes.py

# Run specific test class
python run_tests.py -f test_classes.py --class TestPosition
```

### Coverage Reports
```bash
# Run tests with coverage
python run_tests.py -c

# This generates:
# - Terminal coverage report
# - HTML coverage report in htmlcov/
```

### Test Discovery
```bash
# List all available tests
python run_tests.py --list
```

### Using pytest directly
```bash
# Run all tests
pytest tests/

# Run with coverage
pytest tests/ --cov=src --cov-report=html

# Run specific test
pytest tests/test_classes.py::TestPosition::test_distance_calculation
```

## Test Coverage Goals

Our test suite aims for comprehensive coverage of:

### 🎯 Core Functionality (100% Coverage)
- All public methods and functions
- Data structure initialization and manipulation
- Enum value validation
- Mathematical calculations (distance, rewards, etc.)

### 🔄 State Management (95% Coverage)
- Game state updates and transitions
- Memory system operations
- Strategy storage and retrieval
- Performance metrics tracking

### 🧠 AI Decision Making (90% Coverage)
- Neural network forward/backward passes
- Action selection algorithms
- Reward calculation logic
- Learning parameter updates

### 🌐 Network Communication (85% Coverage)
- Socket connection handling
- Message parsing and sending
- Protocol compliance
- Error recovery

### 🛡️ Error Handling (80% Coverage)
- Invalid input handling
- Network failures
- File I/O errors
- Edge case scenarios

## Test Quality Standards

### ✅ Test Requirements
- Each test method tests a single concept
- Tests are independent and can run in any order
- Tests use descriptive names that explain what they test
- Tests include both positive and negative test cases
- Mock objects are used to isolate units under test

### 📊 Test Metrics
- **Assertion Quality**: Each test makes meaningful assertions
- **Edge Case Coverage**: Tests handle boundary conditions
- **Error Condition Testing**: Tests verify proper error handling
- **Performance Validation**: Tests verify acceptable performance

### 🔧 Test Maintenance
- Tests are updated when functionality changes
- Deprecated tests are removed promptly
- Test data is realistic and representative
- Test documentation is kept current

## Dependencies

The test suite requires the following packages:
```
numpy>=1.20.0
pytest>=6.0.0
pytest-cov>=2.10.0
unittest-mock (built-in with Python 3.3+)
```

Install test dependencies:
```bash
pip install numpy pytest pytest-cov
```

## Continuous Integration

The test suite is designed to work with CI/CD systems:

### GitHub Actions Example
```yaml
- name: Run Tests
  run: |
    pip install numpy pytest pytest-cov
    python ai/run_tests.py -c
```

### Coverage Reporting
- Minimum coverage threshold: 85%
- Coverage reports are generated in HTML format
- Failed tests break the CI pipeline

## Contributing

When adding new AI functionality:

1. **Write tests first** (TDD approach)
2. **Ensure tests pass** before submitting PR
3. **Maintain coverage** above 85%
4. **Document test purpose** in docstrings
5. **Test edge cases** and error conditions

### Test Naming Convention
```python
def test_[method]_[scenario]_[expected_result](self):
    """Test [what is being tested] when [conditions] should [expected outcome]"""
```

Example:
```python
def test_calculate_reward_level_progression_returns_high_reward(self):
    """Test reward calculation when player levels up should return high reward"""
```

## Troubleshooting

### Common Issues

1. **Import Errors**: Ensure the src directory is in Python path
2. **Missing Dependencies**: Install numpy, pytest, pytest-cov
3. **Path Issues**: Run tests from the ai/ directory
4. **Mock Failures**: Verify mock objects match actual interfaces

### Debug Mode
```bash
# Run tests with Python debugger
python -m pytest tests/ --pdb

# Run specific test with maximum verbosity
pytest tests/test_classes.py::TestPosition -vvv
```

### Performance Testing
```bash
# Run tests with timing information
pytest tests/ --durations=10
```

This comprehensive test suite ensures the reliability, maintainability, and correctness of the Zappy AI system! 🚀
