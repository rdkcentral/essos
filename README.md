# Essos

Essos is a library making it simple to create applications that run either as native EGL or Wayland clients. It is a companion to Westeros.

[![CI Build](https://github.com/yourusername/essos/workflows/essos-l1-ci/badge.svg)](https://github.com/yourusername/essos/actions)

## Quick Start

### Build and Test

```bash
# Using helper script (Linux/WSL)
bash tools/ci_build.sh

# Or manual build
mkdir build && cd build
cmake -DENABLE_COVERAGE=ON ..
cmake --build .
ctest --output-on-failure
```

### Windows Build

```powershell
# Using helper script
.\tools\ci_build.bat

# Or manual build
mkdir build
cd build
cmake ..
cmake --build . --config Release
ctest -C Release --output-on-failure
```

## Project Structure

```
essos/
├── src/                 # Essos implementation sources
├── L1/                  # L1 test suite
├── mock/                # Mock and stub files
├── docs/                # Documentation
├── scripts/             # Build and test scripts
├── config/              # Configuration files
├── tools/               # CI build helpers
└── .github/workflows/   # GitHub Actions CI
```

## Documentation

### 📚 Essential Guides
- **[Architecture Guide](docs/ARCHITECTURE.md)** - Complete architecture overview and design principles
- **[CI/Build Guide](docs/README_CI.md)** - Build system, CMake options, and CI integration
- **[Test Documentation](docs/README_TESTS.md)** - Testing guidelines and test suite details
- **[Contributing](docs/CONTRIBUTING.md)** - How to contribute to the project

### 📋 Reference Documentation
- **[Implementation Summary](docs/IMPLEMENTATION_SUMMARY.md)** - Complete implementation details
- **[Project Structure](docs/PROJECT_STRUCTURE.md)** - Detailed directory structure and organization
- **[Cleanup Summary](docs/CLEANUP_SUMMARY.md)** - Architecture cleanup and organization history

## Features

- ✅ Clean directory-based architecture
- ✅ CMake build system with auto-detection
- ✅ GoogleTest integration via FetchContent
- ✅ Code coverage support (lcov/genhtml)
- ✅ GitHub Actions CI with artifacts
- ✅ Backward compatibility with legacy layouts
- ✅ 100% test compatibility maintained

## Requirements

- CMake 3.14 or higher
- C++11 compatible compiler
- GoogleTest (automatically fetched)
- Optional: Wayland, EGL, GLES libraries

## CI Integration

The project includes GitHub Actions workflow that:
- Builds the project with coverage enabled
- Runs all L1 tests
- Generates test results (GoogleTest XML)
- Generates coverage reports (HTML)
- Uploads both as artifacts

## License

Copyright 2017 RDK Management

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

