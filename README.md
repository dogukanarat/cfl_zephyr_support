# cfl_zephyr_support

A minimal scaffold template for creating CMake-based C libraries with proper installation support.

## Overview

This is a ready-to-use scaffold for creating C libraries with CMake. It provides a clean starting point with organized file structure and comprehensive build configuration.

**Features:**
- ✅ Organized directory structure (`src/`, `include/`, `test/`)
- ✅ Comprehensive CMakeLists.txt with install commands
- ✅ Minimal template files with clear section organization
- ✅ CMake package configuration for `find_package()` support
- ✅ Intelligent rename script to customize the library name
- ✅ GitHub Actions CI/CD workflows (build matrix, format check, static analysis)
- ✅ CI automation scripts (`ci/debug.sh`, `ci/release.sh`, `ci/install.sh`)
- ✅ `.gitignore` configured for C/CMake projects
- ✅ `.clang-format` for consistent code formatting
- ✅ `.clang-tidy` for static analysis
- ✅ `.editorconfig` for editor consistency
- ✅ Unity test framework integration
- ✅ MIT License template

## Directory Structure

```
cfl_zephyr_support/
├── include/cfl_zephyr_support/            # Public headers
│   ├── cfl_zephyr_support.h               # Main API header
│   └── cfl_zephyr_support_types.h         # Common types and definitions
├── src/                            # Implementation
│   ├── cfl_zephyr_support.c               # Core implementation
│   └── cfl_zephyr_support_int.c           # Internal utilities
├── test/                           # Unit tests (Unity framework)
│   ├── CMakeLists.txt              # Test build configuration
│   ├── test_cfl_zephyr_support.c          # Example test file
│   └── README.md                   # Testing guide
├── cmake/                          # CMake modules
│   └── cfl_zephyr_supportConfig.cmake.in  # Package config template
├── ci/                             # CI/CD automation scripts
│   ├── debug.sh                    # Debug build + tests
│   ├── release.sh                  # Release build
│   ├── install.sh                  # Installation script
│   └── README.md                   # CI scripts guide
├── scripts/                        # Utility scripts
│   ├── migration.sh                # Library renaming script
│   └── format.sh                   # Code formatting script
├── docs/                           # Documentation
│   └── FORMATTING.md               # Code style guide
├── .github/                        # GitHub Actions workflows
│   └── workflows/
│       ├── ci.yml                  # Main CI with matrix testing
│       ├── format-check.yml        # Code formatting validation
│       └── static-analysis.yml     # clang-tidy analysis
├── .clang-format                   # Code formatting rules
├── .clang-tidy                     # Static analysis config
├── .editorconfig                   # Editor configuration
├── .gitignore                      # Git ignore rules
├── CMakeLists.txt                  # Build configuration
├── CLAUDE.md                       # AI assistant guide
├── LICENSE                         # MIT License
└── README.md                       # This file
```

## File Structure

All source and header files follow a consistent organization:

**Header Files** (`.h`):
```c
/* filename.h - one line definition */

/* All Rights Reserved */

#ifndef INC_FILENAME_H
#define INC_FILENAME_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */

/* Configurations */

/* Definitions */

/* Types */

/* External Declarations */

#ifdef __cplusplus
}
#endif

#endif /* INC_FILENAME_H */
```

**Source Files** (`.c`):
```c
/* filename.c - one line definition */

/* All Rights Reserved */

/* Includes */

/* Imports */

/* Definitions */

/* Types */

/* Forward Declarations */

/* Variables */

/* Functions */
```

## Code Formatting

The scaffold includes a `.clang-format` configuration file that enforces consistent code style:

**Coding Style Rules:**
1. Functions and variables: `camelCase`
2. Defines: `UPPER_CASE_WITH_UNDERSCORE`
3. Global variables: `PascalCase`
4. Braces: Always on new lines (Allman style)
5. Pointers: Close to name (`int *ptr`)
6. Long function signatures: Parameters on separate lines
7. Short function signatures: Single line

**Format All Files:**
```bash
./format.sh
```

**Format Specific Files:**
```bash
clang-format -i src/myfile.c
clang-format -i include/cfl_zephyr_support/myheader.h
```

