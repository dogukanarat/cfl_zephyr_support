#!/bin/bash

# format.sh - Format all source and header files using clang-format

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

echo "Formatting source and header files..."

# Format all .c and .h files
find "$PROJECT_ROOT" -type f \( -name "*.c" -o -name "*.h" \) \
    -not -path "*/build/*" \
    -not -path "*/.git/*" \
    -exec clang-format -i {} \;

echo "Formatting complete!"
