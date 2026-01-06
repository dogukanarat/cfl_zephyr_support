#!/bin/bash

# ============================================================================
# libscaffold Rename Script
# ============================================================================
# This script renames the scaffold library to a custom name while preserving
# case conventions and underscores throughout the codebase.
#
# Usage: ./rename.sh <new_library_name> [--dry-run]
#
# Examples:
#   ./rename.sh mylib          # Rename to mylib
#   ./rename.sh mylib --dry-run # Preview changes without applying
# ============================================================================

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
OLD_NAME="libscaffold"
DRY_RUN=false

# ============================================================================
# Helper Functions
# ============================================================================

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

# ============================================================================
# Case Conversion Functions
# ============================================================================

# Convert to lowercase
to_lower() {
    echo "$1" | tr '[:upper:]' '[:lower:]'
}

# Convert to uppercase
to_upper() {
    echo "$1" | tr '[:lower:]' '[:upper:]'
}

# Convert first character to uppercase (PascalCase)
to_pascal() {
    local str="$1"
    echo "$(echo "${str:0:1}" | tr '[:lower:]' '[:upper:]')${str:1}"
}

# Convert PascalCase to snake_case (e.g., MyNewLib -> my_new_lib)
to_snake_case() {
    local str="$1"
    # Insert underscore before uppercase letters (except the first one)
    # Then convert to lowercase
    echo "$str" | sed 's/\([A-Z]\)/_\1/g' | sed 's/^_//' | tr '[:upper:]' '[:lower:]'
}

# ============================================================================
# Validation
# ============================================================================

validate_name() {
    local name="$1"
    
    if [ -z "$name" ]; then
        print_error "Library name cannot be empty"
        return 1
    fi
    
    if [[ ! "$name" =~ ^[a-zA-Z][a-zA-Z0-9_]*$ ]]; then
        print_error "Invalid library name. Must start with a letter and contain only letters, numbers, and underscores"
        return 1
    fi
    
    return 0
}

# ============================================================================
# Main Rename Logic
# ============================================================================

rename_in_file() {
    local file="$1"
    local old_snake="$2"
    local new_snake="$3"
    local old_pascal="$4"
    local new_pascal="$5"
    local old_upper="$6"
    local new_upper="$7"
    local old_lower="$8"
    local new_lower="$9"

    if [ "$DRY_RUN" = true ]; then
        print_info "Would update content in: $file"
        return
    fi

    # Use sed to replace all occurrences while preserving case
    # Order matters: do more specific replacements first
    sed -i \
        -e "s/${old_snake}/${new_snake}/g" \
        -e "s/${old_pascal}/${new_pascal}/g" \
        -e "s/${old_upper}/${new_upper}/g" \
        -e "s/${old_lower}/${new_lower}/g" \
        "$file"
}

rename_file_or_dir() {
    local old_path="$1"
    local new_path="$2"
    
    if [ "$old_path" = "$new_path" ]; then
        return
    fi
    
    if [ "$DRY_RUN" = true ]; then
        print_info "Would rename: $old_path -> $new_path"
        return
    fi
    
    # Create parent directory if it doesn't exist
    mkdir -p "$(dirname "$new_path")"
    
    # Rename the file or directory
    mv "$old_path" "$new_path"
    print_success "Renamed: $old_path -> $new_path"
}

