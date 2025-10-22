# Essos L1 Test Suite - Architecture Guide

## Directory Structure

The project follows a clean separation of concerns with the following layout:

```
essos-topic-RDK-58448_1/
├── src/                  # Essos source implementation files
│   ├── essos.cpp         # Main essos implementation
│   └── ...               # Other essos sources
├── L1/                   # L1 test suite files
│   ├── test_essos_l1.cpp # L1 tests
│   └── ...               # Other test files
├── mock/                 # Mock implementations and headers
│   ├── essos_mock.h      # Mock headers
│   ├── wayland-stubs.h   # Wayland stub headers
│   └── ...               # Other mocks/stubs
├── .github/
│   └── workflows/
│       └── ci.yml        # GitHub Actions CI workflow
├── tools/                # Build and CI helper scripts
│   ├── ci_build.sh       # Linux CI build script
│   └── ci_build.bat      # Windows CI build script
├── CMakeLists.txt        # Main CMake configuration
└── README_CI.md          # CI and build documentation
```

## Design Principles

### 1. **Backward Compatibility**
- The CMake build system automatically detects the new directory structure
- If `src/`, `L1/`, or `mock/` directories don't exist, it falls back to the legacy top-level layout
- Existing tests continue to work without modification
- Test results format (XML) remains unchanged

### 2. **Clean Separation**
- **src/**: Contains all Essos implementation source files
- **L1/**: Contains all L1 test suite files (test_*.cpp)
- **mock/**: Contains mock headers, stub implementations, and test fixtures

### 3. **CMake-Controlled Build**
The `CMakeLists.txt` handles:
- Automatic directory detection
- GoogleTest integration via FetchContent
- Coverage instrumentation (optional, via `-DENABLE_COVERAGE=ON`)
- Stub/system library selection based on availability
- Consistent test result generation

## Migration Path

### Current State (Legacy)
All files in the top-level directory.

### Target State (New Architecture)
Files organized into `src/`, `L1/`, and `mock/` directories.

### Migration Steps

1. **Create directories** (if they don't exist):
   ```bash
   mkdir -p src L1 mock
   ```

2. **Move source files**:
   ```bash
   mv essos.cpp src/
   mv essos-sample.cpp src/
   # Move other implementation files to src/
   ```

3. **Move test files**:
   ```bash
   mv test_essos_l1.cpp L1/
   mv test_essos_l1_fixed.cpp L1/
   mv test_essos_l1_clean.cpp L1/
   # Move other test files to L1/
   ```

4. **Move mock/stub files**:
   ```bash
   mv essos_mock.h mock/
   mv wayland-stubs.h mock/
   mv westeros-gl-stubs.c mock/
   mv simpleshell-client-protocol.h mock/
   # Move other mock/stub files to mock/
   ```

5. **Keep headers accessible**:
   - Public headers (`essos.h`, `essos-app.h`, etc.) can remain in root or move to `src/`
   - CMake includes the root directory automatically

## Build Instructions

### Local Build (Linux)

```bash
# Configure with coverage
mkdir -p build && cd build
cmake -DENABLE_COVERAGE=ON ..

# Build
cmake --build . -- -j$(nproc)

# Run tests
ctest --output-on-failure

# Generate test XML
./essos_l1_tests --gtest_output=xml:test_results.xml

# Generate coverage report
lcov --directory . --capture --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.cleaned.info
genhtml coverage.cleaned.info --output-directory coverage_html
```

### Using Helper Script

```bash
# Linux/WSL
bash tools/ci_build.sh

# Windows
tools\ci_build.bat
```

## GitHub CI Integration

The `.github/workflows/ci.yml` workflow:

1. **Triggers**: Runs on push/PR to main, master, develop branches
2. **Build**: Compiles with coverage enabled
3. **Test**: Executes all tests via ctest
4. **Artifacts**: Uploads:
   - `test-results` - GoogleTest XML output
   - `coverage-html` - HTML coverage report

### CI Workflow Steps

1. Checkout code
2. Install dependencies (cmake, lcov, genhtml, pkg-config)
3. Configure CMake with `-DENABLE_COVERAGE=ON`
4. Build project
5. Run tests (ctest)
6. Generate test XML (GoogleTest format)
7. Generate coverage (lcov/genhtml)
8. Upload artifacts

## CMake Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_COVERAGE` | `OFF` | Enable code coverage instrumentation |
| `USE_SYSTEM_DEPENDENCIES` | `ON` | Use system libraries when available, else stubs |

### Example Configurations

```bash
# Minimal build (no coverage)
cmake ..

# With coverage
cmake -DENABLE_COVERAGE=ON ..

# Force use of stubs
cmake -DUSE_SYSTEM_DEPENDENCIES=OFF ..
```

## Test Execution

### Test Results
- Format: GoogleTest XML (`test_results.xml`)
- Location: `build/test_results.xml`
- Contains: Test counts, pass/fail status, execution times

### Coverage Reports
- Format: HTML (genhtml output)
- Location: `build/coverage_html/index.html`
- Contains: Line coverage, function coverage, branch coverage

## Advantages of New Architecture

1. **Clarity**: Clear separation between production code, tests, and mocks
2. **Maintainability**: Easier to locate and modify specific components
3. **Scalability**: Simple to add new tests or source files
4. **CI-Friendly**: Clean artifact generation for automated testing
5. **No Breaking Changes**: Fully backward compatible with legacy layout

## Troubleshooting

### Build fails with "No test sources found"
- Ensure `L1/` directory exists with test files, OR
- Test files exist in root directory for legacy fallback

### Coverage not generated
- Verify `ENABLE_COVERAGE=ON` was passed to cmake
- Check that `lcov` and `genhtml` are installed
- Ensure tests were executed before coverage collection

### Tests not found by ctest
- Run `cmake ..` again to regenerate build configuration
- Check that test executable was built: `ls build/essos_l1_tests`

## Future Enhancements

- [ ] Add benchmark tests in `L1/benchmarks/`
- [ ] Separate unit tests from integration tests
- [ ] Add code quality checks (cppcheck, clang-tidy)
- [ ] Generate Doxygen documentation from source comments
- [ ] Add performance regression testing

## References

- CMake Documentation: https://cmake.org/documentation/
- GoogleTest: https://github.com/google/googletest
- GitHub Actions: https://docs.github.com/actions
- lcov: http://ltp.sourceforge.net/coverage/lcov.php
