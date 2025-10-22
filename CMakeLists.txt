# CMakeLists.txt for Essos L1 Tests
# Updated for clean directory structure: src/, L1/, mock/
cmake_minimum_required(VERSION 3.14)
project(EssosL1Tests VERSION 1.0.0 LANGUAGES C CXX)

# Project options
option(ENABLE_COVERAGE "Enable code coverage" ON)
option(USE_SYSTEM_DEPENDENCIES "Use system-installed dependencies" OFF)

# C++11 standard required
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Detect source directories
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/L1")
set(MOCK_DIR "${CMAKE_SOURCE_DIR}/mock")

# Check if directory-based layout exists
if(EXISTS "${SRC_DIR}" AND IS_DIRECTORY "${SRC_DIR}")
    message(STATUS "Using directory-based layout")
    message(STATUS "Source directory: ${SRC_DIR}")
    message(STATUS "Test directory: ${TEST_DIR}")
    message(STATUS "Mock directory: ${MOCK_DIR}")
    
    # Collect source files
    file(GLOB ESSOS_SOURCES
        "${SRC_DIR}/*.cpp"
        "${SRC_DIR}/*.c"
    )
    
    # Collect test files
    file(GLOB TEST_SOURCES
        "${TEST_DIR}/*.cpp"
    )
    
    # Exclude test_essos.cpp to avoid duplicate main() and test functions
    # Only use test_essos_l1.cpp which uses GoogleTest framework
    list(FILTER TEST_SOURCES EXCLUDE REGEX ".*test_essos\\.cpp$")
    
    # Collect mock sources
    file(GLOB MOCK_SOURCES
        "${MOCK_DIR}/*.cpp"
        "${MOCK_DIR}/*.c"
    )
    
    # Include directories
    include_directories(
        ${SRC_DIR}
        ${MOCK_DIR}
        ${MOCK_DIR}/..  # Allow #include <EGL/egl.h> to find mock/EGL/egl.h
    )
else()
    message(STATUS "Using legacy top-level layout")
    
    # Collect source files from top level
    set(ESSOS_SOURCES
        ${CMAKE_SOURCE_DIR}/essos.cpp
    )
    
    # Collect test files from top level
    file(GLOB TEST_SOURCES
        "${CMAKE_SOURCE_DIR}/test_essos_l1*.cpp"
    )
    
    # Exclude test_essos.cpp to avoid duplicate main() and test functions
    list(FILTER TEST_SOURCES EXCLUDE REGEX ".*test_essos\\.cpp$")
    
    # No separate mock directory in legacy layout
    set(MOCK_SOURCES "")
    
    # Include current directory
    include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/mock  # For EGL stub headers
    )
endif()

# Find required packages (optional dependencies)
find_package(PkgConfig)

if(PKG_CONFIG_FOUND AND USE_SYSTEM_DEPENDENCIES)
    pkg_check_modules(WAYLAND wayland-client wayland-egl)
    pkg_check_modules(EGL egl)
    pkg_check_modules(GLES glesv2)
    
    if(WAYLAND_FOUND)
        message(STATUS "Using system Wayland libraries")
        include_directories(${WAYLAND_INCLUDE_DIRS})
    endif()
    
    if(EGL_FOUND)
        message(STATUS "Using system EGL libraries")
        include_directories(${EGL_INCLUDE_DIRS})
    endif()
    
    if(GLES_FOUND)
        message(STATUS "Using system GLES libraries")
        include_directories(${GLES_INCLUDE_DIRS})
    endif()
else()
    message(STATUS "Using stub implementations (no system dependencies)")
endif()

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(googletest)

# Add definitions for build configuration
add_definitions(-DHAVE_WESTEROS=1)

# Filter out essos-sample.cpp from source files to avoid conflicts
list(FILTER ESSOS_SOURCES EXCLUDE REGEX ".*essos-sample\\.cpp$")

# Create test executable
add_executable(test_essos_l1
    ${TEST_SOURCES}
    ${ESSOS_SOURCES}
    ${MOCK_SOURCES}
)

# Link libraries
target_link_libraries(test_essos_l1
    GTest::gtest_main
    GTest::gmock_main
    pthread
    dl
)

# Link optional system libraries if found
if(WAYLAND_FOUND AND USE_SYSTEM_DEPENDENCIES)
    target_link_libraries(test_essos_l1 ${WAYLAND_LIBRARIES})
endif()

if(EGL_FOUND AND USE_SYSTEM_DEPENDENCIES)
    target_link_libraries(test_essos_l1 ${EGL_LIBRARIES})
endif()

if(GLES_FOUND AND USE_SYSTEM_DEPENDENCIES)
    target_link_libraries(test_essos_l1 ${GLES_LIBRARIES})
endif()

# Add coverage flags if enabled
if(ENABLE_COVERAGE)
    message(STATUS "Code coverage enabled")
    target_compile_options(test_essos_l1 PRIVATE --coverage)
    target_link_options(test_essos_l1 PRIVATE --coverage)
endif()

# Enable testing
enable_testing()
add_test(NAME EssosL1Tests COMMAND test_essos_l1)

# Add custom target for running tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_essos_l1
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running Essos L1 Tests"
)

# Print configuration summary
message(STATUS "")
message(STATUS "=== Configuration Summary ===")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Coverage: ${ENABLE_COVERAGE}")
message(STATUS "System Dependencies: ${USE_SYSTEM_DEPENDENCIES}")
message(STATUS "Source files: ${ESSOS_SOURCES}")
message(STATUS "Test files: ${TEST_SOURCES}")
message(STATUS "Mock files: ${MOCK_SOURCES}")
message(STATUS "============================")
message(STATUS "")