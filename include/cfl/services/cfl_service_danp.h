/* cfl_service_danp.h - CFL Service over DANP Transport */

/* All Rights Reserved */

#ifndef INC_CFL_SERVICE_DANP_H
#define INC_CFL_SERVICE_DANP_H

/* Includes */

#include <stdint.h>
#include "osal/osal_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Configurations */

#ifndef CFL_DANP_RX_TASK_STACK_SIZE
#define CFL_DANP_RX_TASK_STACK_SIZE (2048)
#endif

#ifndef CFL_DANP_RX_TASK_PRIORITY
#define CFL_DANP_RX_TASK_PRIORITY (OSAL_THREAD_PRIORITY_NORMAL)
#endif

#ifndef CFL_DANP_MAX_HANDLERS
#define CFL_DANP_MAX_HANDLERS (32)
#endif

#ifndef CFL_DANP_RX_TIMEOUT_MS
#define CFL_DANP_RX_TIMEOUT_MS (1000)
#endif

/* Definitions */

#define CFL_ERR_ALREADY_INIT (-10)
#define CFL_ERR_NOT_INIT     (-11)
#define CFL_ERR_TRANSPORT    (-12)
#define CFL_ERR_NO_RESOURCE  (-13)
#define CFL_ERR_EXISTS       (-14)
#define CFL_ERR_NOT_FOUND    (-15)

/* Types */

typedef struct cfl_service_danp_config_s {
    uint16_t port_id;
} cfl_service_danp_config_t;

/* External Declarations */

/**
 * @brief Initialize CFL service over DANP transport
 * @param config Service configuration
 * @return CFL_OK on success, negative error code on failure
 */
extern int32_t cfl_service_danp_init(const cfl_service_danp_config_t *config);

/**
 * @brief Deinitialize CFL service
 * @return CFL_OK on success, negative error code on failure
 */
extern int32_t cfl_service_danp_deinit(void);

/**
 * @brief Unregister a handler for a specific message ID
 * @param id Message ID to unregister
 * @return CFL_OK on success, negative error code on failure
 */
extern int32_t cfl_service_danp_unregister_handler(uint16_t id);

/**
 * @brief Send a request message
 * @param dst_node    Destination node address
 * @param dst_port    Destination port
 * @param id          Message ID
 * @param payload     Payload data (can be NULL if payload_len is 0)
 * @param payload_len Payload length in bytes
 * @param seq_out     Optional output for sequence number used
 * @return CFL_OK on success, negative error code on failure
 */
extern int32_t cfl_service_danp_send_request(
    uint16_t dst_node,
    uint16_t dst_port,
    uint16_t id,
    const uint8_t *payload,
    uint16_t payload_len,
    uint16_t *seq_out);

/**
 * @brief Send a push message (no response expected)
 * @param dst_node    Destination node address
 * @param dst_port    Destination port
 * @param id          Message ID
 * @param payload     Payload data (can be NULL if payload_len is 0)
 * @param payload_len Payload length in bytes
 * @return CFL_OK on success, negative error code on failure
 */
extern int32_t cfl_service_danp_send_push(
    uint16_t dst_node,
    uint16_t dst_port,
    uint16_t id,
    const uint8_t *payload,
    uint16_t payload_len);

#ifdef __cplusplus
}
#endif

#endif /* INC_CFL_SERVICE_DANP_H */
