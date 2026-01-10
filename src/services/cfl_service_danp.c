/* cfl_service_danp.c - CFL Service over DANP Transport */

/* All Rights Reserved */

/* Includes */

#include <stddef.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "zephyr/logging/log.h"
#include "zephyr/logging/log_instance.h"
#include "zephyr/tmtc.h"

#include "cfl/cfl.h"
#include "cfl/services/cfl_service_danp.h"
#include "danp/danp.h"
#include "danp/danp_buffer.h"
#include "danp/danp_types.h"
#include "osal/osal_thread.h"
#include "osal/osal_time.h"

/* Private Definitions */

LOG_MODULE_DECLARE(cfl);

LOG_INSTANCE_REGISTER(cfl, service, CONFIG_CFL_LOG_LEVEL);

#define CFL_SERVICE_LOG_VER(...) LOG_INST_DBG(LOG_INSTANCE_PTR(cfl, service), __VA_ARGS__)
#define CFL_SERVICE_LOG_DBG(...) LOG_INST_DBG(LOG_INSTANCE_PTR(cfl, service), __VA_ARGS__)
#define CFL_SERVICE_LOG_INF(...) LOG_INST_INF(LOG_INSTANCE_PTR(cfl, service), __VA_ARGS__)
#define CFL_SERVICE_LOG_WRN(...) LOG_INST_WRN(LOG_INSTANCE_PTR(cfl, service), __VA_ARGS__)
#define CFL_SERVICE_LOG_ERR(...) LOG_INST_ERR(LOG_INSTANCE_PTR(cfl, service), __VA_ARGS__)

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

/* Private Helper Functions */

static danp_packet_t *create_nack_packet(uint16_t msg_id, uint16_t msg_seq, int32_t error_code)
{
    danp_packet_t *pkt = danp_buffer_get();
    if (pkt == NULL)
    {
        CFL_SERVICE_LOG_ERR("Failed to allocate NACK packet");
        return NULL;
    }

    cfl_message_t *msg = (cfl_message_t *)pkt->payload;
    msg->sync = CFL_SYNC_WORD;
    msg->version = CFL_VERSION;
    msg->flags = CFL_F_NACK;
    msg->cmd_id = msg_id;
    msg->seq = msg_seq;
    msg->length = sizeof(error_code);
    memcpy(msg->data, &error_code, msg->length);
    pkt->length = CFL_HEADER_SIZE + msg->length;

    return pkt;
}

static danp_packet_t *create_ack_packet(uint16_t msg_id, uint16_t msg_seq)
{
    danp_packet_t *pkt = danp_buffer_get();
    if (pkt == NULL)
    {
        CFL_SERVICE_LOG_ERR("Failed to allocate ACK packet");
        return NULL;
    }

    cfl_message_t *msg = (cfl_message_t *)pkt->payload;
    msg->sync = CFL_SYNC_WORD;
    msg->version = CFL_VERSION;
    msg->flags = CFL_F_ACK;
    msg->cmd_id = msg_id;
    msg->seq = msg_seq;
    msg->length = 0;
    pkt->length = CFL_HEADER_SIZE + msg->length;

    return pkt;
}

static danp_packet_t *create_reply_packet(
    uint16_t msg_id,
    uint16_t msg_seq,
    danp_packet_t *pkt)
{
    if (pkt == NULL)
    {
        CFL_SERVICE_LOG_ERR("Failed to allocate reply packet");
        return NULL;
    }

    cfl_message_t *msg = (cfl_message_t *)pkt->payload;
    msg->sync = CFL_SYNC_WORD;
    msg->version = CFL_VERSION;
    msg->flags = CFL_F_RPLY;
    msg->cmd_id = msg_id;
    msg->seq = msg_seq;
    msg->length = pkt->length - CFL_HEADER_SIZE;
}

