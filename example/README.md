# Example Applications

This directory contains example applications that demonstrate how to use the scaffold_project library with proper POSIX-compliant CLI design.

## Building Examples

To build the example applications, enable the `BUILD_EXAMPLES` option when configuring CMake:

```bash
mkdir build && cd build
cmake -DBUILD_EXAMPLES=ON ..
make
```

## Running the CLI Example

After building, run the example from the build directory:

```bash
./ScaffoldProjectExample <command> [arguments]
```

### Available Commands

```bash
# Get library version
./ScaffoldProjectExample version

# Add two numbers
./ScaffoldProjectExample add 5 3

# Multiply two numbers (demonstrates error handling)
./ScaffoldProjectExample multiply 7 6

# Process a string
./ScaffoldProjectExample foo "hello world"

# Validate a number
./ScaffoldProjectExample bar 42

# Calculate factorial (must be 0-12)
./ScaffoldProjectExample factorial 5
```

### Verbose Mode

Use the `-v` flag to see detailed execution information:

```bash
./ScaffoldProjectExample -v add 10 20
# Output to stderr: Calling scaffold_project_add(10, 20)...
# Output to stdout: 30
# Output to stderr: Result: 10 + 20 = 30
```

### Help

Display usage information:

```bash
./ScaffoldProjectExample -h
```

## Understanding the Example Code

The example application (`example.c`) demonstrates professional CLI application design using POSIX standards:

### Code Structure

```c
/* Standard file template sections */
/* Includes */              - Header files and library includes
/* Definitions */           - Constants and buffer sizes
/* Types */                 - Command enumerations
/* Global Variables */      - Signal handling flags
/* Forward Declarations */  - Function prototypes
/* Functions */             - Implementation
```

### Key Features

1. **getopt() Argument Parsing**
   - Standard POSIX option parsing
   - Supports short options (-v, -h)
   - Command-based interface

2. **Signal Handling**
   - Graceful shutdown on Ctrl+C (SIGINT)
   - Uses sigaction() for reliable signal handling
   - Demonstrates volatile sig_atomic_t flag

3. **Error Handling**
   - Checks library function return values
   - Proper error messages to stderr
   - Exit codes for success/failure

4. **Verbose Logging**
   - Optional verbose mode with -v flag
   - Uses write() and dprintf() to stderr
   - Doesn't mix logs with output

5. **POSIX Compliance**
   - Uses only standard POSIX libraries
   - No external dependencies beyond the library itself
   - Portable across Unix-like systems

### POSIX Functions Demonstrated

- `getopt()` - Command-line option parsing
- `sigaction()` - Signal handling setup
- `write()` - Low-level output (signal-safe)
- `dprintf()` - Formatted output to file descriptor
- `fprintf()`, `printf()` - Formatted output
- `strcmp()` - String comparison
- `atoi()` - String to integer conversion

## Customizing the Example

### Adding a New Command

To add a new command that calls your library function:

1. **Add command to enum** in the Types section:
```c
typedef enum
{
    CMD_VERSION = 0,
    CMD_ADD,
    CMD_MULTIPLY,
    CMD_FOO,
    CMD_BAR,
    CMD_FACTORIAL,
    CMD_MY_NEW_CMD,  // Add here
    CMD_NONE
} command_t;
```

2. **Add forward declaration**:
```c
static void doMyNewCmd(int32_t param, bool verbose);
```

3. **Add command string parsing** in main():
```c
else if (strcmp(cmdStr, "mynewcmd") == 0)
{
    command = CMD_MY_NEW_CMD;
}
```

4. **Add case in switch statement**:
```c
case CMD_MY_NEW_CMD:
    if (optind + 1 >= argc)
    {
        fprintf(stderr, "Error: 'mynewcmd' requires an argument\n");
        return EXIT_FAILURE;
    }
    doMyNewCmd((int32_t)atoi(argv[optind + 1]), verbose);
    break;
```

