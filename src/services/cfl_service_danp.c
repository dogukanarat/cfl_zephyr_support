/* cfl_service_danp.c - CFL Service over DANP Transport */

/* All Rights Reserved */

/* Includes */

#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "danp/danp.h"
#include "danp/danp_buffer.h"
#include "danp/danp_types.h"
#include "osal/osal_thread.h"
#include "osal/osal_time.h"
#include "zephyr/tmtc.h"

#include "zephyr/logging/log.h"

#include "cfl/cfl.h"
#include "cfl/services/cfl_service_danp.h"

/* Private Definitions */

LOG_MODULE_REGISTER(cfl_service, CONFIG_CFL_LOG_LEVEL);

/* Private Types */

typedef struct cfl_service_danp_ctx_s
{
    bool initialized;
    volatile bool running;
    uint16_t local_port;
    danp_socket_t *socket;
    osal_thread_handle_t rx_task_handle;
} cfl_service_danp_ctx_t;

/* Private Variables */

static cfl_service_danp_ctx_t context;

/* Public Functions */

static int32_t cfl_process_message(
    danp_packet_t *rqst_pkt,
    danp_packet_t **rply_pkt,
    danp_packet_t **status_pkt)
{
    int32_t ret = 0;
    cfl_message_t *rqst_msg = (cfl_message_t *)rqst_pkt->payload;
    cfl_message_t *rply_msg = NULL;
    cfl_message_t *status_msg = NULL;
    const struct tmtc_cmd_handler *handler = NULL;
    struct tmtc_args rqst = {0};
    struct tmtc_args rply = {0};

    for (;;)
    {
        LOG_DBG("Processing message");

        if (rqst_pkt == NULL || rqst_pkt->length < CFL_HEADER_SIZE)
        {
            LOG_ERR("Request packet is NULL or too short");
            ret = -EINVAL;
            break;
        }

        /* Validate complete message including CRC */
        if (rqst_pkt->length != (CFL_HEADER_SIZE + rqst_msg->length))
        {
            LOG_ERR("Incomplete message received");
            ret = -EINVAL;
            break;
        }

        /* Handle based on message type */
        if (rqst_msg->flags & CFL_F_RQST)
        {
            LOG_DBG("Handling request message");
            /* Find handler for this request ID */
            handler = tmtc_get_cmd_handler(rqst_msg->id);
            if (NULL == handler)
            {
                ret = -EINVAL;
                LOG_ERR("No handler found for request ID: %d", rqst_msg->id);
                /* No handler - send NACK */
                *status_pkt = danp_buffer_get();
                if (*status_pkt == NULL)
                {
                    LOG_ERR("Failed to allocate reply packet");
                    ret = -ENOMEM;
                    break;
                }
                status_msg = (cfl_message_t *)(*status_pkt)->payload;
                status_msg->version = CFL_VERSION;
                status_msg->seq = rqst_msg->seq;
                status_msg->sync = CFL_SYNC_WORD;
                status_msg->id = rqst_msg->id;
                status_msg->flags = CFL_F_NACK;
                status_msg->length = sizeof(ret);
                memcpy(status_msg->data, &ret, status_msg->length);
                (*status_pkt)->length = CFL_HEADER_SIZE + status_msg->length;
                break;
            }

            LOG_DBG("Executing handler for request ID: %d", rqst_msg->id);
            rqst.hdr_len = CFL_HEADER_SIZE;
            rqst.data = rqst_msg;
            rqst.len = rqst_pkt->length;
            rqst.incomplete = false;

            rply.hdr_len = CFL_HEADER_SIZE;
            rply.data = NULL;
            rply.len = 0;
            rply.incomplete = false;

            ret = tmtc_run_handler(handler, &rqst, &rply);
            if (ret < 0)
            {
                LOG_ERR("Handler execution failed with error: %d", ret);
                *status_pkt = danp_buffer_get();
                if (*status_pkt == NULL)
                {
                    LOG_ERR("Failed to allocate status packet");
                    ret = -ENOMEM;
                    break;
                }

                status_msg = (cfl_message_t *)(*status_pkt)->payload;
                status_msg->version = CFL_VERSION;
                status_msg->seq = rqst_msg->seq;
                status_msg->sync = CFL_SYNC_WORD;
                status_msg->id = rqst_msg->id;
                status_msg->flags = CFL_F_NACK;
                status_msg->length = sizeof(ret);
                memcpy(status_msg->data, &ret, status_msg->length);
                (*status_pkt)->length = CFL_HEADER_SIZE + status_msg->length;
                break;
            }

            if (NULL == rply.data)
            {
                *status_pkt = danp_buffer_get();
                if (*status_pkt == NULL)
                {
                    LOG_ERR("Failed to allocate status packet");
                    ret = -ENOMEM;
                    break;
                }

                status_msg = (cfl_message_t *)(*status_pkt)->payload;
                status_msg->version = CFL_VERSION;
                status_msg->seq = rqst_msg->seq;
                status_msg->sync = CFL_SYNC_WORD;
                status_msg->id = rqst_msg->id;
                status_msg->flags = CFL_F_ACK;
                status_msg->length = 0;
                (*status_pkt)->length = CFL_HEADER_SIZE + status_msg->length;
                break;
            }
            else
            {
                if (rply.len > CFL_MAX_PAYLOAD_SIZE)
                {
                    LOG_ERR("Reply data exceeds max payload size");
                    ret = CFL_ERR_NO_RESOURCE;
                    break;
                }

                *rply_pkt = danp_buffer_get();
                if (*rply_pkt == NULL)
                {
                    LOG_ERR("Failed to allocate reply packet");
                    ret = -ENOMEM;
                    break;
                }

                rply_msg = (cfl_message_t *)(*rply_pkt)->payload;
                rply_msg->version = CFL_VERSION;
                rply_msg->seq = rqst_msg->seq;
                rply_msg->sync = CFL_SYNC_WORD;
                rply_msg->id = rqst_msg->id;
                rply_msg->flags = CFL_F_RPLY;
                rply_msg->length = rply.len;
                memcpy(rply_msg->data, rply.data, rply_msg->length);
                (*rply_pkt)->length = CFL_HEADER_SIZE + rply_msg->length;

                break;
            }
        }
        else if (rqst_msg->flags & CFL_F_PUSH)
        {
            LOG_DBG("Handling push message");
            /* Find handler for this push ID */
            handler = tmtc_get_cmd_handler(rqst_msg->id);
            if (NULL == handler)
            {
                LOG_ERR("No handler found for push ID: %d", rqst_msg->id);
                /* No handler - ignore push */
                break;
            }

            LOG_DBG("Executing handler for push ID: %d", rqst_msg->id);
            rqst.hdr_len = CFL_HEADER_SIZE;
            rqst.data = rqst_msg;
            rqst.len = rqst_pkt->length;
            rqst.incomplete = false;

            rply.hdr_len = CFL_HEADER_SIZE;
            rply.data = NULL;
            rply.len = 0;
            rply.incomplete = false;

            ret = tmtc_run_handler(handler, &rqst, &rply);

            /* Push messages do not expect a reply */
            if (NULL != rply.data)
            {
                LOG_WRN("Handler returned unexpected reply data for push message");
            }
        }
        else
        {
            LOG_ERR("Unknown message flag");
            break;
        }

        break;
    }

    LOG_DBG("Message processing completed with result: %d", ret);
    return ret;
}