static uint8_t *custom_malloc(size_t size)
{
    uint8_t *buffer = NULL;
    danp_packet_t *pkt = NULL;

    for (;;)
    {
        if (size > DANP_MAX_PACKET_SIZE)
        {
            CFL_SERVICE_LOG_ERR("Requested size too large for custom malloc! Requested: %u, Max: %u", size, DANP_MAX_PACKET_SIZE);
            return NULL;
        }

        pkt = danp_buffer_get();
        if (NULL == pkt)
        {
            CFL_SERVICE_LOG_ERR("Failed to allocate packet for custom malloc");
            return NULL;
        }

        buffer = pkt->payload;

        break;
    }

    return buffer;
}

static void setup_tmtc_args(
    struct tmtc_args *rqst,
    struct tmtc_args *rply,
    cfl_message_t *rqst_msg,
    uint16_t rqst_pkt_len)
{
    rqst->hdr_len = CFL_HEADER_SIZE;
    rqst->data = (uint8_t *)rqst_msg;
    rqst->len = rqst_pkt_len;
    rqst->ops.malloc = NULL;

    rqst->incomplete = false;

    rply->hdr_len = CFL_HEADER_SIZE;
    rply->data = NULL;
    rply->len = 0;
    rply->incomplete = false;
    rply->ops.malloc = custom_malloc;
}

static int32_t handle_request_message(
    danp_packet_t *rqst_pkt,
    cfl_message_t *rqst_msg,
    danp_packet_t **rply_pkt,
    danp_packet_t **status_pkt)
{
    int32_t ret = 0;
    const struct tmtc_cmd_handler *handler = NULL;
    struct tmtc_args rqst = {0};
    struct tmtc_args rply = {0};

    CFL_SERVICE_LOG_DBG("Handling request message");

    /* Find handler for this request ID */
    handler = tmtc_get_cmd_handler(rqst_msg->cmd_id);
    if (NULL == handler)
    {
        ret = -EINVAL;
        CFL_SERVICE_LOG_ERR("No handler found for request ID: %d", rqst_msg->cmd_id);
        /* No handler - send NACK */
        *status_pkt = create_nack_packet(rqst_msg->cmd_id, rqst_msg->seq, ret);
        if (*status_pkt == NULL)
        {
            ret = -ENOMEM;
        }
        return ret;
    }

    CFL_SERVICE_LOG_DBG("Executing handler for request ID: %d", rqst_msg->cmd_id);
    setup_tmtc_args(&rqst, &rply, rqst_msg, rqst_pkt->length);

    ret = tmtc_run_handler(handler, &rqst, &rply);
    if (ret < 0)
    {
        CFL_SERVICE_LOG_ERR("Handler execution failed with error: %d", ret);
        *status_pkt = create_nack_packet(rqst_msg->cmd_id, rqst_msg->seq, ret);
        if (*status_pkt == NULL)
        {
            ret = -ENOMEM;
        }
        return ret;
    }

    if (NULL == rply.data)
    {
        *status_pkt = create_ack_packet(rqst_msg->cmd_id, rqst_msg->seq);
        if (*status_pkt == NULL)
        {
            ret = -ENOMEM;
        }
    }
    else
    {
        danp_packet_t *local_rply_pkt = (danp_packet_t *)((uint8_t *)rply.data - offsetof(danp_packet_t, payload));
        local_rply_pkt->length = rply.len;

        create_reply_packet(rqst_msg->cmd_id, rqst_msg->seq, local_rply_pkt);
        *rply_pkt = local_rply_pkt;
    }

    return ret;
}

static int32_t handle_push_message(danp_packet_t *rqst_pkt, cfl_message_t *rqst_msg)
{
    int32_t ret = 0;
    const struct tmtc_cmd_handler *handler = NULL;
    struct tmtc_args rqst = {0};
    struct tmtc_args rply = {0};

    CFL_SERVICE_LOG_DBG("Handling push message");

    /* Find handler for this push ID */
    handler = tmtc_get_cmd_handler(rqst_msg->cmd_id);
    if (NULL == handler)
    {
        CFL_SERVICE_LOG_ERR("No handler found for push ID: %d", rqst_msg->cmd_id);
        /* No handler - ignore push */
        return ret;
    }

    CFL_SERVICE_LOG_DBG("Executing handler for push ID: %d", rqst_msg->cmd_id);
    setup_tmtc_args(&rqst, &rply, rqst_msg, rqst_pkt->length);

    ret = tmtc_run_handler(handler, &rqst, &rply);

    /* Push messages do not expect a reply */
    if (NULL != rply.data)
    {
        CFL_SERVICE_LOG_WRN("Handler returned unexpected reply data for push message");
    }

    return ret;
}

