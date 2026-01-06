#!/bin/bash

# check_toolchain.sh - Verify required tooling for the scaffold

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

check_tool() {
    local tool="$1"
    local desc="$2"

    if command -v "$tool" >/dev/null 2>&1; then
        print_success "$tool ($desc) found"
        return 0
    fi

    print_warning "$tool ($desc) not found"
    return 1
}

print_info "Checking required tools for: $PROJECT_ROOT"

missing=0

check_tool cmake "build system" || missing=1
check_tool make "build tool" || missing=1
check_tool clang-format "formatting" || missing=1
check_tool clang-tidy "static analysis" || missing=1

if [ "$missing" -ne 0 ]; then
    print_warning "Some tools are missing. Install them before running CI scripts."
else
    print_success "All required tools found"
fi

print_info "Optional tools:"
check_tool ninja "fast builds"
check_tool west "Zephyr build tool"
