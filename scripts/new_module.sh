#!/bin/bash

# new_module.sh - Create a new module skeleton (header + source)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

if [ $# -ne 1 ]; then
    print_error "Usage: $0 <module_name>"
    print_error "Example: $0 sensor_driver"
    exit 1
fi

MODULE_NAME="$1"

if [[ ! "$MODULE_NAME" =~ ^[a-zA-Z][a-zA-Z0-9_]*$ ]]; then
    print_error "Invalid module name. Use letters, numbers, and underscores only."
    exit 1
fi

HEADER_DIR="$PROJECT_ROOT/include/scaffold_project"
SOURCE_DIR="$PROJECT_ROOT/src"

HEADER_PATH="$HEADER_DIR/${MODULE_NAME}.h"
SOURCE_PATH="$SOURCE_DIR/${MODULE_NAME}.c"

if [ -f "$HEADER_PATH" ] || [ -f "$SOURCE_PATH" ]; then
    print_error "Module files already exist: ${MODULE_NAME}.h/.c"
    exit 1
fi

HEADER_GUARD="INC_$(echo "$MODULE_NAME" | tr '[:lower:]' '[:upper:]')_H"

print_info "Creating module: $MODULE_NAME"

mkdir -p "$HEADER_DIR" "$SOURCE_DIR"

cat > "$HEADER_PATH" <<EOF
/* ${MODULE_NAME}.h - one line definition */

/* All Rights Reserved */

#ifndef ${HEADER_GUARD}
#define ${HEADER_GUARD}

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes */

/* Configurations */

/* Definitions */

/* Types */

/* External Declarations */

#ifdef __cplusplus
}
#endif

#endif /* ${HEADER_GUARD} */
EOF

cat > "$SOURCE_PATH" <<EOF
/* ${MODULE_NAME}.c - one line definition */

/* All Rights Reserved */

/* Includes */

#include "scaffold_project/${MODULE_NAME}.h"

/* Imports */

/* Definitions */

/* Types */

/* Forward Declarations */

/* Variables */

/* Functions */
EOF

print_success "Created:"
print_success "  $HEADER_PATH"
print_success "  $SOURCE_PATH"
