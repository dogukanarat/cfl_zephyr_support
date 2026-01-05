/* example.c - CLI application demonstrating scaffold_project library */

/* All Rights Reserved */

/*
 * This example demonstrates using the scaffold_project library with:
 * - getopt for command-line argument parsing
 * - Signal handling for graceful shutdown
 * - POSIX file I/O operations
 * - Error handling and verbose logging
 *
 * Users can easily modify this to add new commands and library function calls.
 */

/* Includes */

#include "scaffold_project/scaffold_project.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

/* Definitions */

#define BUFFER_SIZE 256

/* Types */

typedef enum
{
    CMD_VERSION = 0,
    CMD_ADD,
    CMD_MULTIPLY,
    CMD_FOO,
    CMD_BAR,
    CMD_FACTORIAL,
    CMD_NONE
} command_t;

/* Global Variables */

volatile sig_atomic_t keepRunning = 1;

/* Forward Declarations */

static void handleSigint(int sig);
static void printHelp(const char *progName);
static void printVersion(bool verbose);
static void doAdd(int32_t a, int32_t b, bool verbose);
static void doMultiply(int32_t a, int32_t b, bool verbose);
static void doFoo(const char *input, bool verbose);
static void doBar(int32_t value, bool verbose);
static void doFactorial(int32_t n, bool verbose);

/* Functions */

/**
 * @brief Signal handler for SIGINT (Ctrl+C)
 */
static void handleSigint(int sig)
{
    (void)sig;
    keepRunning = 0;
}

/**
 * @brief Print usage information
 */
static void printHelp(const char *progName)
{
    printf("Usage: %s [OPTIONS] <command> [arguments]\n\n", progName);
    printf("OPTIONS:\n");
    printf("  -v          Verbose mode (print detailed status messages)\n");
    printf("  -h          Show this help message\n\n");
    printf("COMMANDS:\n");
    printf("  version                        Get library version\n");
    printf("  add <a> <b>                    Add two numbers\n");
    printf("  multiply <a> <b>               Multiply two numbers\n");
    printf("  foo <string>                   Process a string\n");
    printf("  bar <number>                   Validate a number\n");
    printf("  factorial <n>                  Calculate factorial (0-12)\n\n");
    printf("EXAMPLES:\n");
    printf("  %s version\n", progName);
    printf("  %s add 5 3\n", progName);
    printf("  %s -v multiply 7 6\n", progName);
    printf("  %s foo \"hello world\"\n", progName);
    printf("  %s bar 42\n", progName);
    printf("  %s factorial 5\n", progName);
}

/**
 * @brief Main entry point
 */
