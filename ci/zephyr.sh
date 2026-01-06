#!/bin/bash
# ==============================================================================
# Zephyr Build Script
# ==============================================================================
# This script builds the project using Zephyr's toolchain and CMake integration.
#
# Usage:
#   ./ci/zephyr.sh --board=<board> [--build-dir=<dir>] [--clean]
#
# Examples:
#   ./ci/zephyr.sh --board=native_posix
#   ./ci/zephyr.sh --board=stm32f4_disco --clean
# ==============================================================================

set -e

# ==============================================================================
# Configuration
# ==============================================================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build-zephyr"
BOARD=""
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

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

error_exit() {
    print_error "$1"
    exit 1
}

show_usage() {
    echo "Usage: $0 --board=<board> [--build-dir=<dir>] [--clean]"
    echo ""
    echo "Options:"
    echo "  --board=NAME       Zephyr board name (required)"
    echo "  --build-dir=PATH   Build directory (default: $BUILD_DIR)"
    echo "  --clean            Clean build directory before building"
    echo "  --help             Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 --board=native_posix"
    echo "  $0 --board=stm32f4_disco --clean"
}

# ==============================================================================
# Parse Arguments
# ==============================================================================
for arg in "$@"; do
    case $arg in
        --board=*)
            BOARD="${arg#*=}"
            shift
            ;;
        --build-dir=*)
            BUILD_DIR="${arg#*=}"
            shift
            ;;
        --clean)
            CLEAN_BUILD=true
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

if [ -z "$BOARD" ]; then
    print_error "Missing required --board argument"
    show_usage
    exit 1
fi

# ==============================================================================
# Main Script
# ==============================================================================
print_header "Zephyr Build Script"

if ! command -v west >/dev/null 2>&1; then
    error_exit "west not found. Install Zephyr's west tool before running this script."
fi

if [ "$CLEAN_BUILD" = true ]; then
    print_info "Cleaning build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR" || error_exit "Failed to clean build directory"
fi

print_info "Building for board: $BOARD"
print_info "Build directory: $BUILD_DIR"

west build -b "$BOARD" -d "$BUILD_DIR" "$PROJECT_ROOT" \
    || error_exit "Zephyr build failed"

print_success "Zephyr build completed successfully"
