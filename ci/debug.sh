#!/bin/bash
# ==============================================================================
# Debug Build and Test Script
# ==============================================================================
# This script performs a clean debug build with tests enabled and runs the
# test suite. It's designed for CI/CD pipelines and local development testing.
#
# Usage: ./ci/debug.sh
# ==============================================================================

set -e  # Exit immediately if a command exits with a non-zero status

# ==============================================================================
# Configuration
# ==============================================================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

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
# Main Script
# ==============================================================================
print_header "Debug Build and Test Script"

# Step 1: Clean build directory
print_info "Step 1/4: Cleaning build directory..."
if [ -d "$BUILD_DIR" ]; then
    print_info "Removing existing build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR" || error_exit "Failed to remove build directory"
    print_success "Build directory cleaned"
else
    print_info "Build directory does not exist, creating fresh"
fi

# Create build directory
mkdir -p "$BUILD_DIR" || error_exit "Failed to create build directory"
cd "$BUILD_DIR" || error_exit "Failed to change to build directory"

# Step 2: Configure project with CMake (Debug mode, with tests)
print_info "Step 2/4: Configuring project with CMake..."
print_info "Build configuration:"
print_info "  - Build type: Debug"
print_info "  - Tests: Enabled"
print_info "  - Build directory: $BUILD_DIR"

cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    "$PROJECT_ROOT" \
    || error_exit "CMake configuration failed"

print_success "Project configured successfully"

# Step 3: Build project
print_info "Step 3/4: Building project..."
print_info "Using $(nproc) parallel jobs"

cmake --build . --config Debug --parallel $(nproc) \
    || error_exit "Build failed"

print_success "Project built successfully"

# Step 4: Run tests
print_info "Step 4/4: Running test suite..."

# Run tests with CTest
ctest --output-on-failure --verbose \
    || error_exit "Tests failed"

print_success "All tests passed"

# ==============================================================================
# Summary
# ==============================================================================
print_header "Debug Build Summary"
echo -e "${GREEN}✓${NC} Build directory cleaned"
echo -e "${GREEN}✓${NC} Project configured (Debug mode)"
echo -e "${GREEN}✓${NC} Project built successfully"
echo -e "${GREEN}✓${NC} All tests passed"
echo ""
print_success "Debug build and test completed successfully!"
echo ""
print_info "Build artifacts location: $BUILD_DIR"
print_info "Test executable: $BUILD_DIR/test/test_cfl_zephyr_support"
echo ""