static int32_t send_cfl_message(
    uint16_t dst_node,
    uint16_t dst_port,
    uint16_t id,
    uint8_t flags,
    const uint8_t *payload,
    uint16_t payload_len)
{
    int32_t ret = 0;
    danp_packet_t *pkt = NULL;
    cfl_message_t *msg = NULL;
    int32_t sent_len = 0;

    if (!context.initialized)
    {
        CFL_SERVICE_LOG_ERR("Service not initialized");
        return -EAGAIN;
    }

    if (payload_len > 0 && payload == NULL)
    {
        CFL_SERVICE_LOG_ERR("Payload is NULL but length is non-zero");
        return -EINVAL;
    }

    pkt = danp_buffer_get();
    if (pkt == NULL)
    {
        CFL_SERVICE_LOG_ERR("Failed to allocate packet buffer");
        return -ENOMEM;
    }

    msg = (cfl_message_t *)pkt->payload;
    msg->sync = CFL_SYNC_WORD;
    msg->version = CFL_VERSION;
    msg->flags = flags;
    msg->cmd_id = id;
    msg->seq = 0; /* Sequence number tracking not implemented */
    msg->length = payload_len;
    if (msg->length > 0)
    {
        memcpy(msg->data, payload, msg->length);
    }
    pkt->length = CFL_HEADER_SIZE + msg->length;

    sent_len = danp_send_packet_to(context.socket, pkt, dst_node, dst_port);
    if (sent_len < 0)
    {
        CFL_SERVICE_LOG_ERR("Failed to send packet");
        return -EIO;
    }

    CFL_SERVICE_LOG_DBG("Sent message to node %d port %d, id %d", dst_node, dst_port, id);
    return ret;
}

/* Public Functions */

static int32_t cfl_process_message(
    danp_packet_t *rqst_pkt,
    danp_packet_t **rply_pkt,
    danp_packet_t **status_pkt)
{
    int32_t ret = 0;
    cfl_message_t *rqst_msg = NULL;

    CFL_SERVICE_LOG_DBG("Processing message");

    if (rqst_pkt == NULL || rqst_pkt->length < CFL_HEADER_SIZE)
    {
        CFL_SERVICE_LOG_ERR("Request packet is NULL or too short");
        return -EINVAL;
    }

    rqst_msg = (cfl_message_t *)rqst_pkt->payload;

    /* Validate complete message including CRC */
    if (rqst_pkt->length != (CFL_HEADER_SIZE + rqst_msg->length))
    {
        CFL_SERVICE_LOG_ERR("Incomplete message received");
        return -EINVAL;
    }

    /* Handle based on message type */
    if (rqst_msg->flags & CFL_F_RQST)
    {
        ret = handle_request_message(rqst_pkt, rqst_msg, rply_pkt, status_pkt);
    }
    else if (rqst_msg->flags & CFL_F_PUSH)
    {
        ret = handle_push_message(rqst_pkt, rqst_msg);
    }
    else
    {
        CFL_SERVICE_LOG_ERR("Unknown message flag");
        ret = -EINVAL;
    }

    CFL_SERVICE_LOG_DBG("Message processing completed with result: %d", ret);
    return ret;
}