**Example Formatted Code:**
```c
void shortFunction(int param)
{
    int *localVar = NULL;
}

status_t longFunction(
    int32_t handleId,
    const credentials_t *credential,
    state_t *state,
    uint32_t timeout)
{
    if (credential == NULL)
    {
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}
```

## Building

### Basic Build

```bash
mkdir build
cd build
cmake ..
make
```

The build system automatically defaults to Release mode and provides a configuration summary.

### Build Options

```bash
# Build shared library instead of static
cmake -DBUILD_SHARED_LIBS=ON ..

# Enable tests with Unity framework
cmake -DBUILD_TESTS=ON ..

# Enable examples
cmake -DBUILD_EXAMPLES=ON ..

# Specify build type explicitly
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Force download Unity via FetchContent (even if installed locally)
cmake -DBUILD_TESTS=ON -DFORCE_FETCH_UNITY=ON ..
```

### Build Types

- `Release` (default): Optimized, no debug info
- `Debug`: Debug symbols, no optimization
- `RelWithDebInfo`: Optimized with debug info
- `MinSizeRel`: Optimized for size

## Testing

The scaffold includes [Unity Test Framework](https://github.com/ThrowTheSwitch/Unity) integration with intelligent dependency resolution:

**Automatic Unity resolution:**
1. First tries to find Unity installed locally on your system
2. If not found, automatically downloads it via CMake FetchContent
3. No manual Unity installation required!

### Build and Run Tests

```bash
mkdir build
cd build
cmake -DBUILD_TESTS=ON ..
make
ctest
```

### Run Tests with Verbose Output

```bash
ctest --verbose
```

### Run Specific Test Labels

```bash
# Run only unit tests
ctest -L unit

# Run only cfl_zephyr_support tests
ctest -L cfl_zephyr_support
```

### Writing Tests

See [test/README.md](file:///home/dogukanarat/workspace/cfl_zephyr_support/test/README.md) for a comprehensive guide on writing tests with Unity.

**Quick Example:**
```c
#include "cfl_zephyr_support/cfl_zephyr_support.h"
#include "unity.h"

void test_myFunction_should_returnSuccess(void)
{
    int result = myFunction();
    TEST_ASSERT_EQUAL(0, result);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_myFunction_should_returnSuccess);
    return UNITY_END();
}
```

## Installation

### System-Wide Installation

```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

This will install:
- Library files to `/usr/local/lib/`
- Header files to `/usr/local/include/cfl_zephyr_support/`
- CMake config files to `/usr/local/lib/cmake/cfl_zephyr_support/`

### Custom Installation Prefix

```bash
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install ..
make
make install
```

## Using the Installed Library

### In Your CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.14)
project(MyProject)

# Find the installed library
find_package(CflZephyrSupport REQUIRED)

# Create your executable
add_executable(myapp main.c)

# Link against the library
target_link_libraries(myapp PRIVATE CflZephyrSupport::CflZephyrSupport)
```

### In Your C Code

```c
#include <cfl_zephyr_support/cfl_zephyr_support.h>

int main(void) {
    /* Use the library */
    return 0;
}
```

## Renaming the Scaffold

To customize this scaffold for your own library, use the provided rename script. **Provide the library name in PascalCase** (e.g., `MyHttpClient`, `JsonParser`).

### Preview Changes (Dry Run)

```bash
./scripts/migration.sh MyHttpClient --dry-run
```

This will show you all the changes that would be made without actually modifying any files.

### Apply Rename

```bash
./scripts/migration.sh MyHttpClient
```

The script intelligently generates four case variations and applies them in the right contexts:
- ✅ **snake_case** for filenames: `my_http_client.c`, `my_http_client.h`, `my_http_client_types.h`
- ✅ **lowercase** for directories and packages: `include/myhttpclient/`, `myhttpclientConfig.cmake`
- ✅ **PascalCase** for CMake project name: `project(MyHttpClient)`
- ✅ **UPPERCASE** for header guards: `#ifndef INC_MY_HTTP_CLIENT_H`
- ✅ Renames all files and directories throughout the project
- ✅ Updates all file contents (CMakeLists.txt, headers, sources, README, tests)

### After Renaming

1. Review the changes
2. Update the one-line descriptions in each file
3. Update the license information ("All Rights Reserved")
4. Implement your actual library functionality
5. Add tests and examples as needed

## CMake Installation Features

The scaffold includes complete installation support:

- **Library Installation**: Installs both static and shared libraries
- **Header Installation**: Preserves directory structure
- **CMake Package Config**: Enables `find_package()` support
- **Version Management**: Automatic version file generation
- **Component Support**: Separate Runtime and Development components

## Compiler Support

- **Standard**: C99
- **GCC/Clang**: Full warnings enabled (`-Wall -Wextra -Wpedantic`)
- **MSVC**: Warning level 4 (`/W4`)
- **Symbol visibility**: Controlled for shared libraries

## CI/CD Scripts

The scaffold includes automation scripts for common development tasks:

### Quick Start
```bash
# Debug build with tests
./ci/debug.sh

# Release build
./ci/release.sh

# Install to system
./ci/install.sh
```

### Available Scripts

**`./ci/debug.sh`** - Debug Build & Test
- Performs clean build in Debug mode
- Runs complete test suite
- Use for: development, CI/CD testing, verifying changes

**`./ci/release.sh`** - Release Build
- Builds optimized production library
- Options: `--clean` for fresh build
- Use for: releases, performance testing

**`./ci/install.sh`** - Installation
- Installs library to system or custom location
- Options:
  - `--user` - Install to `~/.local` (no sudo)
  - `--prefix=/path` - Custom install location
  - `--build-type=Debug` - Install debug build
- Use for: deployment, system setup

See [ci/README.md](ci/README.md) for detailed documentation.

## GitHub Actions

The scaffold includes comprehensive GitHub Actions workflows for automated CI/CD:

### Main CI Workflow (`.github/workflows/ci.yml`)

**Automated testing with matrix strategy:**
- Compilers: GCC and Clang
- Library types: Static and Shared
- Total: 4 configurations tested in parallel

**Triggers:**
- Every push to `master`, `main`, or `develop` branches
- Every pull request to `master` or `main`
- Manual trigger via workflow_dispatch

**What it does:**
1. Installs dependencies (CMake, Clang, build tools)
2. Configures and builds the project
3. Runs all tests via CTest
4. Uploads test results and build artifacts

**Build artifacts:** Compiled libraries and test executables for each configuration

### Code Quality Workflows

**Format Check (`.github/workflows/format-check.yml`)**
- Validates code formatting with clang-format
- Runs on every push and pull request
- Fails if code doesn't match `.clang-format` style
- Fix issues with: `./scripts/format.sh`

**Static Analysis (`.github/workflows/static-analysis.yml`)**
- Runs clang-tidy static analysis
- Checks for bugs, performance issues, code quality
- Uses `.clang-tidy` configuration (13 check categories)
- All warnings treated as errors

### Adding CI Badges to Your README

After pushing to GitHub, add these badges to your README:

```markdown
[![CI](https://github.com/username/repo/workflows/CI/badge.svg)](https://github.com/username/repo/actions/workflows/ci.yml)
[![Format Check](https://github.com/username/repo/workflows/Format%20Check/badge.svg)](https://github.com/username/repo/actions/workflows/format-check.yml)
[![Static Analysis](https://github.com/username/repo/workflows/Static%20Analysis/badge.svg)](https://github.com/username/repo/actions/workflows/static-analysis.yml)
```

## Customization

After renaming, you can customize:

1. **Add more source files**: Update `CMakeLists.txt` `target_sources()`
2. **Add dependencies**: Use `find_package()` and `target_link_libraries()`
3. **Add compile definitions**: Use `target_compile_definitions()`
4. **Add tests**: Create a `test/` directory and enable `BUILD_TESTS`
5. **Add examples**: Create an `example/` directory and enable `BUILD_EXAMPLES`

## License

Update the license information in all source files and add a LICENSE file as needed.

## Repository

GitHub: [dogukanarat/cfl_zephyr_support](https://github.com/dogukanarat/cfl_zephyr_support)
