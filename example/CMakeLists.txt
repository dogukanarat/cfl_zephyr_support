# ==============================================================================
# Example Applications
# ==============================================================================
# This CMakeLists.txt builds example applications that demonstrate how to use
# the scaffold_project library.
#
# Users can:
# - Add new example applications by creating new executables
# - Modify existing examples to demonstrate different features
# - Choose whether to install examples (disabled by default)

# ==============================================================================
# Example: Basic CLI Application
# ==============================================================================
# Create the CLI example executable
add_executable(ScaffoldProjectExample
    example.c
)

# Link with the scaffold_project library
# Using the alias target ensures proper namespacing
target_link_libraries(ScaffoldProjectExample
    PRIVATE
        ScaffoldProject::ScaffoldProject
)

# Set C99 standard for the example
target_compile_features(ScaffoldProjectExample
    PRIVATE
        c_std_99
)

# Add compiler warnings for the example (same as library)
target_compile_options(ScaffoldProjectExample
    PRIVATE
        # GCC and Clang warnings
        $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:
            -Wall
            -Wextra
            -Wpedantic
        >
        # MSVC warnings
        $<$<C_COMPILER_ID:MSVC>:
            /W4
        >
)

# ==============================================================================
# Adding New Examples (Template)
# ==============================================================================
# To add a new example application, copy and modify this template:
#
# add_executable(example_scaffold_project_myexample
#     myexample.c
# )
#
# target_link_libraries(example_scaffold_project_myexample
#     PRIVATE
#         ScaffoldProject::ScaffoldProject
# )
#
# target_compile_features(example_scaffold_project_myexample
#     PRIVATE
#         c_std_99
# )

# ==============================================================================
# Installation (Optional)
# ==============================================================================
# By default, examples are not installed to keep the installation clean.
# Uncomment the section below if you want to install example executables.

# install(
#     TARGETS ScaffoldProjectExample
#     RUNTIME
#         DESTINATION ${CMAKE_INSTALL_BINDIR}
#         COMPONENT Examples
# )

# To install examples along with the library:
# cmake --install . --component Examples