static void cfl_service_danp_rx_task(void *arg)
{
    cfl_service_danp_ctx_t *ctx = (cfl_service_danp_ctx_t *)arg;
    danp_packet_t *rqst_pkt = NULL;
    danp_packet_t *rply_pkt = NULL;
    danp_packet_t *status_pkt = NULL;
    uint16_t src_node = 0;
    uint16_t src_port = 0;

    while (ctx->running)
    {
        if (NULL != rqst_pkt)
        {
            CFL_SERVICE_LOG_DBG("Freeing previous request packet");
            danp_buffer_free(rqst_pkt);
            rqst_pkt = NULL;
        }

        rply_pkt = NULL;
        status_pkt = NULL;
        rqst_pkt = danp_recv_packet_from(ctx->socket, &src_node, &src_port, CFL_DANP_RX_TIMEOUT_MS);

        if (NULL != rqst_pkt)
        {
            CFL_SERVICE_LOG_DBG("Received packet from node: %d, port: %d", src_node, src_port);
            cfl_process_message(rqst_pkt, &rply_pkt, &status_pkt);
        }

        if (NULL != status_pkt)
        {
            CFL_SERVICE_LOG_DBG("Sending status packet to node: %d, port: %d", src_node, src_port);
            danp_send_packet_to(ctx->socket, status_pkt, src_node, src_port);
        }

        if (NULL != rply_pkt)
        {
            CFL_SERVICE_LOG_DBG("Sending reply packet to node: %d, port: %d", src_node, src_port);
            danp_send_packet_to(ctx->socket, rply_pkt, src_node, src_port);
        }
    }

    CFL_SERVICE_LOG_DBG("RX task exiting");
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
        if (config == NULL)
        {
            CFL_SERVICE_LOG_ERR("Configuration is NULL");
            ret = -EINVAL;
            break;
        }

        if (context.initialized)
        {
            CFL_SERVICE_LOG_ERR("Service already initialized");
            ret = -EALREADY;
            break;
        }

        memset(&context, 0, sizeof(context));
        context.local_port = config->port_id;

        context.socket = danp_socket(DANP_TYPE_DGRAM);
        if (context.socket == NULL)
        {
            CFL_SERVICE_LOG_ERR("Failed to create socket");
            ret = -ENOMEM;
            break;
        }
        is_socket_created = true;

        ret = danp_bind(context.socket, context.local_port);
        if (ret < 0)
        {
            CFL_SERVICE_LOG_ERR("Failed to bind socket");
            ret = -EADDRNOTAVAIL;
            break;
        }
        context.running = true;

        context.rx_task_handle = osal_thread_create(cfl_service_danp_rx_task, &context, &task_attr);
        if (context.rx_task_handle == NULL)
        {
            CFL_SERVICE_LOG_ERR("Failed to create RX task");
            ret = -ENOMEM;
            break;
        }

        CFL_SERVICE_LOG_INF("CFL service over DANP initialized on port %d", context.local_port);

        context.initialized = true;
        break;
    }

    if (0 != ret)
    {
        if (is_socket_created && context.socket != NULL)
        {
            CFL_SERVICE_LOG_DBG("Closing socket due to initialization failure");
            danp_close(context.socket);
            context.socket = NULL;
        }

        context.initialized = false;
        context.running = false;
    }

    return ret;
}

int32_t cfl_service_danp_deinit(void)
{
    int32_t ret = 0;

    for (;;)
    {
        CFL_SERVICE_LOG_DBG("Deinitializing CFL service over DANP");

        if (!context.initialized)
        {
            CFL_SERVICE_LOG_ERR("Service not initialized");
            ret = -EINVAL;
            break;
        }

        CFL_SERVICE_LOG_DBG("Signaling RX task to stop");
        context.running = false;
        osal_delay_ms(CFL_DANP_RX_TIMEOUT_MS * 2);

        if (context.socket != NULL)
        {
            CFL_SERVICE_LOG_DBG("Closing socket");
            danp_close(context.socket);
            context.socket = NULL;
        }

        memset(&context, 0, sizeof(context));
        break;
    }

    return ret;
}

int32_t cfl_service_danp_send_request(
    uint16_t dst_node,
    uint16_t dst_port,
    uint16_t id,
    const uint8_t *payload,
    uint16_t payload_len,
    uint16_t *seq_out)
{
    (void)seq_out; /* Sequence number tracking not implemented */
    return send_cfl_message(dst_node, dst_port, id, CFL_F_RQST, payload, payload_len);
}

int32_t cfl_service_danp_send_push(
    uint16_t dst_node,
    uint16_t dst_port,
    uint16_t id,
    const uint8_t *payload,
    uint16_t payload_len)
{
    return send_cfl_message(dst_node, dst_port, id, CFL_F_PUSH, payload, payload_len);
}