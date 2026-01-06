/* example_embedded.c - Embedded-safe example for scaffold_project */

/* All Rights Reserved */

/*
 * Minimal example that avoids POSIX APIs for embedded/OSAL environments.
 * This is intended to compile on bare-metal or RTOS targets.
 */

/* Includes */

#include "scaffold_project/scaffold_project.h"
#include <stddef.h>
#include <stdint.h>

/* Definitions */

#define OUTPUT_BUFFER_SIZE 32

/* Types */

/* Forward Declarations */

/* Functions */

int main(void)
{
    int32_t multiplyResult = 0;
    char output[OUTPUT_BUFFER_SIZE];

    (void)scaffold_project_get_version();

    (void)scaffold_project_add(10, 20);
    (void)scaffold_project_multiply(3, 7, &multiplyResult);
    (void)scaffold_project_foo("embedded", output, sizeof(output));
    (void)scaffold_project_bar(42);
    (void)scaffold_project_factorial(5);

    (void)multiplyResult;
    (void)output;

    return 0;
}
