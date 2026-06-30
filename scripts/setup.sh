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
# Build essos  plugin for all variants
################################################################################

#######################################
# Essos Dependency Installer
# Purpose: Install all dependencies needed for L1 tests and Coverity analysis
# Usage: ./setup.sh
#######################################

set -e

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

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

# Detect OS
detect_os() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$ID
        VER=$VERSION_ID
        log_info "Detected OS: $OS $VER"
    else
        log_error "Cannot detect OS. /etc/os-release not found."
        exit 1
    fi
}

# Install dependencies for Ubuntu/Debian
install_ubuntu_debian() {
    log_info "Installing dependencies for Ubuntu/Debian..."
    
    sudo apt-get update
    
    # Build essentials
    sudo apt-get install -y \
        build-essential \
        cmake \
        pkg-config \
        git \
        autoconf \
        automake \
        libtool
    
    # Wayland dependencies
    sudo apt-get install -y \
        libwayland-dev \
        wayland-protocols \
        libwayland-egl1-mesa \
        libwayland-client0 \
        libwayland-server0
    
    # EGL/OpenGL dependencies
    sudo apt-get install -y \
        libgl1-mesa-dev \
        libegl1-mesa-dev \
        libgles2-mesa-dev \
        mesa-common-dev
    
    # XKB dependencies
    sudo apt-get install -y \
        libxkbcommon-dev \
        libxkbcommon-x11-dev
    
    # Test dependencies
    sudo apt-get install -y \
        libgtest-dev \
        libgmock-dev \
        lcov \
        gcovr
    
    # Additional utilities
    sudo apt-get install -y \
        curl \
        wget \
        unzip
    
    log_success "Ubuntu/Debian dependencies installed"
}

# Install dependencies for Fedora/CentOS/RHEL
install_fedora_rhel() {
    log_info "Installing dependencies for Fedora/CentOS/RHEL..."
    
    if command -v dnf &> /dev/null; then
        PKG_MGR="dnf"
    else
        PKG_MGR="yum"
    fi
    
    sudo $PKG_MGR update -y
    
    # Build essentials
    sudo $PKG_MGR install -y \
        gcc \
        gcc-c++ \
        cmake \
        pkg-config \
        git \
        autoconf \
        automake \
        libtool
    
    # Wayland dependencies
    sudo $PKG_MGR install -y \
        wayland-devel \
        wayland-protocols-devel \
        mesa-libwayland-egl-devel
    
    # EGL/OpenGL dependencies
    sudo $PKG_MGR install -y \
        mesa-libGL-devel \
        mesa-libEGL-devel \
        mesa-libGLES-devel
    
    # XKB dependencies
    sudo $PKG_MGR install -y \
        libxkbcommon-devel \
        libxkbcommon-x11-devel
    
    # Test dependencies
    sudo $PKG_MGR install -y \
        gtest-devel \
        gmock-devel \
        lcov \
        gcovr
    
    # Additional utilities
    sudo $PKG_MGR install -y \
        curl \
        wget \
        unzip
    
    log_success "Fedora/RHEL dependencies installed"
}

# Build and install Google Test (if needed)
install_gtest_from_source() {
    if pkg-config --exists gtest; then
        log_info "Google Test already installed via package manager"
        return 0
    fi
    
    log_info "Building Google Test from source..."
    
    GTEST_DIR="/tmp/googletest"
    rm -rf "$GTEST_DIR"
    
    git clone https://github.com/google/googletest.git "$GTEST_DIR"
    cd "$GTEST_DIR"
    
    mkdir -p build
    cd build
    cmake ..
    make -j$(nproc)
    sudo make install
    
    cd -
    rm -rf "$GTEST_DIR"
    
    log_success "Google Test built and installed from source"
}

# Verify installations
verify_dependencies() {
    log_info "Verifying dependencies..."
    
    local missing=()
    
    # Check build tools
    command -v gcc >/dev/null 2>&1 || missing+=("gcc")
    command -v g++ >/dev/null 2>&1 || missing+=("g++")
    command -v cmake >/dev/null 2>&1 || missing+=("cmake")
    command -v pkg-config >/dev/null 2>&1 || missing+=("pkg-config")
    command -v autoconf >/dev/null 2>&1 || missing+=("autoconf")
    command -v automake >/dev/null 2>&1 || missing+=("automake")
    command -v libtoolize >/dev/null 2>&1 || missing+=("libtool")

    
    # Check coverage tools
    command -v lcov >/dev/null 2>&1 || missing+=("lcov")
    command -v gcovr >/dev/null 2>&1 || missing+=("gcovr")
    
    # Check pkg-config packages
    pkg-config --exists wayland-client || missing+=("wayland-client")
    pkg-config --exists wayland-server || missing+=("wayland-server")
    pkg-config --exists wayland-egl || missing+=("wayland-egl")
    pkg-config --exists egl || missing+=("egl")
    pkg-config --exists glesv2 || missing+=("glesv2")
    pkg-config --exists xkbcommon || missing+=("xkbcommon")
    
    if [ ${#missing[@]} -eq 0 ]; then
        log_success "All dependencies verified successfully"
        return 0
    else
        log_warning "Missing dependencies: ${missing[*]}"
        return 1
    fi
}

# Display installed versions
show_versions() {
    log_info "Installed versions:"
    echo "  GCC: $(gcc --version | head -n1)"
    echo "  G++: $(g++ --version | head -n1)"
    echo "  CMake: $(cmake --version | head -n1)"
    echo "  Autoconf: $(autoconf --version | head -n1)"
    echo "  Automake: $(automake --version | head -n1)"
    echo "  Libtool: $(libtool --version | head -n1)"
    
    if command -v lcov &> /dev/null; then
        echo "  LCOV: $(lcov --version | head -n1)"
    fi
    
    if command -v gcovr &> /dev/null; then
        echo "  gcovr: $(gcovr --version | head -n1)"
    fi
}

# Main installation flow
main() {
    log_info "=== Essos Dependency Installer ==="
    log_info "This script installs dependencies for L1 tests and Coverity analysis"
    echo ""
    
    detect_os
    
    case $OS in
        ubuntu|debian)
            install_ubuntu_debian
            ;;
        fedora|centos|rhel)
            install_fedora_rhel
            ;;
        *)
            log_error "Unsupported OS: $OS"
            log_info "Please install dependencies manually"
            exit 1
            ;;
    esac
    
    # Try to build GTest from source if not available
    if ! verify_dependencies; then
        log_info "Attempting to install Google Test from source..."
        install_gtest_from_source
    fi
    
    # Final verification
    if verify_dependencies; then
        echo ""
        show_versions
        echo ""
        log_success "All dependencies installed successfully!"
        log_info "You can now run: ./build.sh --l1 or ./build.sh --coverity"
    else
        if [ -n "$ACT" ]; then
            log_warning "Some dependencies are still missing, but continuing in ACT environment..."
            log_info "This is expected when running locally with act tool."
            exit 0
        else
            log_error "Some dependencies are still missing. Please install them manually."
            exit 1
        fi
    fi
}

main "$@"
