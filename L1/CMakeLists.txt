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
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(essos_l1_tests PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(essos_l1_tests PRIVATE --coverage)
        find_program(GCOVR_EXECUTABLE gcovr)
        if(GCOVR_EXECUTABLE)
            execute_process(
                COMMAND "${GCOVR_EXECUTABLE}" --version
                OUTPUT_VARIABLE GCOVR_VERSION_OUTPUT
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET)
            if(NOT GCOVR_VERSION_OUTPUT MATCHES "([0-9]+\\.[0-9]+(\\.[0-9]+)?)")
                message(FATAL_ERROR "Unable to determine gcovr version from: ${GCOVR_VERSION_OUTPUT}")
            endif()
            set(GCOVR_VERSION "${CMAKE_MATCH_1}")
            if(GCOVR_VERSION VERSION_LESS "8.4")
                message(FATAL_ERROR "Essos scoped coverage requires gcovr 8.4 or newer; found ${GCOVR_VERSION_OUTPUT}. Install gcovr 8.4+ and reconfigure.")
            endif()
            if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14")
                message(FATAL_ERROR "Essos function-scoped coverage requires GCC 14 or newer; found ${CMAKE_CXX_COMPILER_VERSION}. Configure with GCC/G++ 14+ and rebuild.")
            endif()
            string(REGEX MATCH "^[0-9]+" GCOV_GCC_MAJOR "${CMAKE_CXX_COMPILER_VERSION}")
            find_program(GCOV_EXECUTABLE NAMES "gcov-${GCOV_GCC_MAJOR}" gcov)
            if(NOT GCOV_EXECUTABLE)
                message(FATAL_ERROR "A gcov executable matching GCC ${CMAKE_CXX_COMPILER_VERSION} is required to read coverage data.")
            endif()
            execute_process(
                COMMAND "${GCOV_EXECUTABLE}" --version
                OUTPUT_VARIABLE GCOV_VERSION_OUTPUT
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET)
            if(GCOV_VERSION_OUTPUT MATCHES "gcov[^0-9]*([0-9]+)")
                set(GCOV_VERSION_MAJOR "${CMAKE_MATCH_1}")
            else()
                message(FATAL_ERROR "Unable to determine gcov version from: ${GCOV_VERSION_OUTPUT}")
            endif()
            if(NOT "${GCOV_VERSION_MAJOR}" STREQUAL "${GCOV_GCC_MAJOR}")
                message(FATAL_ERROR "gcov ${GCOV_VERSION_OUTPUT} does not match GCC ${CMAKE_CXX_COMPILER_VERSION}; install/select the matching gcov executable.")
            endif()
            set(ESSOS_COVERAGE_FUNCTIONS
                "EssContextCreate|EssContextDestroy|EssContextGetLastErrorDetail|EssContextSupportWayland|EssContextSupportDirect|EssContextSetUseWayland|EssContextGetUseWayland|EssContextSetUseDirect|EssContextGetUseDirect|EssContextSetName|EssContextSetEGLConfigAttributes|EssContextGetEGLConfigAttributes|EssContextSetEGLSurfaceAttributes|EssContextGetEGLSurfaceAttributes|EssContextSetEGLContextAttributes|EssContextGetEGLContextAttributes|EssContextSetDisplayMode|EssContextSetInitialWindowSize|EssContextSetSwapInterval|EssContextInit|EssContextGetEGLDisplayType|EssContextGetAppPlatformDisplayType|EssContextSetKeyListener|EssContextSetKeyAndMetadataListener|EssContextSetPointerListener|EssContextSetTouchListener|EssContextSetSettingsListener|EssContextSetTerminateListener|EssContextSetGamepadConnectionListener|EssContextSetKeyRepeatInitialDelay|EssContextSetKeyRepeatPeriod|EssContextSetDisplaySize|EssContextGetDisplaySize|EssContextGetDisplaySafeArea|EssContextSetWindowPosition|EssContextSetWindowFocus|EssContextRunEventLoopOnce|essSetDisplaySize|essProcessKeyPressed|essProcessKeyReleased|essProcessKeyRepeat|essProcessPointerMotion|essProcessPointerButtonPressed|essProcessPointerButtonReleased|essProcessTouchDown|essProcessTouchUp|essProcessTouchMotion|essProcessTouchFrame|essProcessGamepadButtonPressed|essProcessGamepadButtonReleased|essProcessGamepadAxisChanged")
            add_custom_target(coverage-essos
                COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/coverage_html_essos"
                COMMAND "${GCOVR_EXECUTABLE}" --root "${ESSOS_SOURCE_ROOT}"
                    --gcov-executable "${GCOV_EXECUTABLE}"
                    --filter "${ESSOS_SOURCE_ROOT}/essos.cpp"
                    --exclude-function "/^(?!(${ESSOS_COVERAGE_FUNCTIONS})$).*/"
                    --lcov "${CMAKE_BINARY_DIR}/coverage_essos_filtered.info"
                    --html-details "${CMAKE_BINARY_DIR}/coverage_html_essos/index.html"
                    --print-summary --fail-under-line 75
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                DEPENDS essos_l1_tests
                VERBATIM
                COMMENT "Report coverage for the explicitly listed Essos API and event-dispatch scope (75% minimum)")
        else()
            message(WARNING "gcovr not found: coverage-essos target is unavailable")
        endif()
    else()
        message(WARNING "Coverage instrumentation is configured only for GCC")
    endif()
endif()
