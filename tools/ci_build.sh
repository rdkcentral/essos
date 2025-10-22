#!/bin/bash
# CI Build Script for Linux/WSL
# Builds project with coverage and runs tests

set -e

echo "=== Essos L1 CI Build ==="

# Clean and create build directory
rm -rf build
mkdir build
cd build

# Configure with coverage
echo "Configuring CMake with coverage..."
cmake -DENABLE_COVERAGE=ON ..

# Build
echo "Building project..."
cmake --build .

# Run tests
echo "Running tests..."
ctest --output-on-failure

# Generate GoogleTest XML
echo "Generating test results XML..."
./test_essos_l1 --gtest_output=xml:test_results.xml || true

# Generate coverage report
if command -v lcov &> /dev/null && command -v genhtml &> /dev/null; then
    echo "Generating coverage report..."
    lcov --capture --directory . --output-file coverage.info
    lcov --remove coverage.info '/usr/*' '*/build/_deps/*' --output-file coverage.info
    genhtml coverage.info --output-directory coverage-html
    echo "Coverage report generated in build/coverage-html/"
else
    echo "Warning: lcov/genhtml not found, skipping coverage report"
fi

echo "=== Build Complete ==="