perform_rename() {
    local new_name="$1"

    # Generate all case variations
    local old_lower="scaffoldproject"
    local new_lower="$(to_lower "$new_name")"

    local old_snake="scaffold_project"
    local new_snake="$(to_snake_case "$new_name")"

    local old_pascal="ScaffoldProject"
    local new_pascal="$(to_pascal "$new_name")"

    local old_upper="SCAFFOLD_PROJECT"
    local new_upper="$(to_upper "$(to_snake_case "$new_name")")"

    print_info "Renaming scaffold from 'LibScaffold' to '$new_pascal'"
    print_info "Case variations:"
    print_info "  lowercase: $old_lower -> $new_lower (for directories, packages)"
    print_info "  snake_case: $old_snake -> $new_snake (for filenames)"
    print_info "  PascalCase: $old_pascal -> $new_pascal (for CMake project)"
    print_info "  UPPERCASE: $old_upper -> $new_upper (for header guards)"
    echo
    
    # Step 1: Update file contents
    print_info "Step 1: Updating file contents..."

    local files_to_update=(
        "CMakeLists.txt"
        "cmake/${old_pascal}Config.cmake.in"
        "cmake/${old_pascal}.pc.in"
        "include/${old_snake}/${old_snake}.h"
        "include/${old_snake}/${old_snake}_types.h"
        "src/${old_snake}.c"
        "src/${old_snake}_int.c"
        "README.md"
        "test/test_${old_snake}.c"
        "test/CMakeLists.txt"
        "example/example.c"
        "example/example_embedded.c"
        "example/CMakeLists.txt"
        "example/README.md"
        "CLAUDE.md"
        "ci/debug.sh"
        "ci/release.sh"
        "ci/install.sh"
        "ci/format-check.sh"
        "ci/clang-tidy.sh"
        "ci/zephyr.sh"
        "ci/README.md"
        "scripts/check_toolchain.sh"
        "scripts/new_module.sh"
        ".github/workflows/ci.yml"
        ".github/workflows/format-check.yml"
        ".github/workflows/static-analysis.yml"
    )

    for file in "${files_to_update[@]}"; do
        local full_path="$PROJECT_DIR/$file"
        if [ -f "$full_path" ]; then
            # Replace all variations: snake_case, PascalCase, UPPERCASE, lowercase
            rename_in_file "$full_path" "$old_snake" "$new_snake" "$old_pascal" "$new_pascal" "$old_upper" "$new_upper" "$old_lower" "$new_lower"
            print_success "Updated: $file"
        fi
    done
    
    echo
    print_info "Step 2: Renaming files and directories..."

    # Step 2: Rename files (must be done before renaming directories)
    # CMake config uses lowercase for package name
    if [ -f "$PROJECT_DIR/cmake/${old_pascal}Config.cmake.in" ]; then
        rename_file_or_dir \
            "$PROJECT_DIR/cmake/${old_pascal}Config.cmake.in" \
            "$PROJECT_DIR/cmake/${new_pascal}Config.cmake.in"
    fi

    if [ -f "$PROJECT_DIR/cmake/${old_pascal}.pc.in" ]; then
        rename_file_or_dir \
            "$PROJECT_DIR/cmake/${old_pascal}.pc.in" \
            "$PROJECT_DIR/cmake/${new_pascal}.pc.in"
    fi

    # Header files use snake_case
    if [ -f "$PROJECT_DIR/include/${old_snake}/${old_snake}.h" ]; then
        rename_file_or_dir \
            "$PROJECT_DIR/include/${old_snake}/${old_snake}.h" \
            "$PROJECT_DIR/include/${old_snake}/${new_snake}.h"
    fi

    if [ -f "$PROJECT_DIR/include/${old_snake}/${old_snake}_types.h" ]; then
        rename_file_or_dir \
            "$PROJECT_DIR/include/${old_snake}/${old_snake}_types.h" \
            "$PROJECT_DIR/include/${old_snake}/${new_snake}_types.h"
    fi

    # Source files use snake_case
    if [ -f "$PROJECT_DIR/src/${old_snake}.c" ]; then
        rename_file_or_dir \
            "$PROJECT_DIR/src/${old_snake}.c" \
            "$PROJECT_DIR/src/${new_snake}.c"
    fi

    if [ -f "$PROJECT_DIR/src/${old_snake}_int.c" ]; then
        rename_file_or_dir \
            "$PROJECT_DIR/src/${old_snake}_int.c" \
            "$PROJECT_DIR/src/${new_snake}_int.c"
    fi

    # Test files use snake_case with test_ prefix
    if [ -f "$PROJECT_DIR/test/test_${old_snake}.c" ]; then
        rename_file_or_dir \
            "$PROJECT_DIR/test/test_${old_snake}.c" \
            "$PROJECT_DIR/test/test_${new_snake}.c"
    fi

    # Step 3: Rename directories (directories use lowercase, not snake_case)
    if [ -d "$PROJECT_DIR/include/${old_snake}" ]; then
        rename_file_or_dir \
            "$PROJECT_DIR/include/${old_snake}" \
            "$PROJECT_DIR/include/${new_snake}"
    fi
    
    echo
    if [ "$DRY_RUN" = true ]; then
        print_warning "DRY RUN COMPLETE - No changes were made"
        print_info "Run without --dry-run to apply changes"
    else
        print_success "Rename complete!"
        print_info "Your library has been renamed from '$old_lower' to '$new_lower'"
    fi
}

# ============================================================================
# Main Script
# ============================================================================

main() {
    # Parse arguments
    if [ $# -eq 0 ]; then
        print_error "Usage: $0 <NewLibNamePascalCase> [--dry-run]"
        exit 1
    fi
    
    NEW_NAME="$1"
    
    if [ "$2" = "--dry-run" ]; then
        DRY_RUN=true
        print_warning "Running in DRY RUN mode - no changes will be made"
        echo
    fi
    
    # Validate the new name
    if ! validate_name "$NEW_NAME"; then
        exit 1
    fi
    
    # Confirm with user (unless dry run)
    if [ "$DRY_RUN" = false ]; then
        echo
        print_warning "This will rename the library from 'libscaffold' to '$NEW_NAME'"
        print_warning "This operation will modify files and directories in: $PROJECT_DIR"
        echo
        read -p "Are you sure you want to continue? (y/N) " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            print_info "Operation cancelled"
            exit 0
        fi
        echo
    fi
    
    # Perform the rename
    perform_rename "$NEW_NAME"
}

main "$@"
