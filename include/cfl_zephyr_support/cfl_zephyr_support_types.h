/* cfl_zephyr_support_types.h - Common types and definitions */

/* All Rights Reserved */

#ifndef INC_CFL_ZEPHYR_SUPPORT_TYPES_H
#define INC_CFL_ZEPHYR_SUPPORT_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes */

#include <stdint.h>

/* Configurations */


/* Definitions */

#define CFL_ZEPHYR_SUPPORT_MAX_STRING_LEN 256
#define CFL_ZEPHYR_SUPPORT_VERSION_MAJOR 1
#define CFL_ZEPHYR_SUPPORT_VERSION_MINOR 0
#define CFL_ZEPHYR_SUPPORT_VERSION_PATCH 0

/* Types */

/**
 * @brief Return status codes for library functions
 */
typedef enum
{
    CFL_ZEPHYR_SUPPORT_SUCCESS = 0,     /**< Operation successful */
    CFL_ZEPHYR_SUPPORT_ERROR = -1,      /**< Generic error */
    CFL_ZEPHYR_SUPPORT_ERROR_NULL = -2, /**< Null pointer error */
    CFL_ZEPHYR_SUPPORT_ERROR_INVALID = -3 /**< Invalid parameter error */
} cfl_zephyr_support_status_t;

/**
 * @brief Operation result structure
 */
typedef struct
{
    int32_t value;               /**< Result value */
    cfl_zephyr_support_status_t status; /**< Operation status */
} cfl_zephyr_support_result_t;

/* External Declarations */


#ifdef __cplusplus
}
#endif

#endif /* INC_CFL_ZEPHYR_SUPPORT_TYPES_H */