static void cfl_danp_rx_task(void *arg)
{
    cfl_service_danp_ctx_t *ctx = (cfl_service_danp_ctx_t *)arg;
    danp_packet_t *rqst_pkt = NULL;
    danp_packet_t *rply_pkt = NULL;
    danp_packet_t *status_pkt = NULL;
    uint16_t src_node = 0;
    uint16_t src_port = 0;

    LOG_INF("TMTC service initialized on port %d", ctx->local_port);

    while (ctx->running)
    {
        if (NULL != rqst_pkt)
        {
            LOG_DBG("Freeing previous request packet");
            danp_buffer_free(rqst_pkt);
            rqst_pkt = NULL;
        }

        rply_pkt = NULL;
        status_pkt = NULL;
        rqst_pkt = danp_recv_packet_from(ctx->socket, &src_node, &src_port, CFL_DANP_RX_TIMEOUT_MS);

        if (NULL != rqst_pkt)
        {
            LOG_DBG("Received packet from node: %d, port: %d", src_node, src_port);
            cfl_process_message(rqst_pkt, &rply_pkt, &status_pkt);
        }

        if (NULL != status_pkt)
        {
            LOG_DBG("Sending status packet to node: %d, port: %d", src_node, src_port);
            danp_send_packet_to(ctx->socket, status_pkt, src_node, src_port);
        }

        if (NULL != rply_pkt)
        {
            LOG_DBG("Sending reply packet to node: %d, port: %d", src_node, src_port);
            danp_send_packet_to(ctx->socket, rply_pkt, src_node, src_port);
        }
    }

    LOG_DBG("RX task exiting");
    osal_thread_delete(context.rx_task_handle);
}

