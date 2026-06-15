#!/bin/bash

#
# If not stated otherwise in this file or this component's license file the
# following copyright and licenses apply:
#
# Copyright 2020 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

################################################################################
# Build essos plugin for all variants
################################################################################


#######################################
# Essos Build Script
# Purpose: Build essos with L1 tests and/or Coverity analysis
# Usage: ./build.sh [OPTIONS]
#   Options:
#     --l1           : Build L1 tests with coverage
#     --coverity     : Build for Coverity analysis
#     --both         : Build both L1 and Coverity
#     --clean        : Clean build directories before building
#     --verbose      : Enable verbose output
#     --jobs N       : Number of parallel build jobs (default: nproc)
#     --help         : Show this help message
#######################################

set -e

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Default configuration
BUILD_L1=false
BUILD_COVERITY=false
CLEAN_BUILD=false
VERBOSE=false
JOBS=$(nproc 2>/dev/null || echo 4)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Build directories
L1_BUILD_DIR="$PROJECT_ROOT/L1/build"
COVERITY_BUILD_DIR="$PROJECT_ROOT/build-coverity"
AUTOTOOLS_BUILD_DIR="$PROJECT_ROOT"

# Log functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_step() {
    echo -e "${CYAN}[STEP]${NC} $1"
}

# Show help
show_help() {
    cat << EOF
Essos Build Script

Usage: $0 [OPTIONS]

Options:
    --l1               Build L1 tests with coverage
    --coverity         Build for Coverity analysis
    --both             Build both L1 and Coverity
    --clean            Clean build directories before building
    --verbose          Enable verbose output
    --jobs N           Number of parallel build jobs (default: $JOBS)
    --help             Show this help message

Examples:
    $0 --l1                     # Build L1 tests with coverage
    $0 --coverity               # Build for Coverity analysis
    $0 --both --clean           # Clean and build both
    $0 --l1 --jobs 8 --verbose  # Build L1 with 8 jobs and verbose output

EOF
}

# Parse command line arguments
parse_args() {
    if [ $# -eq 0 ]; then
        log_info "No arguments provided. Building both L1 and Coverity by default."
        BUILD_L1=true
        BUILD_COVERITY=true
        return
    fi
    
    while [ $# -gt 0 ]; do
        case $1 in
            --l1)
                BUILD_L1=true
                shift
                ;;
            --coverity)
                BUILD_COVERITY=true
                shift
                ;;
            --both)
                BUILD_L1=true
                BUILD_COVERITY=true
                shift
                ;;
            --clean)
                CLEAN_BUILD=true
                shift
                ;;
            --verbose)
                VERBOSE=true
                shift
                ;;
            --jobs)
                JOBS=$2
                shift 2
                ;;
            --help|-h)
                show_help
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
}

# Clean build directories
clean_builds() {
    log_step "Cleaning build directories..."
    
    if [ "$BUILD_L1" = true ] && [ -d "$L1_BUILD_DIR" ]; then
        log_info "Removing L1 build directory: $L1_BUILD_DIR"
        rm -rf "$L1_BUILD_DIR"
    fi
    
    if [ "$BUILD_COVERITY" = true ]; then
        if [ -d "$COVERITY_BUILD_DIR" ]; then
            log_info "Removing Coverity build directory: $COVERITY_BUILD_DIR"
            rm -rf "$COVERITY_BUILD_DIR"
        fi
        
        # Clean autotools artifacts
        if [ -f "$AUTOTOOLS_BUILD_DIR/Makefile" ]; then
            log_info "Cleaning autotools build artifacts"
            cd "$AUTOTOOLS_BUILD_DIR"
            make clean 2>/dev/null || true
            make distclean 2>/dev/null || true
        fi
    fi
    
    log_success "Build directories cleaned"
}

# Build L1 tests with coverage
build_l1() {
    log_step "Building L1 Tests with Coverage"
    
    # Check if L1 directory exists
    if [ ! -d "$PROJECT_ROOT/L1" ]; then
        log_error "L1 directory not found at $PROJECT_ROOT/L1"
        return 1
    fi
    
    # Create build directory
    mkdir -p "$L1_BUILD_DIR"
    cd "$L1_BUILD_DIR"
    
    # Configure with CMake
    log_info "Configuring L1 tests with CMake..."
    if [ "$VERBOSE" = true ]; then
        cmake -DCMAKE_BUILD_TYPE=Debug \
              -DENABLE_COVERAGE=ON \
              -DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
              -DCMAKE_C_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
              ..
    else
        cmake -DCMAKE_BUILD_TYPE=Debug \
              -DENABLE_COVERAGE=ON \
              -DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
              -DCMAKE_C_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
              .. > /dev/null
    fi
    
    # Build
    log_info "Building L1 tests (using $JOBS parallel jobs)..."
    if [ "$VERBOSE" = true ]; then
        make -j"$JOBS"
    else
        make -j"$JOBS" > /dev/null
    fi
    
    log_success "L1 tests built successfully"
    
    # Run tests
    log_info "Running L1 tests..."
    if [ -f "./essos_l1_tests" ]; then
        ./essos_l1_tests || {
            log_warning "Some tests failed, but continuing with coverage generation"
        }
    else
        log_error "Test executable not found: ./essos_l1_tests"
        return 1
    fi
    
    # Generate coverage report
    generate_coverage_report
    
    cd "$PROJECT_ROOT"
}

