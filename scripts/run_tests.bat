@echo off
REM Windows Batch Test Runner for Essos L1 Tests
REM Alternative to bash script for Windows development

setlocal EnableDelayedExpansion

REM Color codes for Windows
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

REM Function to print colored output (simulated with echo)
:print_status
echo [INFO] %~1
goto :eof

:print_success  
echo [SUCCESS] %~1
goto :eof

:print_warning
echo [WARNING] %~1
goto :eof

:print_error
echo [ERROR] %~1
goto :eof

REM Function to check if command exists
:command_exists
where %1 >nul 2>&1
goto :eof

REM Function to show usage
:show_usage
echo Usage: %0 [OPTION]
echo.
echo Options:
echo   install-deps    Install required dependencies (via vcpkg/chocolatey)
echo   build-cmake     Build using CMake
echo   build-make      Build using MinGW/MSYS2 Make
echo   test            Run the L1 tests
echo   clean           Clean build artifacts
echo   wsl             Run tests in WSL (recommended)
echo   help            Show this help message
echo.
echo Examples:
echo   %0 install-deps  # Install dependencies
echo   %0 wsl           # Run in WSL (recommended)
echo   %0 build-cmake   # Build with CMake
echo   %0 test          # Run tests
goto :eof

REM Function to install dependencies via package managers
:install_dependencies
call :print_status "Installing dependencies on Windows..."

REM Check for vcpkg
call :command_exists vcpkg
if !errorlevel! == 0 (
    call :print_status "Using vcpkg to install dependencies..."
    vcpkg install gtest gmock egl-headers
    call :print_success "Dependencies installed via vcpkg"
    goto :eof
)

REM Check for chocolatey
call :command_exists choco
if !errorlevel! == 0 (
    call :print_status "Using Chocolatey to install dependencies..."
    choco install mingw cmake -y
    call :print_success "Basic tools installed via Chocolatey"
    call :print_warning "You may need to manually install gtest/gmock"
    goto :eof
)

call :print_error "No supported package manager found (vcpkg/chocolatey)"
call :print_status "Please install manually or use WSL"
goto :eof

REM Function to detect project structure
:detect_structure
if exist "src" if exist "L1" if exist "mock" (
    call :print_status "Detected directory-based structure (src/, L1/, mock/)"
    set "USE_DIRS=1"
) else (
    call :print_status "Detected legacy top-level structure"
    set "USE_DIRS=0"
)
goto :eof

REM Function to build with CMake
:build_cmake
call :print_status "Building with CMake..."

REM Navigate to project root if we're in scripts directory
cd /d "%~dp0.."

call :detect_structure

call :command_exists cmake
if !errorlevel! neq 0 (
    call :print_error "CMake not found. Please install CMake first."
    exit /b 1
)

if not exist build mkdir build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
if !errorlevel! neq 0 (
    call :print_error "CMake configuration failed"
    cd ..
    exit /b 1
)

cmake --build . --parallel
if !errorlevel! neq 0 (
    call :print_error "Build failed"
    cd ..
    exit /b 1
)

cd ..
call :print_success "Build completed successfully"
goto :eof

REM Function to build with Make
:build_make
call :print_status "Building with MinGW Make..."

REM Navigate to project root if we're in scripts directory
cd /d "%~dp0.."

call :detect_structure

call :command_exists mingw32-make
if !errorlevel! == 0 (
    set "MAKE_CMD=mingw32-make"
) else (
    call :command_exists make
    if !errorlevel! == 0 (
        set "MAKE_CMD=make"
    ) else (
        call :print_error "Make not found. Please install MinGW or MSYS2"
        exit /b 1
    )
)

!MAKE_CMD! clean
!MAKE_CMD! -j4
if !errorlevel! neq 0 (
    call :print_error "Build failed"
    exit /b 1
)

call :print_success "Build completed successfully"
goto :eof

REM Function to run tests
:run_tests
call :print_status "Running L1 tests..."

REM Navigate to project root if we're in scripts directory
cd /d "%~dp0.."

