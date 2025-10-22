#!/bin/bash
# Test Runner Script for Essos L1 Tests

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install dependencies
install_dependencies() {
    print_status "Installing dependencies..."
    
    if command_exists apt-get; then
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
        print_success "Dependencies installed successfully"
    elif command_exists yum; then
        sudo yum install -y \
            gcc-c++ \
            cmake \
            gtest-devel \
            gmock-devel \
            mesa-libEGL-devel \
            wayland-devel \
            pkgconfig \
            gcov
        print_success "Dependencies installed successfully"
    else
        print_error "Unsupported package manager. Please install dependencies manually."
        exit 1
    fi
}

# Function to detect project structure
detect_structure() {
    if [ -d "src" ] && [ -d "L1" ] && [ -d "mock" ]; then
        print_status "Detected directory-based structure (src/, L1/, mock/)"
        return 0
    else
        print_status "Detected legacy top-level structure"
        return 1
    fi
}

# Function to build tests
build_tests() {
    print_status "Building tests..."
    
    # Navigate to project root if we're in scripts directory
    if [ "$(basename "$PWD")" = "scripts" ]; then
        cd ..
    fi
    
    detect_structure
    
    if [ -f "CMakeLists.txt" ]; then
        # Use CMake if available
        print_status "Using CMake build system"
        mkdir -p build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
        make -j$(nproc)
        cd ..
        print_success "Tests built successfully using CMake"
    elif [ -f "Makefile" ]; then
        # Use Makefile
        print_status "Using Makefile build system"
        make clean
        make -j$(nproc)
        print_success "Tests built successfully using Makefile"
    else
        print_error "No build system found (CMakeLists.txt or Makefile)"
        exit 1
    fi
}

# Function to run tests
run_tests() {
    print_status "Running L1 tests..."
    
    # Navigate to project root if we're in scripts directory
    if [ "$(basename "$PWD")" = "scripts" ]; then
        cd ..
    fi
    
    # Try different possible test executable locations
    if [ -f "build/test_essos_l1" ]; then
        print_status "Running test executable: build/test_essos_l1"
        ./build/test_essos_l1 --gtest_output=xml:test_results.xml --gtest_color=yes
    elif [ -f "test_essos_l1" ]; then
        print_status "Running test executable: test_essos_l1"
        ./test_essos_l1 --gtest_output=xml:test_results.xml --gtest_color=yes
    elif [ -f "build/essos_l1_tests" ]; then
        print_status "Running test executable: build/essos_l1_tests"
        ./build/essos_l1_tests --gtest_output=xml:test_results.xml --gtest_color=yes
    elif [ -f "essos_l1_tests" ]; then
        print_status "Running test executable: essos_l1_tests"
        ./essos_l1_tests --gtest_output=xml:test_results.xml --gtest_color=yes
    else
        print_error "Test executable not found. Please build first."
        print_status "Looked for: build/test_essos_l1, test_essos_l1, build/essos_l1_tests, essos_l1_tests"
        exit 1
    fi
    
    if [ $? -eq 0 ]; then
        print_success "Tests completed successfully"
    else
        print_error "Some tests failed"
        exit 1
    fi
}

