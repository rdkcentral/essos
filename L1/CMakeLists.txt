# L1 Test Suite for Essos
# CMake configuration for L1 tests with mocks
# Target: 75%+ code coverage

cmake_minimum_required(VERSION 3.14)

project(EssosL1Tests
    VERSION 1.0.0
    DESCRIPTION "L1 Test Suite for Essos with Mocks"
    LANGUAGES C CXX
)

# C++ Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Options
option(ENABLE_COVERAGE "Enable code coverage" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Coverage configuration
if(ENABLE_COVERAGE)
    message(STATUS "Code coverage enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage -lgcov")
endif()

# Find dependencies
find_package(PkgConfig REQUIRED)

# GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Wayland (for real headers)
pkg_check_modules(WAYLAND REQUIRED wayland-client>=1.6.0)
pkg_check_modules(WAYLAND_EGL REQUIRED wayland-egl>=0.0.0)
pkg_check_modules(XKBCOMMON REQUIRED xkbcommon>=0.5.0)
pkg_check_modules(EGL REQUIRED egl>=0.0)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
    ${WAYLAND_INCLUDE_DIRS}
    ${WAYLAND_EGL_INCLUDE_DIRS}
    ${XKBCOMMON_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
)

# Mock source files
set(MOCK_SOURCES
    mocks/mock_wayland.cpp
    mocks/mock_egl.cpp
    mocks/mock_xkb.cpp
    mocks/mock_system.cpp
    mocks/mock_westeros_gl.cpp
)

# Mock library
add_library(essos_mocks STATIC ${MOCK_SOURCES})
target_include_directories(essos_mocks PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${WAYLAND_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
    ${XKBCOMMON_INCLUDE_DIRS}
)

# Essos source to test (using actual source with mocks)
set(ESSOS_TEST_SOURCES
    ../essos.cpp
)

# Test source files
set(TEST_SOURCES
    tests/L1_essos_context_tests.cpp
    tests/L1_essos_display_tests.cpp
    tests/L1_essos_input_tests.cpp
    tests/L1_essos_listener_tests.cpp
    tests/L1_essos_window_tests.cpp
    tests/L1_essos_integration_tests.cpp
    tests/L1_essos_coverage_tests.cpp
    tests/test_main.cpp
)

# Test executable
add_executable(essos_l1_tests
    ${TEST_SOURCES}
    ${ESSOS_TEST_SOURCES}
)

# Link libraries
target_link_libraries(essos_l1_tests
    essos_mocks
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    pthread
    dl
)

# Compile definitions
target_compile_definitions(essos_l1_tests PRIVATE
    HAVE_WAYLAND=1
    L1_TESTING=1
)

# Enable testing
enable_testing()

# Add test
add_test(NAME EssosL1Tests COMMAND essos_l1_tests)

# Coverage target
if(ENABLE_COVERAGE)
    add_custom_target(coverage
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage_html
        COMMAND lcov --capture --directory . --output-file coverage.info --rc lcov_branch_coverage=1
        COMMAND lcov --remove coverage.info '/usr/*' '*/test/*' '*/tests/*' '*/mocks/*' --output-file coverage_filtered.info --rc lcov_branch_coverage=1
        COMMAND genhtml coverage_filtered.info --output-directory coverage_html --title "Essos L1 Coverage Report" --legend --rc genhtml_branch_coverage=1
        COMMAND lcov --list coverage_filtered.info
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS essos_l1_tests
        COMMENT "Generating code coverage report"
    )
endif()

# Install test executable
install(TARGETS essos_l1_tests
    RUNTIME DESTINATION bin
)

# Print configuration summary
message(STATUS "========================================")
message(STATUS "Essos L1 Test Suite Configuration")
message(STATUS "========================================")
message(STATUS "Coverage: ${ENABLE_COVERAGE}")
message(STATUS "Mock sources: ${MOCK_SOURCES}")
message(STATUS "Test sources: ${TEST_SOURCES}")
message(STATUS "========================================")
