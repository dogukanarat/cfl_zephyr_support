#!/bin/bash
# ==============================================================================
# Installation Script
# ==============================================================================
# This script builds and installs the library to the system.
# It can install to the default location (/usr/local) or a custom prefix.
#
# Usage:
#   ./ci/install.sh                    # Install to /usr/local (requires sudo)
#   ./ci/install.sh --prefix=/opt/lib  # Install to custom location
#   ./ci/install.sh --user             # Install to ~/.local
#
# Options:
#   --prefix=PATH    Install to custom prefix (default: /usr/local)
#   --user           Install to user directory (~/.local)
#   --no-sudo        Don't use sudo (useful for custom prefixes)
#   --build-type     Build type: Release or Debug (default: Release)
# ==============================================================================

set -e  # Exit immediately if a command exits with a non-zero status

# ==============================================================================
# Configuration
# ==============================================================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
INSTALL_PREFIX="/usr/local"
USE_SUDO=true
BUILD_TYPE="Release"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# ==============================================================================
# Helper Functions
# ==============================================================================
print_header() {
    echo ""
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}  $1${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""
}

print_info() {
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

# Error handler
error_exit() {
    print_error "$1"
    exit 1
}

show_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --prefix=PATH    Install to custom prefix (default: /usr/local)"
    echo "  --user           Install to user directory (~/.local)"
    echo "  --no-sudo        Don't use sudo"
    echo "  --build-type=TYPE Build type: Release or Debug (default: Release)"
    echo "  --help           Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                            # Install to /usr/local (requires sudo)"
    echo "  $0 --user                     # Install to ~/.local"
    echo "  $0 --prefix=/opt/mylib        # Install to /opt/mylib"
    echo "  $0 --build-type=Debug --user  # Install debug build to ~/.local"
}

# ==============================================================================
# Parse Arguments
# ==============================================================================
for arg in "$@"; do
    case $arg in
        --prefix=*)
            INSTALL_PREFIX="${arg#*=}"
            USE_SUDO=false
            shift
            ;;
        --user)
            INSTALL_PREFIX="$HOME/.local"
            USE_SUDO=false
            shift
            ;;
        --no-sudo)
            USE_SUDO=false
            shift
            ;;
        --build-type=*)
            BUILD_TYPE="${arg#*=}"
            shift
            ;;
        --help)
            show_usage
            exit 0
            ;;
        *)
            print_error "Unknown option: $arg"
            show_usage
            exit 1
            ;;
    esac
done

# Validate build type
if [[ "$BUILD_TYPE" != "Release" && "$BUILD_TYPE" != "Debug" ]]; then
    error_exit "Invalid build type: $BUILD_TYPE (must be Release or Debug)"
fi

# ==============================================================================
# Main Script
# ==============================================================================
print_header "Installation Script"

print_info "Installation configuration:"
print_info "  - Install prefix: $INSTALL_PREFIX"
print_info "  - Build type: $BUILD_TYPE"
print_info "  - Use sudo: $USE_SUDO"
echo ""

# Step 1: Check if build directory exists
print_info "Step 1/4: Checking build directory..."
if [ ! -d "$BUILD_DIR" ]; then
    print_warning "Build directory does not exist, building project first..."
    mkdir -p "$BUILD_DIR" || error_exit "Failed to create build directory"
else
    print_info "Build directory exists: $BUILD_DIR"
fi

cd "$BUILD_DIR" || error_exit "Failed to change to build directory"

# Step 2: Configure project with CMake
print_info "Step 2/4: Configuring project with CMake..."

cmake \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DBUILD_TESTS=OFF \
    "$PROJECT_ROOT" \
    || error_exit "CMake configuration failed"

print_success "Project configured successfully"

# Step 3: Build project
print_info "Step 3/4: Building project..."
print_info "Using $(nproc) parallel jobs"

cmake --build . --config $BUILD_TYPE --parallel $(nproc) \
    || error_exit "Build failed"

print_success "Project built successfully"

# Step 4: Install project
print_info "Step 4/4: Installing project..."

if [ "$USE_SUDO" = true ]; then
    print_warning "Installing to system directory, sudo password may be required..."
    sudo cmake --install . --config $BUILD_TYPE \
        || error_exit "Installation failed"
else
    cmake --install . --config $BUILD_TYPE \
        || error_exit "Installation failed"
fi

print_success "Project installed successfully"

# ==============================================================================
# Summary
# ==============================================================================
print_header "Installation Summary"
echo -e "${GREEN}✓${NC} Project configured ($BUILD_TYPE mode)"
echo -e "${GREEN}✓${NC} Project built successfully"
echo -e "${GREEN}✓${NC} Project installed to $INSTALL_PREFIX"
echo ""
print_success "Installation completed successfully!"
echo ""
print_info "Installation locations:"
print_info "  - Library: $INSTALL_PREFIX/lib/"
print_info "  - Headers: $INSTALL_PREFIX/include/cfl_zephyr_support/"
print_info "  - CMake config: $INSTALL_PREFIX/lib/cmake/cfl_zephyr_support/"
echo ""

# Add usage instructions
if [ "$INSTALL_PREFIX" = "$HOME/.local" ]; then
    print_info "To use the library, you may need to set:"
    echo "  export PKG_CONFIG_PATH=\$HOME/.local/lib/pkgconfig:\$PKG_CONFIG_PATH"
    echo "  export CMAKE_PREFIX_PATH=\$HOME/.local:\$CMAKE_PREFIX_PATH"
    echo ""
elif [ "$INSTALL_PREFIX" != "/usr/local" ] && [ "$INSTALL_PREFIX" != "/usr" ]; then
    print_info "To use the library, you may need to set:"
    echo "  export CMAKE_PREFIX_PATH=$INSTALL_PREFIX:\$CMAKE_PREFIX_PATH"
    echo ""
fi

print_info "To use in your CMakeLists.txt:"
echo "  find_package(cfl_zephyr_support REQUIRED)"
echo "  target_link_libraries(your_target PRIVATE cfl_zephyr_support::cfl_zephyr_support)"
echo ""
