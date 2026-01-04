#!/bin/bash
# ==============================================================================
# Release Build Script
# ==============================================================================
# This script performs an optimized release build of the project.
# It's designed for CI/CD pipelines and creating production-ready builds.
#
# Usage: ./ci/release.sh [--clean]
#
# Options:
#   --clean    Clean build directory before building (optional)
# ==============================================================================

set -e  # Exit immediately if a command exits with a non-zero status

# ==============================================================================
# Configuration
# ==============================================================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
CLEAN_BUILD=false

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

# ==============================================================================
# Parse Arguments
# ==============================================================================
for arg in "$@"; do
    case $arg in
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        *)
            print_error "Unknown option: $arg"
            echo "Usage: $0 [--clean]"
            exit 1
            ;;
    esac
done

# ==============================================================================
# Main Script
# ==============================================================================
print_header "Release Build Script"

# Step 1: Handle build directory
if [ "$CLEAN_BUILD" = true ]; then
    print_info "Step 1/3: Cleaning build directory (--clean flag provided)..."
    if [ -d "$BUILD_DIR" ]; then
        print_info "Removing existing build directory: $BUILD_DIR"
        rm -rf "$BUILD_DIR" || error_exit "Failed to remove build directory"
        print_success "Build directory cleaned"
    else
        print_info "Build directory does not exist, will create fresh"
    fi
else
    print_info "Step 1/3: Using existing build directory (use --clean to rebuild from scratch)..."
    if [ -d "$BUILD_DIR" ]; then
        print_info "Build directory exists: $BUILD_DIR"
    else
        print_info "Build directory does not exist, will create fresh"
    fi
fi

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR" || error_exit "Failed to create build directory"
cd "$BUILD_DIR" || error_exit "Failed to change to build directory"

# Step 2: Configure project with CMake (Release mode)
print_info "Step 2/3: Configuring project with CMake..."
print_info "Build configuration:"
print_info "  - Build type: Release"
print_info "  - Optimizations: Enabled (-O3)"
print_info "  - Debug symbols: Disabled"
print_info "  - Build directory: $BUILD_DIR"

cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=OFF \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    "$PROJECT_ROOT" \
    || error_exit "CMake configuration failed"

print_success "Project configured successfully"

# Step 3: Build project
print_info "Step 3/3: Building project..."
print_info "Using $(nproc) parallel jobs"

cmake --build . --config Release --parallel $(nproc) \
    || error_exit "Build failed"

print_success "Project built successfully"

# ==============================================================================
# Summary
# ==============================================================================
print_header "Release Build Summary"
if [ "$CLEAN_BUILD" = true ]; then
    echo -e "${GREEN}✓${NC} Build directory cleaned"
fi
echo -e "${GREEN}✓${NC} Project configured (Release mode)"
echo -e "${GREEN}✓${NC} Project built successfully with optimizations"
echo ""
print_success "Release build completed successfully!"
echo ""
print_info "Build artifacts location: $BUILD_DIR"
print_info "Library: $BUILD_DIR/cflzephyrsupport_project.a"
echo ""
print_info "To install the library, run: ./ci/install.sh"
echo ""