int32_t cfl_service_danp_init(const cfl_service_danp_config_t *config)
{
    int32_t ret = 0;
    bool is_socket_created = false;
    osal_thread_attr_t task_attr = {
        .name = "cfl_rx",
        .priority = CFL_DANP_RX_TASK_PRIORITY,
        .stack_size = CFL_DANP_RX_TASK_STACK_SIZE,
    };

    for (;;)
    {
        LOG_DBG("Initializing CFL service over DANP");

        if (config == NULL)
        {
            LOG_ERR("Configuration is NULL");
            ret = -EINVAL;
            break;
        }

        if (context.initialized)
        {
            LOG_ERR("Service already initialized");
            ret = -EALREADY;
            break;
        }

        memset(&context, 0, sizeof(context));
        context.local_port = config->port_id;

        LOG_DBG("Creating socket");
        context.socket = danp_socket(DANP_TYPE_DGRAM);
        if (context.socket == NULL)
        {
            LOG_ERR("Failed to create socket");
            ret = -ENOMEM;
            break;
        }
        is_socket_created = true;

        LOG_DBG("Binding socket to port: %d", context.local_port);
        ret = danp_bind(context.socket, context.local_port);
        if (ret < 0)
        {
            LOG_ERR("Failed to bind socket");
            ret = -EADDRNOTAVAIL;
            break;
        }
        context.running = true;

        LOG_DBG("Creating RX task");
        context.rx_task_handle = osal_thread_create(cfl_danp_rx_task, &context, &task_attr);
        if (context.rx_task_handle == NULL)
        {
            LOG_ERR("Failed to create RX task");
            ret = -ENOMEM;
            break;
        }

        context.initialized = true;
        break;
    }

    if (0 != ret)
    {
        if (is_socket_created && context.socket != NULL)
        {
            LOG_DBG("Closing socket due to initialization failure");
            danp_close(context.socket);
            context.socket = NULL;
        }

        context.initialized = false;
        context.running = false;
    }

    LOG_DBG("Initialization completed with result: %d", ret);
    return ret;
}

int32_t cfl_service_danp_deinit(void)
{
    int32_t ret = 0;

    for (;;)
    {
        LOG_DBG("Deinitializing CFL service over DANP");

        if (!context.initialized)
        {
            LOG_ERR("Service not initialized");
            ret = -EINVAL;
            break;
        }

        LOG_DBG("Signaling RX task to stop");
        context.running = false;
        osal_delay_ms(CFL_DANP_RX_TIMEOUT_MS * 2);

        if (context.socket != NULL)
        {
            LOG_DBG("Closing socket");
            danp_close(context.socket);
            context.socket = NULL;
        }

        memset(&context, 0, sizeof(context));
        break;
    }

    LOG_DBG("Deinitialization completed with result: %d", ret);
    return ret;
}

