/* cfl_zephyr_support.h - Main API header for cfl_zephyr_support library */

/* All Rights Reserved */

#ifndef INC_CFL_ZEPHYR_SUPPORT_H
#define INC_CFL_ZEPHYR_SUPPORT_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes */

#include "cfl_zephyr_support_types.h"
#include <stdbool.h>
#include <stddef.h>

/* Configurations */


/* Definitions */


/* Types */


/* External Declarations */

/**
 * @brief Get library version string
 *
 * @return Version string in format "major.minor.patch"
 */
const char *cfl_zephyr_support_get_version(void);

/**
 * @brief Add two integers
 *
 * @param a First operand
 * @param b Second operand
 * @return Sum of a and b
 */
int32_t cfl_zephyr_support_add(int32_t a, int32_t b);

/**
 * @brief Multiply two integers with error handling
 *
 * @param a First operand
 * @param b Second operand
 * @param result Pointer to store result
 * @return CFL_ZEPHYR_SUPPORT_SUCCESS on success, error code otherwise
 */
cfl_zephyr_support_status_t cfl_zephyr_support_multiply(
    int32_t a,
    int32_t b,
    int32_t *result);

/**
 * @brief Example function that processes a string
 *
 * @param input Input string to process
 * @param output Buffer to store processed string
 * @param outputSize Size of output buffer
 * @return CFL_ZEPHYR_SUPPORT_SUCCESS on success, error code otherwise
 */
cfl_zephyr_support_status_t cfl_zephyr_support_foo(
    const char *input,
    char *output,
    size_t outputSize);

/**
 * @brief Example function that validates input
 *
 * @param value Value to validate
 * @return true if valid, false otherwise
 */
bool cfl_zephyr_support_bar(int32_t value);

/**
 * @brief Compute factorial of a number
 *
 * @param n Input number (must be >= 0 and <= 12)
 * @return Result structure with factorial value and status
 */
cfl_zephyr_support_result_t cfl_zephyr_support_factorial(int32_t n);

#ifdef __cplusplus
}
#endif

#endif /* INC_CFL_ZEPHYR_SUPPORT_H */