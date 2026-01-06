#!/bin/bash
# ==============================================================================
# Format Check Script
# ==============================================================================
# This script checks code formatting using clang-format without modifying files.
#
# Usage: ./ci/format-check.sh
# ==============================================================================

set -e

# ==============================================================================
# Configuration
# ==============================================================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

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

# ==============================================================================
# Main Script
# ==============================================================================
print_header "Format Check Script"

print_info "Searching for C/C header files under: $PROJECT_ROOT"

FILES=$(find "$PROJECT_ROOT" -type f \( -name "*.c" -o -name "*.h" \) \
    -not -path "*/build/*" \
    -not -path "*/.git/*")

if [ -z "$FILES" ]; then
    print_info "No C/C header files found"
    exit 0
fi

print_info "Running clang-format checks..."

echo "$FILES" | xargs clang-format --dry-run --Werror \
    || error_exit "Formatting issues detected"

print_success "All files are properly formatted"
