# L1 Test Suite - Essos Project

## Overview
Comprehensive L1 test suite for essos-main with 75%+ coverage target.

## Directory Structure
```
L1/
├── CMakeLists.txt              # CMake build configuration
├── mocks/                      # Mock implementations
│   ├── mock_wayland.cpp        # Wayland compositor/client mock
│   ├── mock_egl.cpp            # EGL graphics mock
│   ├── mock_xkb.cpp            # XKB keyboard mock
│   └── mock_system.cpp         # System call mocks
└── tests/                      # Test files
    ├── test_main.cpp           # GTest entry point
    ├── L1_essos_context_tests.cpp      # Context lifecycle tests
    ├── L1_essos_display_tests.cpp      # Display management tests
    ├── L1_essos_input_tests.cpp        # Input handling tests
    ├── L1_essos_listener_tests.cpp     # Event listener tests
    ├── L1_essos_window_tests.cpp       # Window management tests
    └── L1_essos_integration_tests.cpp  # Integration/workflow tests
```

## Test Coverage

### 1. Context Tests (15+ test cases)
- `EssContextCreate` / `EssContextDestroy`
- `EssContextInit`
- `EssContextSetName`
- `EssContextGetLastErrorDetail`
- NULL handling, multiple contexts, lifecycle

### 2. Display Tests (12+ test cases)
- `EssContextGetDisplaySize`
- `EssContextGetDisplaySafeArea`
- `EssContextGetEGLDisplayType`
- Bounds checking, consistency validation

### 3. Input Tests (20+ test cases)
- `EssContextSetKeyListener` - keyboard events
- `EssContextSetPointerListener` - mouse/pointer
- `EssContextSetTouchListener` - touch input
- Callback verification, user data handling

### 4. Listener Tests (15+ test cases)
- `EssContextSetSettingsListener` - display settings
- `EssContextSetTerminateListener` - termination events
- Partial callbacks, replacement, clearing

### 5. Window Tests (15+ test cases)
- `EssContextCreateNativeWindow`
- `EssContextDestroyNativeWindow`
- Various sizes, NULL validation, lifecycle

### 6. Integration Tests (15+ test cases)
- `EssContextStart` / `EssContextStop`
- `EssContextRunEventLoopOnce`
- `EssContextUpdateDisplay`
- Complete application workflows

## Building and Running

### Prerequisites
```bash
# Install dependencies
sudo apt-get install -y \
    libgtest-dev \
    libwayland-dev \
    libegl1-mesa-dev \
    libxkbcommon-dev \
    lcov \
    gcovr
```

### Build
```bash
cd essos-main/L1
mkdir -p build
cd build
cmake ..
make
```

### Run Tests
```bash
# Run all tests
./essos_l1_tests

# Run with verbose output
./essos_l1_tests --gtest_verbose

# Run specific test suite
./essos_l1_tests --gtest_filter="EssosContextTest.*"
```

### Generate Coverage
```bash
# Generate coverage report
make coverage

# View coverage report
google-chrome coverage/index.html
```

## Mock Layer

### Wayland Mock
- Implements `wl_display`, `wl_registry`, `wl_compositor`
- Simulates `wl_surface`, `wl_seat`, `wl_pointer`, `wl_keyboard`, `wl_touch`
- Control functions: `mock_wayland_reset()`, `mock_wayland_set_dispatch_fail()`

### EGL Mock
- Implements EGL 1.4 API
- Functions: `eglGetDisplay`, `eglInitialize`, `eglCreateContext`
- Mock structures: `_EGLDisplay`, `_EGLConfig`, `_EGLSurface`, `_EGLContext`

### XKB Mock
- Keyboard context and keymap management
- Reference counting, state updates
- Keysym conversion

### System Mock
- `poll()` for event loop testing
- Configurable timeout/failure modes

## Coverage Target

**Goal: 75%+ line and branch coverage of essos.cpp**

### Coverage Metrics
- Line coverage: Target 75%+
- Branch coverage: Target 70%+
- Function coverage: Target 80%+

### Coverage Report Files
- `coverage/index.html` - HTML report
- `coverage.xml` - Cobertura XML
- `coverage.info` - LCOV format

## Test Patterns

### Fixture Pattern
```cpp
class EssosContextTest : public ::testing::Test {
protected:
    EssCtx *ctx;
    
    void SetUp() override {
        mock_wayland_reset();
        ctx = EssContextCreate();
    }
    
    void TearDown() override {
        if (ctx) {
            EssContextDestroy(ctx);
        }
    }
};
```

### Callback Testing
```cpp
static bool callback_called = false;
static void callback_function(void *userData, ...) {
    callback_called = true;
}

TEST_F(EssosTest, TestCallback) {
    callback_called = false;
    EssContextSetListener(ctx, callback_function, nullptr);
    // Trigger event
    EXPECT_TRUE(callback_called);
}
```

## Integration with CI/CD

### GitHub Actions
The L1 tests are integrated into `.github/workflows/L1.yml`:
```yaml
- name: Build L1 Tests
  run: |
    cd essos-main/L1/build
    cmake ..
    make

- name: Run L1 Tests
  run: |
    cd essos-main/L1/build
    ./essos_l1_tests

- name: Generate Coverage
  run: |
    cd essos-main/L1/build
    make coverage
```

## Troubleshooting

### Build Failures
```bash
# Check GTest installation
dpkg -l | grep gtest

# Check Wayland installation
pkg-config --modversion wayland-client

# Clean and rebuild
rm -rf build
mkdir build && cd build
cmake .. && make
```

### Test Failures
```bash
# Run with debugging
./essos_l1_tests --gtest_break_on_failure

# Run single test
./essos_l1_tests --gtest_filter="EssosContextTest.CreateDestroy_Success"

# Verbose output
./essos_l1_tests --gtest_verbose
```

### Coverage Issues
```bash
# Check coverage files
ls -la CMakeFiles/essos_l1_tests.dir/tests/*.gcno

# Manual coverage generation
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage
```

## Maintenance

### Adding New Tests
1. Create test file in `L1/tests/`
2. Add to `CMakeLists.txt` in `TEST_SOURCES`
3. Follow naming convention: `L1_essos_<feature>_tests.cpp`
4. Use test fixtures for setup/teardown
5. Include NULL parameter validation

### Updating Mocks
1. Edit mock file in `L1/mocks/`
2. Add control functions as needed
3. Update reset functions
4. Document new mock behavior

### Coverage Threshold
If coverage drops below 75%:
1. Identify uncovered lines with `lcov --summary coverage.info`
2. Add tests for uncovered paths
3. Review branch coverage for conditionals
4. Test error paths and edge cases

## Test Statistics

**Total Test Cases: 90+**
- Context Tests: 15
- Display Tests: 12
- Input Tests: 20
- Listener Tests: 15
- Window Tests: 15
- Integration Tests: 15

**Mock Functions: 80+**
- Wayland: 40+
- EGL: 30+
- XKB: 8+
- System: 2+

**Expected Coverage: 75-85%**
- essos.cpp: 75%+
- Mock coverage: 100% (by definition)
- Test coverage: N/A (tests don't need coverage)

## License
Copyright 2024 RDK Management. Licensed under Apache 2.0.