# Generate coverage report
generate_coverage_report() {
    log_step "Generating Coverage Report"
    
    cd "$L1_BUILD_DIR"
    
    # Create coverage directory
    mkdir -p coverage
    
    # Generate HTML coverage report with lcov
    if command -v lcov &> /dev/null && command -v genhtml &> /dev/null; then
        log_info "Generating HTML coverage report with lcov..."
        
        # Capture coverage data
        lcov --capture --directory . --output-file coverage/coverage.info \
             --rc lcov_branch_coverage=1 2>&1 | grep -v "WARNING: no data found" || true
        
        # Remove external dependencies from coverage
        lcov --remove coverage/coverage.info '/usr/*' '*/test/*' '*/mocks/*' \
             --output-file coverage/coverage_filtered.info \
             --rc lcov_branch_coverage=1 2>&1 | grep -v "WARNING: no data found" || true
        
        # Generate HTML report
        genhtml coverage/coverage_filtered.info \
                --output-directory coverage/html \
                --branch-coverage \
                --title "Essos L1 Test Coverage" 2>&1 | grep -v "WARNING: no data found" || true
        
        log_success "HTML coverage report generated: $L1_BUILD_DIR/coverage/html/index.html"
    fi
    
    # Generate text summary with gcovr
    if command -v gcovr &> /dev/null; then
        log_info "Generating coverage summary with gcovr..."
        
        gcovr --root .. \
              --exclude '.*/test/.*' \
              --exclude '.*/mocks/.*' \
              --print-summary \
              --txt coverage/coverage_summary.txt \
              --html-details coverage/coverage_gcovr.html \
              --branches 2>&1 | tee coverage/coverage_summary.txt
        
        log_success "Coverage summary generated: $L1_BUILD_DIR/coverage/coverage_summary.txt"
    else
        log_warning "gcovr not found. Skipping gcovr coverage report."
    fi
    
    # Display summary
    if [ -f "coverage/coverage_summary.txt" ]; then
        echo ""
        log_info "Coverage Summary:"
        cat coverage/coverage_summary.txt
        echo ""
    fi
}

# Build for Coverity analysis
build_coverity() {
    log_step "Building for Coverity Analysis"
    
    cd "$PROJECT_ROOT"
    
    # Check if configure script exists
    if [ ! -f "configure" ]; then
        log_info "Running autoreconf to generate configure script..."
        if [ "$VERBOSE" = true ]; then
            autoreconf --install --force
        else
            autoreconf --install --force > /dev/null 2>&1
        fi
    fi
    
    # Create build directory for Coverity
    mkdir -p "$COVERITY_BUILD_DIR"
    cd "$COVERITY_BUILD_DIR"
    
    # Configure with autotools
    log_info "Configuring with autotools for Coverity..."
    if [ "$VERBOSE" = true ]; then
        ../configure \
            --enable-essoswesterosfree \
            --enable-static \
            --disable-shared \
            CFLAGS="-g -O0" \
            CXXFLAGS="-g -O0"
    else
        ../configure \
            --enable-essoswesterosfree \
            --enable-static \
            --disable-shared \
            CFLAGS="-g -O0" \
            CXXFLAGS="-g -O0" > /dev/null 2>&1
    fi
    
    # Build
    log_info "Building for Coverity (using $JOBS parallel jobs)..."
    if [ "$VERBOSE" = true ]; then
        make -j"$JOBS"
    else
        make -j"$JOBS" > /dev/null
    fi
    
    log_success "Coverity build completed successfully"
    log_info "Build artifacts in: $COVERITY_BUILD_DIR"
    
    cd "$PROJECT_ROOT"
}

# Display build summary
show_summary() {
    echo ""
    log_info "========================================"
    log_info "           Build Summary"
    log_info "========================================"
    
    if [ "$BUILD_L1" = true ]; then
        echo -e "${GREEN}✓${NC} L1 Tests: Built and executed"
        if [ -d "$L1_BUILD_DIR/coverage" ]; then
            echo -e "  Coverage Report: $L1_BUILD_DIR/coverage/html/index.html"
        fi
    fi
    
    if [ "$BUILD_COVERITY" = true ]; then
        echo -e "${GREEN}✓${NC} Coverity Build: Completed"
        echo -e "  Build Directory: $COVERITY_BUILD_DIR"
    fi
    
    log_info "========================================"
    echo ""
}

# Main execution
main() {
    log_info "=== Essos Build Script ==="
    echo ""
    
    # Parse arguments
    parse_args "$@"
    
    # Display build configuration
    log_info "Build Configuration:"
    echo "  L1 Tests: $([ "$BUILD_L1" = true ] && echo "Enabled" || echo "Disabled")"
    echo "  Coverity: $([ "$BUILD_COVERITY" = true ] && echo "Enabled" || echo "Disabled")"
    echo "  Clean Build: $([ "$CLEAN_BUILD" = true ] && echo "Yes" || echo "No")"
    echo "  Parallel Jobs: $JOBS"
    echo "  Verbose: $([ "$VERBOSE" = true ] && echo "Yes" || echo "No")"
    echo ""
    
    # Clean if requested
    if [ "$CLEAN_BUILD" = true ]; then
        clean_builds
        echo ""
    fi
    
    # Build L1 tests
    if [ "$BUILD_L1" = true ]; then
        build_l1 || {
            log_error "L1 build failed"
            exit 1
        }
        echo ""
    fi
    
    # Build for Coverity
    if [ "$BUILD_COVERITY" = true ]; then
        build_coverity || {
            log_error "Coverity build failed"
            exit 1
        }
        echo ""
    fi
    
    # Show summary
    show_summary
    
    log_success "All builds completed successfully!"
}

# Execute main
main "$@"