5. **Implement the function**:
```c
static void doMyNewCmd(int32_t param, bool verbose)
{
    if (verbose)
    {
        dprintf(STDERR_FILENO, "Calling my_library_function(%d)...\n", param);
    }

    /* Call your library function here */
    int32_t result = my_library_function(param);

    printf("%d\n", result);

    if (verbose)
    {
        dprintf(STDERR_FILENO, "Operation completed successfully\n");
    }
}
```

6. **Update help text** in printHelp():
```c
printf("  mynewcmd <param>               Description of command\n");
```

### Adding New Options

To add a new command-line option (like `-o` for output file):

1. **Update getopt string** in main():
```c
while ((opt = getopt(argc, argv, "vho:")) != -1)  // Added 'o:'
```

2. **Add case in switch**:
```c
case 'o':
    outputFile = optarg;  // Declare variable at top of main()
    break;
```

3. **Update help text**:
```c
printf("  -o <file>   Write output to file\n");
```

## Common Patterns Demonstrated

### Simple Function Call (version)
```c
const char *version = scaffold_project_get_version();
printf("%s\n", version);
```

### Function with Return Value (add)
```c
int32_t result = scaffold_project_add(a, b);
printf("%d\n", result);
```

### Error Handling with Status Codes (multiply)
```c
scaffold_project_status_t status = scaffold_project_multiply(a, b, &result);
if (status == SCAFFOLD_PROJECT_SUCCESS)
{
    printf("%d\n", result);
}
else
{
    fprintf(stderr, "Error: multiply failed with status code %d\n", status);
    exit(EXIT_FAILURE);
}
```

### String Processing with Buffers (foo)
```c
char output[BUFFER_SIZE];
scaffold_project_status_t status = scaffold_project_foo(input, output, sizeof(output));
if (status == SCAFFOLD_PROJECT_SUCCESS)
{
    printf("%s\n", output);
}
```

### Boolean Return Values (bar)
```c
bool isValid = scaffold_project_bar(value);
printf("%s\n", isValid ? "valid" : "invalid");
```

### Result Structures (factorial)
```c
scaffold_project_result_t result = scaffold_project_factorial(n);
if (result.status == SCAFFOLD_PROJECT_SUCCESS)
{
    printf("%d\n", result.value);
}
else
{
    fprintf(stderr, "Error: factorial failed with status code %d\n", result.status);
}
```

## Design Principles

This example follows these CLI design principles:

1. **Separate output from logging**: Results go to stdout, logs/errors to stderr
2. **Exit codes**: Return EXIT_SUCCESS (0) or EXIT_FAILURE (1)
3. **Help text**: Always provide -h for usage information
4. **Error messages**: Clear, actionable error messages
5. **Argument validation**: Check argument count before parsing
6. **Signal handling**: Clean shutdown on interruption
7. **POSIX compliance**: Portable, standard library usage

## Advanced: Piping and Redirection

The example is designed to work well with Unix pipes and redirection:

```bash
# Output only (stdout)
./ScaffoldProjectExample add 5 3 > result.txt

# Errors only (stderr)
./ScaffoldProjectExample add 5 3 2> errors.txt

# Verbose output to stderr, result to stdout
./ScaffoldProjectExample -v add 5 3 2> log.txt > result.txt

# Chain commands
./ScaffoldProjectExample add 5 3 | xargs ./ScaffoldProjectExample multiply 2
```

## Migration Notes

After running `./scripts/migration.sh YourLibName`, the example files will be automatically updated:

- `example.c` - All `scaffold_project_*` calls → `your_lib_name_*`
- `ScaffoldProjectExample` → `example_your_lib_name_cli` (executable)
- Function names, types, and constants automatically renamed

Make sure to:
1. Update the command implementations to match your actual API
2. Add/remove commands based on your library's functionality
3. Update help text to describe your library's purpose

## Further Reading

- `../include/scaffold_project/scaffold_project.h` - Full API documentation
- `../docs/FORMATTING.md` - Code style guidelines
- `../test/` - Unit test examples
- `man getopt` - POSIX getopt documentation
- `man sigaction` - Signal handling documentation