# Function to generate coverage report
generate_coverage() {
    print_status "Generating coverage report..."
    
    # Navigate to project root if we're in scripts directory
    if [ "$(basename "$PWD")" = "scripts" ]; then
        cd ..
    fi
    
    if command_exists gcov; then
        # Generate coverage for all source directories
        if [ -d "src" ]; then
            print_status "Generating coverage for src/ directory"
            gcov src/*.cpp 2>/dev/null || true
        fi
        if [ -d "L1" ]; then
            print_status "Generating coverage for L1/ directory"
            gcov L1/*.cpp 2>/dev/null || true
        fi
        # Also try legacy top-level
        gcov *.cpp 2>/dev/null || true
        
        if command_exists lcov; then
            lcov --capture --directory . --output-file coverage.info
            lcov --remove coverage.info '/usr/*' '*_deps/*' '*/googletest/*' --output-file coverage.info
            lcov --list coverage.info
            
            if command_exists genhtml; then
                genhtml coverage.info --output-directory coverage_html
                print_success "HTML coverage report generated in coverage_html/"
            fi
        fi
        print_success "Coverage report generated"
    else
        print_warning "gcov not available, skipping coverage report"
    fi
}

# Function to run static analysis
run_static_analysis() {
    print_status "Running static analysis..."
    
    # Navigate to project root if we're in scripts directory
    if [ "$(basename "$PWD")" = "scripts" ]; then
        cd ..
    fi
    
    if command_exists cppcheck; then
        # Analyze based on directory structure
        if [ -d "src" ] && [ -d "L1" ]; then
            print_status "Analyzing src/ and L1/ directories"
            cppcheck --enable=all --std=c++11 --suppress=missingIncludeSystem \
                -I src -I mock \
                src/ L1/ 2>&1 | tee static_analysis.log
        else
            print_status "Analyzing top-level directory"
            cppcheck --enable=all --std=c++11 --suppress=missingIncludeSystem . 2>&1 | tee static_analysis.log
        fi
        print_success "Static analysis completed (results in static_analysis.log)"
    else
        print_warning "cppcheck not available, skipping static analysis"
    fi
}

# Function to run memory leak detection
run_memory_check() {
    print_status "Running memory leak detection..."
    
    # Navigate to project root if we're in scripts directory
    if [ "$(basename "$PWD")" = "scripts" ]; then
        cd ..
    fi
    
    if command_exists valgrind; then
        # Find the test executable
        TEST_EXEC=""
        if [ -f "build/test_essos_l1" ]; then
            TEST_EXEC="./build/test_essos_l1"
        elif [ -f "test_essos_l1" ]; then
            TEST_EXEC="./test_essos_l1"
        elif [ -f "build/essos_l1_tests" ]; then
            TEST_EXEC="./build/essos_l1_tests"
        elif [ -f "essos_l1_tests" ]; then
            TEST_EXEC="./essos_l1_tests"
        else
            print_error "Test executable not found for memory check"
            return 1
        fi
        
        print_status "Running valgrind on: $TEST_EXEC"
        valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
            --xml=yes --xml-file=valgrind_report.xml \
            $TEST_EXEC
        
        print_success "Memory check completed (results in valgrind_report.xml)"
    else
        print_warning "valgrind not available, skipping memory check"
    fi
}

# Function to clean build artifacts
clean_build() {
    print_status "Cleaning build artifacts..."
    
    # Navigate to project root if we're in scripts directory
    if [ "$(basename "$PWD")" = "scripts" ]; then
        cd ..
    fi
    
    # Remove build directory
    rm -rf build/
    
    # Remove object files from all directories
    rm -f *.o src/*.o L1/*.o mock/*.o
    
    # Remove coverage files from all directories
    rm -f *.gcno *.gcda *.gcov
    rm -f src/*.gcno src/*.gcda L1/*.gcno L1/*.gcda mock/*.gcno mock/*.gcda
    rm -f coverage.info
    rm -rf coverage_html/
    
    # Remove test executables
    rm -f test_essos_l1 essos_l1_tests
    
    # Remove report files
    rm -f static_analysis.log valgrind_report.xml test_results.xml
    
    # Use Makefile clean if available
    if [ -f "Makefile" ]; then
        make clean 2>/dev/null || true
    fi
    
    print_success "Build artifacts cleaned"
}

# Function to show usage
show_usage() {
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  install-deps    Install required dependencies"
    echo "  build           Build the test executable"
    echo "  test            Run the L1 tests"
    echo "  coverage        Generate coverage report"
    echo "  static-analysis Run static code analysis"
    echo "  memory-check    Run memory leak detection"
    echo "  full            Run complete test suite (build + test + coverage + analysis)"
    echo "  clean           Clean build artifacts"
    echo "  help            Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 install-deps  # Install dependencies"
    echo "  $0 full          # Run complete test suite"
    echo "  $0 test          # Just run tests"
    echo "  $0 clean         # Clean up"
}

# Function to run full test suite
run_full_suite() {
    print_status "Starting complete L1 test suite..."
    
    # Navigate to project root if we're in scripts directory
    if [ "$(basename "$PWD")" = "scripts" ]; then
        cd ..
    fi
    
    # Display project structure
    detect_structure
    
    build_tests
    run_tests
    generate_coverage
    run_static_analysis
    run_memory_check
    
    print_success "Complete L1 test suite finished!"
    print_status "Generated reports:"
    echo "  - Test results: test_results.xml"
    echo "  - Coverage: coverage.info, coverage_html/"
    echo "  - Static analysis: static_analysis.log"
    echo "  - Memory check: valgrind_report.xml"
}

# Main script logic
main() {
    case "${1:-help}" in
        install-deps)
            install_dependencies
            ;;
        build)
            build_tests
            ;;
        test)
            run_tests
            ;;
        coverage)
            generate_coverage
            ;;
        static-analysis)
            run_static_analysis
            ;;
        memory-check)
            run_memory_check
            ;;
        full)
            run_full_suite
            ;;
        clean)
            clean_build
            ;;
        help|--help|-h)
            show_usage
            ;;
        *)
            print_error "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
}

# Check if script is being sourced or executed
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi