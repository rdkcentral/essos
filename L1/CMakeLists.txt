cmake_minimum_required(VERSION 3.16)
project(essos_l1_tests LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_COVERAGE "Instrument Essos L1 tests for gcov/lcov" OFF)

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

    find_program(GCOVR_EXECUTABLE gcovr)
    if(NOT GCOVR_EXECUTABLE)
        message(FATAL_ERROR "ENABLE_COVERAGE requires gcovr")
    endif()

    target_compile_options(essos_l1_tests PRIVATE --coverage -O0 -g)
    target_link_options(essos_l1_tests PRIVATE --coverage)

    add_custom_target(coverage-essos
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/coverage_html"
        COMMAND "${GCOVR_EXECUTABLE}"
            --root "${ESSOS_SOURCE_ROOT}"
            --filter "${ESSOS_SOURCE_ROOT}/essos.cpp"
            --html-details "${CMAKE_BINARY_DIR}/coverage_html/index.html"
            --xml "${CMAKE_BINARY_DIR}/coverage.xml"
            --print-summary
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        DEPENDS essos_l1_tests
        COMMENT "Measure production-source coverage for essos.cpp"
        VERBATIM
    )
endif()