int32_t cfl_service_danp_unregister_handler(uint16_t id)
{
    (void)id;
    /* Handler registration is managed by the tmtc subsystem */
    LOG_WRN("Handler unregistration not supported through this interface");
    return -ENOTSUP;
}

int32_t cfl_service_danp_send_request(
    uint16_t dst_node,
    uint16_t dst_port,
    uint16_t id,
    const uint8_t *payload,
    uint16_t payload_len,
    uint16_t *seq_out)
{
    int32_t ret = CFL_OK;
    danp_packet_t *pkt = NULL;
    cfl_message_t *msg = NULL;
    int32_t sent_len = 0;

    (void)seq_out; /* Sequence number tracking not implemented */

    for (;;)
    {
        if (!context.initialized)
        {
            LOG_ERR("Service not initialized");
            ret = CFL_ERR_NOT_INIT;
            break;
        }

        if (payload_len > 0 && payload == NULL)
        {
            LOG_ERR("Payload is NULL but length is non-zero");
            ret = CFL_ERR_NULL;
            break;
        }

        pkt = danp_buffer_get();
        if (pkt == NULL)
        {
            LOG_ERR("Failed to allocate packet buffer");
            ret = CFL_ERR_NO_RESOURCE;
            break;
        }

        msg = (cfl_message_t *)pkt->payload;
        cfl_message_init(msg, id, CFL_F_RQST);

        if (payload_len > 0)
        {
            memcpy(msg->data, payload, payload_len);
        }
        cfl_message_set_length(msg, payload_len);
        cfl_message_compute_crc(msg);
        pkt->length = CFL_HEADER_SIZE + payload_len;

        sent_len = danp_send_packet_to(context.socket, pkt, dst_node, dst_port);
        if (sent_len < 0)
        {
            LOG_ERR("Failed to send request packet");
            ret = CFL_ERR_TRANSPORT;
            break;
        }

        LOG_DBG("Sent request to node %d port %d, id %d", dst_node, dst_port, id);
        break;
    }

    return ret;
}

int32_t cfl_service_danp_send_push(
    uint16_t dst_node,
    uint16_t dst_port,
    uint16_t id,
    const uint8_t *payload,
    uint16_t payload_len)
{
    int32_t ret = CFL_OK;
    danp_packet_t *pkt = NULL;
    cfl_message_t *msg = NULL;
    int32_t sent_len = 0;

    for (;;)
    {
        if (!context.initialized)
        {
            LOG_ERR("Service not initialized");
            ret = CFL_ERR_NOT_INIT;
            break;
        }

        if (payload_len > 0 && payload == NULL)
        {
            LOG_ERR("Payload is NULL but length is non-zero");
            ret = CFL_ERR_NULL;
            break;
        }

        pkt = danp_buffer_get();
        if (pkt == NULL)
        {
            LOG_ERR("Failed to allocate packet buffer");
            ret = CFL_ERR_NO_RESOURCE;
            break;
        }

        msg = (cfl_message_t *)pkt->payload;
        cfl_message_init(msg, id, CFL_F_PUSH);

        if (payload_len > 0)
        {
            memcpy(msg->data, payload, payload_len);
        }
        cfl_message_set_length(msg, payload_len);
        cfl_message_compute_crc(msg);
        pkt->length = CFL_HEADER_SIZE + payload_len;

        sent_len = danp_send_packet_to(context.socket, pkt, dst_node, dst_port);
        if (sent_len < 0)
        {
            LOG_ERR("Failed to send push packet");
            ret = CFL_ERR_TRANSPORT;
            break;
        }

        LOG_DBG("Sent push to node %d port %d, id %d", dst_node, dst_port, id);
        break;
    }

    return ret;
}