# ==============================================================================
# Test Suite Configuration
# ==============================================================================
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# ==============================================================================
# Unity Test Framework Dependency
# ==============================================================================
# Try to find Unity framework locally first (if installed on the system)
# If not found, download it using FetchContent

# Option to force using FetchContent even if Unity is installed locally
option(FORCE_FETCH_UNITY "Force download Unity via FetchContent" OFF)

# Variable to track if Unity was found
set(UNITY_FOUND FALSE)

# Try to find Unity locally first (unless FORCE_FETCH_UNITY is ON)
if(NOT FORCE_FETCH_UNITY)
    message(STATUS "Looking for Unity test framework...")

    # Try pkg-config first (most reliable on Linux)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(UNITY QUIET unity)
    endif()

    # If pkg-config didn't find it, try CMake's find_package
    if(NOT UNITY_FOUND)
        find_package(Unity QUIET)
    endif()

    # If still not found, look for Unity in common locations
    if(NOT UNITY_FOUND AND NOT Unity_FOUND)
        find_path(UNITY_INCLUDE_DIR
            NAMES unity.h
            PATHS
                /usr/include
                /usr/local/include
                /opt/local/include
                ${CMAKE_PREFIX_PATH}/include
        )

        find_library(UNITY_LIBRARY
            NAMES unity
            PATHS
                /usr/lib
                /usr/local/lib
                /opt/local/lib
                ${CMAKE_PREFIX_PATH}/lib
        )

        if(UNITY_INCLUDE_DIR AND UNITY_LIBRARY)
            set(UNITY_FOUND TRUE)
            message(STATUS "Found Unity: ${UNITY_LIBRARY}")
        endif()
    endif()
endif()

# If Unity was found locally, create an imported target
if(UNITY_FOUND OR Unity_FOUND)
    if(NOT TARGET unity)
        add_library(unity INTERFACE IMPORTED)

        # Set include directories
        if(UNITY_INCLUDE_DIRS)
            set_target_properties(unity PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${UNITY_INCLUDE_DIRS}"
            )
        elseif(UNITY_INCLUDE_DIR)
            set_target_properties(unity PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${UNITY_INCLUDE_DIR}"
            )
        endif()

        # Set library path
        if(UNITY_LIBRARIES)
            set_target_properties(unity PROPERTIES
                INTERFACE_LINK_LIBRARIES "${UNITY_LIBRARIES}"
            )
        elseif(UNITY_LIBRARY)
            set_target_properties(unity PROPERTIES
                INTERFACE_LINK_LIBRARIES "${UNITY_LIBRARY}"
            )
        endif()

        message(STATUS "Using locally installed Unity test framework")
    endif()
else()
    # Unity not found locally, download it using FetchContent
    message(STATUS "Unity not found locally, downloading via FetchContent...")

    include(FetchContent)

    # Declare Unity as a fetchable dependency
    FetchContent_Declare(
        unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
        GIT_TAG v2.6.0
        GIT_SHALLOW TRUE        # Only clone the specific tag (faster)
        GIT_PROGRESS TRUE       # Show download progress
    )

    # Make Unity available (download and configure)
    FetchContent_MakeAvailable(unity)

    message(STATUS "Unity test framework downloaded successfully")
endif()

# ==============================================================================
# Test Executable Definition
# ==============================================================================
# Create test executable with all test source files
add_executable(TestCflZephyrSupport)

# Add test source files
target_sources(TestCflZephyrSupport
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test_cfl_zephyr_support.c
)

# ==============================================================================
# Test Dependencies and Libraries
# ==============================================================================
# Link the test executable against the library under test and Unity
target_link_libraries(TestCflZephyrSupport
    PRIVATE
        # The library we're testing (using namespace alias)
        CflZephyrSupport::CflZephyrSupport

        # Unity test framework
        unity
)

# Include Unity headers if we downloaded it via FetchContent
if(NOT UNITY_FOUND AND NOT Unity_FOUND)
    target_include_directories(TestCflZephyrSupport
        PRIVATE
            ${unity_SOURCE_DIR}/src
    )
endif()

# ==============================================================================
# Test Registration
# ==============================================================================
# Register the test executable with CTest
add_test(
    NAME cfl_zephyr_support_unit_tests
    COMMAND TestCflZephyrSupport
)

# Set test properties
set_tests_properties(cfl_zephyr_support_unit_tests
    PROPERTIES
        # Maximum time allowed for test execution (seconds)
        TIMEOUT 30

        # Working directory for test execution
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}

        # Labels for organizing tests
        LABELS "unit;cfl_zephyr_support"
)

# ==============================================================================
# Summary
# ==============================================================================
message(STATUS "Test suite configured successfully")
if(UNITY_FOUND OR Unity_FOUND)
    message(STATUS "  Unity source: System installation")
else()
    message(STATUS "  Unity source: FetchContent (downloaded)")
endif()
