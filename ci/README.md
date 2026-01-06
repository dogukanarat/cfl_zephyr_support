# CI Scripts

This directory contains continuous integration and automation scripts for building, testing, and installing the library.

## Available Scripts

### 🔍 debug.sh - Debug Build and Test

Performs a clean debug build with tests enabled and runs the test suite.

**Usage:**
```bash
./ci/debug.sh
```

**What it does:**
1. Cleans the build directory completely
2. Configures project in Debug mode with tests enabled
3. Builds the project with debug symbols
4. Runs the complete test suite with verbose output

**Use cases:**
- Local development testing
- CI/CD debug builds
- Verifying code changes with tests

**Output:**
- Build artifacts in `build/`
- Test executable: `build/test/test_cfl_zephyr_support`
- Detailed test output with pass/fail status

---

### 🚀 release.sh - Release Build

Performs an optimized release build of the project.

**Usage:**
```bash
./ci/release.sh              # Incremental build
./ci/release.sh --clean      # Clean build from scratch
```

**Options:**
- `--clean` - Remove build directory before building (full rebuild)

**What it does:**
1. Optionally cleans build directory (with `--clean`)
2. Configures project in Release mode with optimizations
3. Builds the project with `-O3` optimization level
4. Creates optimized production-ready library

**Use cases:**
- Production builds
- Performance testing
- Release preparation
- CI/CD release pipelines

**Output:**
- Optimized library: `build/cflzephyrsupport_project.a`
- No debug symbols (smaller binary size)
- Maximum optimization enabled

---

### 📦 install.sh - Install Library

Builds and installs the library to the system or a custom location.

**Usage:**
```bash
# Install to /usr/local (requires sudo)
./ci/install.sh

# Install to user directory (~/.local)
./ci/install.sh --user

# Install to custom prefix
./ci/install.sh --prefix=/opt/mylibs

# Install debug build to custom location
./ci/install.sh --prefix=$HOME/libs --build-type=Debug

# Install without sudo (for custom prefixes)
./ci/install.sh --prefix=/tmp/test --no-sudo
```

**Options:**
- `--prefix=PATH` - Install to custom prefix (default: `/usr/local`)
- `--user` - Install to user directory (`~/.local`)
- `--no-sudo` - Don't use sudo for installation
- `--build-type=TYPE` - Build type: `Release` or `Debug` (default: `Release`)
- `--help` - Show help message

**What it does:**
1. Checks/creates build directory
2. Configures project with specified install prefix
3. Builds the project (if needed)
4. Installs library, headers, and CMake config files

**Installation locations:**
- **Default** (`/usr/local`):
  - Libraries: `/usr/local/lib/`
  - Headers: `/usr/local/include/cfl_zephyr_support/`
  - CMake config: `/usr/local/lib/cmake/cfl_zephyr_support/`

- **User install** (`~/.local`):
  - Libraries: `~/.local/lib/`
  - Headers: `~/.local/include/cfl_zephyr_support/`
  - CMake config: `~/.local/lib/cmake/cfl_zephyr_support/`

- **Custom prefix** (`/opt/mylibs`):
  - Libraries: `/opt/mylibs/lib/`
  - Headers: `/opt/mylibs/include/cfl_zephyr_support/`
  - CMake config: `/opt/mylibs/lib/cmake/cfl_zephyr_support/`

**Use cases:**
- System-wide installation
- User-local installation (no sudo required)
- Custom installation for testing
- CI/CD deployment

---

## Typical Workflows

### Local Development
```bash
# Make changes to code
vim src/cfl_zephyr_support.c

# Test changes
./ci/debug.sh

# If tests pass, create release build
./ci/release.sh --clean
```

### CI/CD Pipeline
```bash
# Debug build with tests (for pull requests)
./ci/debug.sh

# Release build (for releases)
./ci/release.sh --clean

# Install to staging area (for deployment testing)
./ci/install.sh --prefix=/tmp/staging --no-sudo
```

### Installing for Use in Other Projects
```bash
# System-wide (recommended for most users)
./ci/install.sh

# User-local (no sudo needed)
./ci/install.sh --user

# Custom location (for testing)
./ci/install.sh --prefix=$HOME/mylibs
```

### Using Installed Library

After installation, use in your CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.14)
project(MyProject)

# Find the installed library
find_package(CflZephyrSupport REQUIRED)

# Link your executable against it
add_executable(myapp main.c)
target_link_libraries(myapp PRIVATE CflZephyrSupport::CflZephyrSupport)
```

If installed to a custom prefix, set `CMAKE_PREFIX_PATH`:
```bash
cmake -DCMAKE_PREFIX_PATH=/opt/mylibs ..
```

---

## Script Features

All scripts include:
- ✅ **Error handling** - Exit immediately on failure
- ✅ **Colored output** - Easy to read status messages
- ✅ **Progress tracking** - Step-by-step execution feedback
- ✅ **Summary reports** - Clear success/failure summaries
- ✅ **Parallel builds** - Automatic detection of CPU cores
- ✅ **Path detection** - Automatic project root detection

---

## Environment Variables

The scripts respect standard environment variables:

- `CMAKE_BUILD_TYPE` - Override build type
- `CMAKE_INSTALL_PREFIX` - Override install prefix
- `CMAKE_PREFIX_PATH` - Additional search paths for dependencies

Example:
```bash
CMAKE_INSTALL_PREFIX=$HOME/.local ./ci/install.sh --no-sudo
```

---

## Troubleshooting

### debug.sh fails
- Check that all source files compile
- Verify tests are properly written
- Check test output for specific failures

### release.sh fails
- Check compiler warnings/errors
- Verify all dependencies are available
- Try `--clean` flag for fresh build

### install.sh fails
- **Permission denied**: Use `sudo` or `--user` flag
- **Directory not writable**: Use `--prefix` with writable location
- **Library conflicts**: Check if library already installed

### Tests fail in debug.sh
- Review test output for specific failures
- Check test assertions in `test/test_cfl_zephyr_support.c`
- Verify library functionality is implemented

---

## Integration with CI/CD Systems

### GitHub Actions Example
```yaml
- name: Run debug build and tests
  run: ./ci/debug.sh

- name: Build release
  run: ./ci/release.sh --clean

- name: Install to staging
  run: ./ci/install.sh --prefix=${{ github.workspace }}/install --no-sudo
```

### GitLab CI Example
```yaml
test:
  script:
    - ./ci/debug.sh

build:
  script:
    - ./ci/release.sh --clean

deploy:
  script:
    - ./ci/install.sh --prefix=/opt/libs
```

---

## Adding to Your Library

After migrating this scaffold to your library name, the scripts will automatically use your library name. The migration script updates:
- Build directory paths
- Library names in output messages
- Test executable names
- Installation paths

No manual updates to CI scripts needed!
