# Makefile for Essos L1 Tests
# Updated for clean directory structure: src/, L1/, mock/
# Alternative to CMake for simpler builds

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -O0 -fprofile-arcs -ftest-coverage
INCLUDES = -I. -Isrc -Imock -I/usr/include/gtest -I/usr/include/gmock
LIBS = -lgtest -lgtest_main -lgmock -lgmock_main -pthread -ldl

# Check for directory structure
SRC_DIR = src
L1_DIR = L1
MOCK_DIR = mock

# Detect if using directory-based layout
ifeq ($(wildcard $(SRC_DIR)/essos.cpp),$(SRC_DIR)/essos.cpp)
    $(info Using directory-based layout)
    USE_DIRS = 1
    # Source files from directories
    ESSOS_SOURCES = $(filter-out %/essos-sample.cpp, $(wildcard $(SRC_DIR)/*.cpp))
    # Exclude test_essos.cpp to avoid duplicate main() and test functions
    TEST_SOURCES = $(filter-out %/test_essos.cpp, $(wildcard $(L1_DIR)/*.cpp))
    MOCK_SOURCES = $(wildcard $(MOCK_DIR)/*.cpp) $(wildcard $(MOCK_DIR)/*.c)
else
    $(info Using legacy top-level layout)
    USE_DIRS = 0
    # Source files from top level
    ESSOS_SOURCES = essos.cpp
    # Exclude test_essos.cpp to avoid duplicate main() and test functions
    TEST_SOURCES = $(filter-out test_essos.cpp, $(wildcard test_essos_l1*.cpp))
    MOCK_SOURCES =
endif

# Check for Wayland support (optional)
WAYLAND_CFLAGS = $(shell pkg-config --cflags wayland-client wayland-egl 2>/dev/null)
WAYLAND_LIBS = $(shell pkg-config --libs wayland-client wayland-egl 2>/dev/null)

ifneq ($(WAYLAND_CFLAGS),)
	CXXFLAGS += $(WAYLAND_CFLAGS) -DHAVE_WAYLAND=1
	LIBS += $(WAYLAND_LIBS)
endif

# Check for EGL support (optional)
EGL_CFLAGS = $(shell pkg-config --cflags egl 2>/dev/null)
EGL_LIBS = $(shell pkg-config --libs egl 2>/dev/null)

ifneq ($(EGL_CFLAGS),)
	CXXFLAGS += $(EGL_CFLAGS)
	LIBS += $(EGL_LIBS)
endif

# Add Westeros support
CXXFLAGS += -DHAVE_WESTEROS=1

TARGET = test_essos_l1

# Object files
ALL_SOURCES = $(ESSOS_SOURCES) $(TEST_SOURCES) $(MOCK_SOURCES)
ALL_OBJECTS = $(ALL_SOURCES:.cpp=.o)
ALL_OBJECTS := $(ALL_OBJECTS:.c=.o)

# Default target
all: $(TARGET)

# Build the test executable
$(TARGET): $(ALL_OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
	@echo "Build complete: $(TARGET)"

# Compile C++ source files
%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile C source files
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run tests
test: $(TARGET)
	@echo "Running tests..."
	./$(TARGET)

# Run tests with verbose output
test-verbose: $(TARGET)
	@echo "Running tests with verbose output..."
	./$(TARGET) --gtest_output=xml:test_results.xml --gtest_print_time=1 --gtest_color=yes

# Generate coverage report (requires gcov and lcov)
coverage: test
	@echo "Generating coverage report..."
	@if command -v lcov >/dev/null 2>&1; then \
		lcov --capture --directory . --output-file coverage.info; \
		lcov --remove coverage.info '/usr/*' '*_deps/*' --output-file coverage.info; \
		genhtml coverage.info --output-directory coverage-html; \
		echo "Coverage HTML report generated in coverage-html/"; \
	else \
		gcov $(ALL_SOURCES); \
		echo "Coverage files generated: *.gcov"; \
		echo "Install lcov for HTML reports: sudo apt-get install lcov"; \
	fi

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -f $(ALL_OBJECTS) $(TARGET) test_results.xml
	rm -f src/*.o L1/*.o mock/*.o
	rm -f *.gcno *.gcda *.gcov
	rm -f src/*.gcno src/*.gcda L1/*.gcno L1/*.gcda mock/*.gcno mock/*.gcda
	@echo "Clean complete"

# Clean everything including coverage data
clean-all: clean
	@echo "Cleaning all generated files..."
	rm -f *.info
	rm -rf coverage-html
	@echo "Deep clean complete"

# Install dependencies (Ubuntu/Debian)
install-deps:
	@echo "Installing dependencies..."
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
	@echo "Dependencies installed"

# Build Google Test from source (if not available as package)
build-gtest:
	@echo "Building Google Test from source..."
	@if [ -d /usr/src/googletest ]; then \
		cd /usr/src/googletest && \
		sudo cmake . && \
		sudo cmake --build . && \
		sudo cp lib/*.a /usr/lib/ || sudo cp */lib*.a /usr/lib/; \
		echo "Google Test built successfully"; \
	else \
		echo "Google Test source not found at /usr/src/googletest"; \
		echo "Please install libgtest-dev first"; \
	fi

# Show project structure
structure:
	@echo "Project Structure:"
	@echo "Current layout: $(if $(USE_DIRS),Directory-based,Legacy top-level)"
	@if [ "$(USE_DIRS)" = "1" ]; then \
		echo "  src/     - $(words $(ESSOS_SOURCES)) source files"; \
		echo "  L1/      - $(words $(TEST_SOURCES)) test files"; \
		echo "  mock/    - $(words $(MOCK_SOURCES)) mock files"; \
	else \
		echo "  Top-level - $(words $(ESSOS_SOURCES)) source files"; \
		echo "  Top-level - $(words $(TEST_SOURCES)) test files"; \
	fi

# Help target
help:
	@echo "Essos L1 Test Suite - Makefile Help"
	@echo "===================================="
	@echo ""
	@echo "Available targets:"
	@echo "  all          - Build the test executable (default)"
	@echo "  test         - Run tests"
	@echo "  test-verbose - Run tests with verbose output and XML results"
	@echo "  coverage     - Generate coverage report (requires lcov)"
	@echo "  clean        - Clean build files"
	@echo "  clean-all    - Clean all files including coverage reports"
	@echo "  install-deps - Install required dependencies (Ubuntu/Debian)"
	@echo "  build-gtest  - Build Google Test from source"
	@echo "  structure    - Show current project structure"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Build Configuration:"
	@echo "  Layout:      $(if $(USE_DIRS),Directory-based (src/L1/mock),Legacy top-level)"
	@echo "  Target:      $(TARGET)"
	@echo "  Compiler:    $(CXX)"
	@echo "  C++ Std:     C++11"
	@echo "  Coverage:    Enabled (--coverage)"
	@echo ""
	@echo "Optional Dependencies:"
	@echo "  Wayland:     $(if $(WAYLAND_LIBS),Found,Not found - using stubs)"
	@echo "  EGL:         $(if $(EGL_LIBS),Found,Not found - using stubs)"

.PHONY: all test test-verbose coverage clean clean-all install-deps build-gtest structure help