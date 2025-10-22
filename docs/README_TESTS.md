# Essos Library L1 Test Cases

This directory contains comprehensive Level 1 (L1) test cases for the Essos library, a portable application framework for OpenGL ES applications that can run on direct EGL or Wayland environments.

## Overview

The L1 test suite provides thorough validation of:
- **Basic API functionality** - Core context management, initialization, configuration
- **Success and failure scenarios** - Both positive and negative test cases
- **Boundary conditions** - Edge cases, extreme values, invalid inputs
- **Error handling** - Proper error reporting and graceful degradation
- **Resource management** - Memory leaks, proper cleanup, resource lifecycle
- **State transitions** - Context states, mode switching, configuration changes
- **Thread safety** - Concurrent operations, race conditions
- **Platform compatibility** - Wayland vs Direct mode, different display types

## Test Structure

### Test Categories

1. **Context Management Tests**
   - Context creation/destruction
   - Multiple context handling
   - Error state management
   - Resource cleanup validation

2. **Display Configuration Tests**
   - Display size and position settings
   - Window configuration
   - Display mode switching
   - EGL display type handling

3. **Input Event Tests**
   - Keyboard event handling
   - Pointer/mouse event processing
   - Touch event management
   - Key repeat configuration

4. **Gamepad Tests**
   - Gamepad connection/disconnection
   - Button and axis event handling
   - Device information retrieval
   - State management

5. **Platform Specific Tests**
   - Wayland vs Direct mode
   - Platform capability queries
   - EGL configuration
   - Native window management

6. **Boundary and Stress Tests**
   - Extreme parameter values
   - Resource exhaustion scenarios
   - Concurrent operations
   - Memory pressure conditions

### Test Classes

- `EssosL1Test` - Base test fixture with context setup/teardown
- `EssosGamepadL1Test` - Gamepad-specific test fixture with event callbacks  
- `EssosInputL1Test` - Input event test fixture with comprehensive callbacks

## Files

- **`test_essos_l1.cpp`** - Main test file with all L1 test cases
- **`CMakeLists.txt`** - CMake build configuration
- **`Makefile`** - Alternative Makefile build system
- **`run_tests.sh`** - Test runner script with automation
- **`README.md`** - This documentation file

## Building and Running

### Prerequisites

#### Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libgtest-dev \
    libgmock-dev \
    libegl1-mesa-dev \
    libwayland-dev \
    libwayland-egl1-mesa \
    pkg-config \
    gcov \
    lcov
```

#### CentOS/RHEL:
```bash
sudo yum install -y \
    gcc-c++ \
    cmake \
    gtest-devel \
    gmock-devel \
    mesa-libEGL-devel \
    wayland-devel \
    pkgconfig \
    gcov
```

### Build Methods

#### Method 1: Using CMake (Recommended)
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
./essos_l1_tests
```

#### Method 2: Using Makefile
```bash
make clean && make -j$(nproc)
./essos_l1_tests
```

#### Method 3: Using Test Runner Script (Linux/WSL)
```bash
# Install dependencies
./run_tests.sh install-deps

# Run complete test suite
./run_tests.sh full

# Or run individual steps
./run_tests.sh build
./run_tests.sh test
./run_tests.sh coverage
```

### Windows Development

On Windows, you can use:
- **WSL (Windows Subsystem for Linux)** - Recommended approach
- **MSYS2/MinGW** - Alternative with some modifications needed
- **Visual Studio** - Requires project file generation

#### WSL Setup:
```powershell
# Install WSL Ubuntu
wsl --install -d Ubuntu

# Switch to WSL and follow Linux instructions
wsl
cd /mnt/d/Westeros/essos-topic-RDK-58448_1
./run_tests.sh install-deps
./run_tests.sh full
```

## Test Execution Options

### Basic Test Run
```bash
./essos_l1_tests
```

### Verbose Output with XML Results
```bash
./essos_l1_tests --gtest_output=xml:test_results.xml --gtest_print_time=1
```

### Running Specific Test Suites
```bash
# Run only context management tests
./essos_l1_tests --gtest_filter="EssosL1Test.*"

# Run only gamepad tests  
./essos_l1_tests --gtest_filter="EssosGamepadL1Test.*"

# Run only input tests
./essos_l1_tests --gtest_filter="EssosInputL1Test.*"
```

### Running Specific Tests
```bash
# Run specific test case
./essos_l1_tests --gtest_filter="EssosL1Test.ContextCreate_Success"

# Run tests matching pattern
./essos_l1_tests --gtest_filter="*Context*"
```

## Coverage Analysis

### Generate Coverage Report
```bash
# Build with coverage flags
make clean && make CXXFLAGS="-g -O0 --coverage"

# Run tests
./essos_l1_tests

# Generate coverage report
gcov *.cpp
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html

# View report
xdg-open coverage_html/index.html
```

### Expected Coverage Metrics
- **Line Coverage**: > 85%
- **Function Coverage**: > 90%
- **Branch Coverage**: > 80%

## Quality Assurance

### Static Analysis
```bash
# Using cppcheck
cppcheck --enable=all --std=c++14 --suppress=missingIncludeSystem .

# Using clang-static-analyzer
scan-build make
```

### Memory Leak Detection
```bash
# Using Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./essos_l1_tests

# Using AddressSanitizer
make CXXFLAGS="-g -O1 -fsanitize=address -fno-omit-frame-pointer"
./essos_l1_tests
```

### Performance Profiling
```bash
# Using perf
perf record ./essos_l1_tests
perf report

# Using gprof
make CXXFLAGS="-g -pg"
./essos_l1_tests
gprof essos_l1_tests gmon.out > profile_report.txt
```

## Test Case Categories

### Success Scenarios (Positive Tests)
- Valid parameter ranges
- Normal operation flows
- Standard use cases
- Resource allocation/deallocation
- Event callback registration
- Configuration changes

### Failure Scenarios (Negative Tests)  
- NULL pointer parameters
- Invalid parameter values
- Out-of-range inputs
- Resource exhaustion
- Invalid state transitions
- Unsupported operations

### Boundary Tests
- Minimum/maximum valid values
- Zero and negative values
- Very large numbers
- Empty strings
- Buffer limits

### Concurrency Tests
- Multiple contexts simultaneously
- Concurrent operations on same context
- Thread safety validation
- Race condition detection

## Expected Test Results

### Total Tests: ~150+ test cases

#### Context Management (25+ tests)
- ✅ Context creation/destruction
- ✅ Multiple context handling  
- ✅ Error reporting
- ✅ Resource cleanup

#### Display & EGL (30+ tests)
- ✅ Display configuration
- ✅ EGL setup and teardown
- ✅ Window management
- ✅ Mode switching

#### Input Events (40+ tests)  
- ✅ Keyboard events
- ✅ Pointer events
- ✅ Touch events
- ✅ Event listener management

#### Gamepad (25+ tests)
- ✅ Connection handling
- ✅ Button/axis events
- ✅ Device information
- ✅ State queries

#### Platform Support (15+ tests)
- ✅ Wayland compatibility
- ✅ Direct mode support
- ✅ Platform detection
- ✅ Feature queries

#### Stress & Boundary (15+ tests)
- ✅ Resource limits
- ✅ Extreme values
- ✅ Concurrent operations
- ✅ Error conditions

## Troubleshooting

### Common Issues

#### Build Errors
```bash
# Missing dependencies
./run_tests.sh install-deps

# Clean rebuild
make clean && make
```

#### Runtime Errors
```bash
# Check EGL availability
eglinfo

# Verify Wayland support
echo $WAYLAND_DISPLAY

# Check permissions
ls -la /dev/dri/
```

#### Test Failures
```bash
# Run with debugging
gdb ./essos_l1_tests
(gdb) run --gtest_filter="FailingTest"
(gdb) bt

# Enable debug logging
export ESS_DEBUG=1
./essos_l1_tests
```

### Platform-Specific Notes

#### Direct EGL Mode
- Requires direct access to GPU
- May need root privileges
- Check /dev/dri permissions

#### Wayland Mode  
- Requires running Wayland compositor
- Set WAYLAND_DISPLAY environment variable
- May need wayland-protocols package

#### Headless Testing
- Use Xvfb for X11 simulation
- Configure EGL_MESA_platform_surfaceless
- Use software rendering for CI/CD

## Continuous Integration

### GitHub Actions Example
```yaml
name: Essos L1 Tests
on: [push, pull_request]
jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install dependencies
        run: ./run_tests.sh install-deps
      - name: Build and test
        run: ./run_tests.sh full
      - name: Upload results
        uses: actions/upload-artifact@v2
        with:
          name: test-results
          path: |
            test_results.xml
            coverage_html/
```

### Jenkins Pipeline
```groovy
pipeline {
    agent any
    stages {
        stage('Build') {
            steps {
                sh './run_tests.sh build'
            }
        }
        stage('Test') {
            steps {
                sh './run_tests.sh test'
            }
            post {
                always {
                    junit 'test_results.xml'
                }
            }
        }
        stage('Coverage') {
            steps {
                sh './run_tests.sh coverage'
            }
            post {
                always {
                    publishHTML([
                        allowMissing: false,
                        alwaysLinkToLastBuild: true,
                        keepAll: true,
                        reportDir: 'coverage_html',
                        reportFiles: 'index.html',
                        reportName: 'Coverage Report'
                    ])
                }
            }
        }
    }
}
```

## Contributing

When adding new test cases:

1. **Follow naming conventions**: `TestSuite.TestName_Scenario`
2. **Use descriptive assertions**: Include meaningful error messages
3. **Test both success and failure**: Cover positive and negative cases
4. **Document test purpose**: Add comments explaining test objectives
5. **Verify resource cleanup**: Ensure no leaks or dangling pointers
6. **Consider thread safety**: Add concurrent tests for shared resources

### Test Template
```cpp
TEST_F(EssosL1Test, FunctionName_Scenario) {
    // Arrange - Set up test conditions
    
    // Act - Call function under test
    
    // Assert - Verify expected results
    EXPECT_TRUE(result) << "Descriptive failure message";
}
```

## References

- [Essos Library Documentation](https://github.com/rdkcentral/essos)
- [Google Test Framework](https://github.com/google/googletest)
- [EGL Specification](https://www.khronos.org/egl/)
- [Wayland Protocol](https://wayland.freedesktop.org/)
- [RDK Documentation](https://wiki.rdkcentral.com/)