REM Try different possible test executable locations
if exist build\test_essos_l1.exe (
    call :print_status "Running test executable: build\test_essos_l1.exe"
    build\test_essos_l1.exe --gtest_output=xml:test_results.xml --gtest_color=yes
) else if exist test_essos_l1.exe (
    call :print_status "Running test executable: test_essos_l1.exe"
    test_essos_l1.exe --gtest_output=xml:test_results.xml --gtest_color=yes
) else if exist build\essos_l1_tests.exe (
    call :print_status "Running test executable: build\essos_l1_tests.exe"
    build\essos_l1_tests.exe --gtest_output=xml:test_results.xml --gtest_color=yes
) else if exist essos_l1_tests.exe (
    call :print_status "Running test executable: essos_l1_tests.exe"
    essos_l1_tests.exe --gtest_output=xml:test_results.xml --gtest_color=yes
) else (
    call :print_error "Test executable not found. Please build first."
    call :print_status "Looked for: build\test_essos_l1.exe, test_essos_l1.exe, build\essos_l1_tests.exe, essos_l1_tests.exe"
    exit /b 1
)

if !errorlevel! == 0 (
    call :print_success "Tests completed successfully"
) else (
    call :print_error "Some tests failed"
    exit /b 1
)
goto :eof

REM Function to clean build artifacts
:clean_build
call :print_status "Cleaning build artifacts..."

REM Navigate to project root if we're in scripts directory
cd /d "%~dp0.."

REM Remove build directory
if exist build rmdir /s /q build

REM Remove object files from all directories
if exist *.obj del *.obj
if exist src\*.obj del src\*.obj
if exist L1\*.obj del L1\*.obj
if exist mock\*.obj del mock\*.obj

REM Remove executables
if exist *.exe del *.exe
if exist test_essos_l1.exe del test_essos_l1.exe
if exist essos_l1_tests.exe del essos_l1_tests.exe

REM Remove debug files
if exist *.pdb del *.pdb

REM Remove test results
if exist test_results.xml del test_results.xml

REM Remove coverage files
if exist *.gcno del *.gcno
if exist *.gcda del *.gcda
if exist src\*.gcno del src\*.gcno
if exist src\*.gcda del src\*.gcda
if exist L1\*.gcno del L1\*.gcno
if exist L1\*.gcda del L1\*.gcda
if exist mock\*.gcno del mock\*.gcno
if exist mock\*.gcda del mock\*.gcda

call :print_success "Build artifacts cleaned"
goto :eof

REM Function to run in WSL (recommended approach)
:run_wsl
call :print_status "Running tests in WSL (recommended)..."

call :command_exists wsl
if !errorlevel! neq 0 (
    call :print_error "WSL not found. Please install WSL first:"
    echo   wsl --install -d Ubuntu
    exit /b 1
)

REM Navigate to project root if we're in scripts directory
cd /d "%~dp0.."

REM Get current directory path and convert to WSL path
for %%I in (.) do set "PROJECT_DIR=%%~fI"
set "WSL_PATH=/mnt/d/Westeros/essos-topic-RDK-58448_1 1/essos-topic-RDK-58448_1"

call :print_status "Switching to WSL Ubuntu..."
call :print_status "Project path: %WSL_PATH%"

wsl bash -c "cd '%WSL_PATH%' && chmod +x scripts/run_tests.sh && ./scripts/run_tests.sh full"

if !errorlevel! == 0 (
    call :print_success "WSL tests completed successfully"
) else (
    call :print_error "WSL tests failed"
    exit /b 1
)
goto :eof

REM Main script logic
if "%~1"=="" goto show_usage
if "%~1"=="help" goto show_usage
if "%~1"=="--help" goto show_usage
if "%~1"=="-h" goto show_usage

if "%~1"=="install-deps" (
    call :install_dependencies
) else if "%~1"=="build-cmake" (
    call :build_cmake
) else if "%~1"=="build-make" (
    call :build_make  
) else if "%~1"=="test" (
    call :run_tests
) else if "%~1"=="clean" (
    call :clean_build
) else if "%~1"=="wsl" (
    call :run_wsl
) else (
    call :print_error "Unknown option: %~1"
    call :show_usage
    exit /b 1
)

endlocal