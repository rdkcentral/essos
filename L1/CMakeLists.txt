cmake_minimum_required(VERSION 3.16)
project(essos_l1_tests LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_COVERAGE "Instrument Essos L1 tests for lcov" OFF)

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
find_path(EGL_INCLUDE_DIR EGL/egl.h)
if(NOT EGL_INCLUDE_DIR)
    message(FATAL_ERROR "EGL development headers are required to compile the Essos public API")
endif()

get_filename_component(ESSOS_SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

add_executable(essos_l1_tests
    tests/essos_l1_test.cpp
    tests/mocks/egl-mock.cpp
    ${ESSOS_SOURCE_ROOT}/essos.cpp)
target_include_directories(essos_l1_tests PRIVATE "${ESSOS_SOURCE_ROOT}" "${EGL_INCLUDE_DIR}")
target_compile_definitions(essos_l1_tests PRIVATE UNIT_TEST)
target_link_libraries(essos_l1_tests PRIVATE GTest::gtest_main Threads::Threads ${CMAKE_DL_LIBS})
target_compile_options(essos_l1_tests PRIVATE -Wall -Wextra -Wno-unused-parameter)

enable_testing()
include(GoogleTest)
gtest_discover_tests(essos_l1_tests PROPERTIES LABELS "Essos")

if(ENABLE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "ENABLE_COVERAGE requires GCC or Clang gcov-compatible instrumentation")
    endif()

    find_program(LCOV_EXECUTABLE lcov)
    find_program(GENHTML_EXECUTABLE genhtml)
    if(NOT LCOV_EXECUTABLE OR NOT GENHTML_EXECUTABLE)
        message(FATAL_ERROR "ENABLE_COVERAGE requires lcov and genhtml")
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        string(REGEX MATCH "^[0-9]+" GCOV_GCC_MAJOR "${CMAKE_CXX_COMPILER_VERSION}")
        find_program(GCOV_EXECUTABLE NAMES "gcov-${GCOV_GCC_MAJOR}")
        if(NOT GCOV_EXECUTABLE)
            message(FATAL_ERROR "A gcov executable matching GCC ${CMAKE_CXX_COMPILER_VERSION} is required for coverage")
        endif()

        execute_process(
            COMMAND "${GCOV_EXECUTABLE}" --version
            OUTPUT_VARIABLE GCOV_VERSION_OUTPUT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
        if(NOT GCOV_VERSION_OUTPUT MATCHES "gcov[^0-9]*${GCOV_GCC_MAJOR}([. ]|$)")
            message(FATAL_ERROR "${GCOV_EXECUTABLE} does not match GCC ${CMAKE_CXX_COMPILER_VERSION}; found ${GCOV_VERSION_OUTPUT}")
        endif()
    else()
        find_program(LLVM_COV_EXECUTABLE NAMES llvm-cov)
        if(NOT LLVM_COV_EXECUTABLE)
            message(FATAL_ERROR "ENABLE_COVERAGE with Clang requires llvm-cov")
        endif()
        set(GCOV_EXECUTABLE "${LLVM_COV_EXECUTABLE} gcov")
    endif()

    target_compile_options(essos_l1_tests PRIVATE --coverage -O0 -g)
    target_link_options(essos_l1_tests PRIVATE --coverage)

    add_custom_target(coverage-essos
        COMMAND "${LCOV_EXECUTABLE}" --directory "${CMAKE_BINARY_DIR}"
            --base-directory "${ESSOS_SOURCE_ROOT}"
            --capture
            --include "*/essos.cpp"
            --gcov-tool "${GCOV_EXECUTABLE}"
            --output-file "${CMAKE_BINARY_DIR}/coverage.info"
            --rc branch_coverage=1
        COMMAND "${GENHTML_EXECUTABLE}" "${CMAKE_BINARY_DIR}/coverage.info"
            --output-directory "${CMAKE_BINARY_DIR}/coverage_html"
            --title "Essos L1 Coverage"
            --legend
            --show-details
            --branch-coverage
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        DEPENDS essos_l1_tests
        COMMENT "Measure Essos production-source coverage with lcov"
        VERBATIM
    )
endif()