int main(int argc, char *argv[])
{
    int opt;
    bool verbose = false;
    command_t command = CMD_NONE;

    /* Setup signal handling for graceful shutdown */
    struct sigaction sa;
    sa.sa_handler = handleSigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    /* Parse command-line options using getopt */
    while ((opt = getopt(argc, argv, "vh")) != -1)
    {
        switch (opt)
        {
            case 'v':
                verbose = true;
                break;
            case 'h':
                printHelp(argv[0]);
                return EXIT_SUCCESS;
            default:
                printHelp(argv[0]);
                return EXIT_FAILURE;
        }
    }

    /* Check if command is provided */
    if (optind >= argc)
    {
        fprintf(stderr, "Error: No command specified\n\n");
        printHelp(argv[0]);
        return EXIT_FAILURE;
    }

    /* Parse command */
    const char *cmdStr = argv[optind];

    if (strcmp(cmdStr, "version") == 0)
    {
        command = CMD_VERSION;
    }
    else if (strcmp(cmdStr, "add") == 0)
    {
        command = CMD_ADD;
    }
    else if (strcmp(cmdStr, "multiply") == 0)
    {
        command = CMD_MULTIPLY;
    }
    else if (strcmp(cmdStr, "foo") == 0)
    {
        command = CMD_FOO;
    }
    else if (strcmp(cmdStr, "bar") == 0)
    {
        command = CMD_BAR;
    }
    else if (strcmp(cmdStr, "factorial") == 0)
    {
        command = CMD_FACTORIAL;
    }
    else
    {
        fprintf(stderr, "Error: Unknown command '%s'\n\n", cmdStr);
        printHelp(argv[0]);
        return EXIT_FAILURE;
    }

    /* Execute command */
    switch (command)
    {
        case CMD_VERSION:
            printVersion(verbose);
            break;

        case CMD_ADD:
            if (optind + 2 >= argc)
            {
                fprintf(stderr, "Error: 'add' requires two arguments\n");
                return EXIT_FAILURE;
            }
            doAdd((int32_t)atoi(argv[optind + 1]), (int32_t)atoi(argv[optind + 2]), verbose);
            break;

        case CMD_MULTIPLY:
            if (optind + 2 >= argc)
            {
                fprintf(stderr, "Error: 'multiply' requires two arguments\n");
                return EXIT_FAILURE;
            }
            doMultiply(
                (int32_t)atoi(argv[optind + 1]),
                (int32_t)atoi(argv[optind + 2]),
                verbose);
            break;

        case CMD_FOO:
            if (optind + 1 >= argc)
            {
                fprintf(stderr, "Error: 'foo' requires a string argument\n");
                return EXIT_FAILURE;
            }
            doFoo(argv[optind + 1], verbose);
            break;

        case CMD_BAR:
            if (optind + 1 >= argc)
            {
                fprintf(stderr, "Error: 'bar' requires a number argument\n");
                return EXIT_FAILURE;
            }
            doBar((int32_t)atoi(argv[optind + 1]), verbose);
            break;

        case CMD_FACTORIAL:
            if (optind + 1 >= argc)
            {
                fprintf(stderr, "Error: 'factorial' requires a number argument\n");
                return EXIT_FAILURE;
            }
            doFactorial((int32_t)atoi(argv[optind + 1]), verbose);
            break;

        case CMD_NONE:
        default:
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Print library version
 */
static void printVersion(bool verbose)
{
    if (verbose)
    {
        const char msg[] = "Calling scaffold_project_get_version()...\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
    }

    const char *version = scaffold_project_get_version();
    printf("%s\n", version);

    if (verbose)
    {
        const char msg[] = "Version retrieved successfully\n";
        write(STDERR_FILENO, msg, sizeof(msg) - 1);
    }
}

/**
 * @brief Demonstrate addition function
 */
static void doAdd(int32_t a, int32_t b, bool verbose)
{
    if (verbose)
    {
        dprintf(STDERR_FILENO, "Calling scaffold_project_add(%d, %d)...\n", a, b);
    }

    int32_t result = scaffold_project_add(a, b);
    printf("%d\n", result);

    if (verbose)
    {
        dprintf(STDERR_FILENO, "Result: %d + %d = %d\n", a, b, result);
    }
}

/**
 * @brief Demonstrate multiplication with error handling
 */
static void doMultiply(int32_t a, int32_t b, bool verbose)
{
    int32_t result = 0;

    if (verbose)
    {
        dprintf(STDERR_FILENO, "Calling scaffold_project_multiply(%d, %d, &result)...\n", a, b);
    }

    scaffold_project_status_t status = scaffold_project_multiply(a, b, &result);

    if (status == SCAFFOLD_PROJECT_SUCCESS)
    {
        printf("%d\n", result);
        if (verbose)
        {
            dprintf(STDERR_FILENO, "Result: %d × %d = %d (status: SUCCESS)\n", a, b, result);
        }
    }
    else
    {
        fprintf(stderr, "Error: multiply failed with status code %d\n", status);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Demonstrate string processing
 */
static void doFoo(const char *input, bool verbose)
{
    char output[BUFFER_SIZE];

    if (verbose)
    {
        dprintf(STDERR_FILENO, "Calling scaffold_project_foo(\"%s\", output, %d)...\n", input,
                BUFFER_SIZE);
    }

    scaffold_project_status_t status = scaffold_project_foo(input, output, sizeof(output));

    if (status == SCAFFOLD_PROJECT_SUCCESS)
    {
        printf("%s\n", output);
        if (verbose)
        {
            dprintf(STDERR_FILENO, "String processed successfully (status: SUCCESS)\n");
        }
    }
    else
    {
        fprintf(stderr, "Error: foo failed with status code %d\n", status);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Demonstrate validation function
 */
static void doBar(int32_t value, bool verbose)
{
    if (verbose)
    {
        dprintf(STDERR_FILENO, "Calling scaffold_project_bar(%d)...\n", value);
    }

    bool isValid = scaffold_project_bar(value);
    printf("%s\n", isValid ? "valid" : "invalid");

    if (verbose)
    {
        dprintf(STDERR_FILENO, "Validation result: %s\n", isValid ? "valid" : "invalid");
    }
}

/**
 * @brief Demonstrate factorial with result structure
 */
static void doFactorial(int32_t n, bool verbose)
{
    if (verbose)
    {
        dprintf(STDERR_FILENO, "Calling scaffold_project_factorial(%d)...\n", n);
    }

    scaffold_project_result_t result = scaffold_project_factorial(n);

    if (result.status == SCAFFOLD_PROJECT_SUCCESS)
    {
        printf("%d\n", result.value);
        if (verbose)
        {
            dprintf(STDERR_FILENO, "Factorial: %d! = %d (status: SUCCESS)\n", n, result.value);
        }
    }
    else
    {
        fprintf(stderr, "Error: factorial failed with status code %d\n", result.status);
        if (result.status == SCAFFOLD_PROJECT_ERROR_INVALID)
        {
            fprintf(stderr, "Note: Input must be between 0 and 12\n");
        }
        exit(EXIT_FAILURE);
    }
}