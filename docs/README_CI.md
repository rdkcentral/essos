# Essos L1 Tests - CI/CD and Build Guide

## Quick Start

### Build Locally (Linux)
```bash
mkdir build && cd build
cmake -DENABLE_COVERAGE=ON ..
cmake --build . -- -j$(nproc)
ctest --output-on-failure
./essos_l1_tests --gtest_output=xml:test_results.xml
```

### Build Locally (Windows)
```cmd
mkdir build && cd build
cmake -DENABLE_COVERAGE=ON ..
cmake --build . -- /m
ctest --output-on-failure
```

### Use Helper Scripts
```bash
# Linux/WSL
bash tools/ci_build.sh

# Windows
tools\ci_build.bat
```

## Directory Layout

### Recommended Structure
```
├── src/      # Essos implementation sources (essos.cpp, etc.)
├── L1/       # L1 test suite files (test_*.cpp)
├── mock/     # Mock headers & source files used by tests
```

### Backward Compatibility
If `src/`, `L1/`, or `mock/` directories don't exist, CMake automatically falls back to using top-level sources and tests. This ensures existing workflows continue working without modification.

## CI Integration

### GitHub Actions Workflow
The `.github/workflows/ci.yml` workflow runs automatically on:
- Push to `main`, `master`, or `develop` branches
- Pull requests to these branches
- Manual trigger via workflow_dispatch

### What CI Does
1. **Build**: Compiles project with coverage enabled
2. **Test**: Runs all tests via ctest
3. **Coverage**: Generates HTML coverage report (lcov/genhtml)
4. **Artifacts**: Uploads test results and coverage reports

### CI Artifacts
- **test-results**: Contains `test_results.xml` (GoogleTest XML format)
- **coverage-html**: Contains HTML coverage report (viewable in browser)

## Test Results

### Format
- **File**: `build/test_results.xml`
- **Format**: GoogleTest XML
- **Contents**: Test counts, pass/fail status, execution times, failure messages

### Coverage
- **File**: `build/coverage_html/index.html`
- **Format**: HTML (genhtml)
- **Contents**: Line/function/branch coverage by file

## CMake Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_COVERAGE` | OFF | Enable coverage instrumentation (--coverage flags) |
| `USE_SYSTEM_DEPENDENCIES` | ON | Use system wayland/egl/gles if available, else stubs |

### Examples
```bash
# Basic build (no coverage)
cmake ..

# Build with coverage
cmake -DENABLE_COVERAGE=ON ..

# Force stub usage (no system dependencies)
cmake -DUSE_SYSTEM_DEPENDENCIES=OFF ..
```

## Ensuring Test Stability

### Test Results Unchanged
The new architecture preserves:
- Test executable behavior
- GoogleTest XML output format
- Test execution sequence
- Pass/fail results (100% pass maintained)

### Backward Compatibility Guarantees
- Existing tests run identically
- No changes to test logic required
- Legacy file layout still supported
- CI artifacts remain compatible with existing tooling

## Migration to New Layout

See `ARCHITECTURE.md` for detailed migration steps to move files into `src/`, `L1/`, and `mock/` directories.

Quick migration:
```bash
# Create directories
mkdir -p src L1 mock

# Move sources
mv essos.cpp essos-sample.cpp src/

# Move tests
mv test_essos_l1*.cpp L1/

# Move mocks/stubs
mv *-stubs.* *mock.h simpleshell-client-protocol.h mock/

# Rebuild
rm -rf build && mkdir build && cd build
cmake -DENABLE_COVERAGE=ON .. && cmake --build .
```

## Troubleshooting

### "No test sources found"
Ensure either:
- `L1/` directory exists with `test_*.cpp` files, OR
- Test files exist in root directory (legacy mode)

### Coverage not generated
- Verify `-DENABLE_COVERAGE=ON` was used
- Install `lcov` and `genhtml`: `sudo apt-get install lcov`
- Ensure tests ran before coverage collection

### CI workflow not triggering
- Check branch names match triggers (main/master/develop)
- Verify `.github/workflows/ci.yml` exists and is valid YAML
- Check GitHub Actions is enabled in repository settings

## Advanced Usage

### Custom Test Runs
```bash
# Run specific test pattern
./build/essos_l1_tests --gtest_filter="*Init*"

# Verbose output
./build/essos_l1_tests --gtest_verbose

# Repeat tests
./build/essos_l1_tests --gtest_repeat=10
```

### Coverage Filtering
```bash
# Exclude system headers and tests from coverage
lcov --directory . --capture --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/L1/*' --output-file coverage.filtered.info
genhtml coverage.filtered.info --output-directory coverage_html
```

## References

- Main architecture guide: `ARCHITECTURE.md`
- CMakeLists.txt: Build configuration
- CI workflow: `.github/workflows/ci.yml`
- Helper scripts: `tools/ci_build.sh` and `tools/ci_build.bat`
