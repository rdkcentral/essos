@echo off
REM CI Build Script for Windows
REM Builds project and runs tests

echo === Essos L1 CI Build ===

REM Clean and create build directory
if exist build rmdir /s /q build
mkdir build
cd build

REM Configure
echo Configuring CMake...
cmake ..

REM Build
echo Building project...
cmake --build . --config Release

REM Run tests
echo Running tests...
ctest -C Release --output-on-failure

echo === Build Complete ===
