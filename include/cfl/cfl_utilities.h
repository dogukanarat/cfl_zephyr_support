/* cfl_utilities.h - one line definition */

/* All Rights Reserved */

#ifndef INC_CFL_UTILITIES_H
#define INC_CFL_UTILITIES_H

/* Includes */

#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Configurations */


/* Definitions */


/* Types */


/* External Declarations */


extern int32_t tmtc_transaction(
    uint16_t dest_id,
    uint16_t tmtc_id,
    const uint8_t *request,
    uint16_t request_len,
    uint8_t *reply,
    uint16_t reply_size,
    uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /* INC_CFL_UTILITIES_H */