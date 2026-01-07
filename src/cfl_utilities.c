/* cfl_utilities.c - one line definition */

/* All Rights Reserved */

/* Includes */

#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

#include "danp/danp.h"
#include "danp/danp_buffer.h"

#include "cfl/cfl.h"
#include "cfl/cfl_utilities.h"

/* Imports */


/* Definitions */

LOG_MODULE_DECLARE(cfl);

/* Types */


/* Forward Declarations */


/* Variables */


/* Functions */

static int32_t tmtc_transaction_packet(
    uint16_t dest_id,
    uint16_t dest_port,
    danp_packet_t *rqst_pkt,
    danp_packet_t **received_pkt,
    uint32_t timeout)
{
    int32_t ret = 0;
    danp_socket_t *sock = NULL;
    bool is_sock_created = false;
    int32_t sent_len = 0;

    for (;;)
    {
        if (NULL == received_pkt)
        {
            ret = -1; // Invalid argument
            LOG_ERR("Received packet pointer is NULL");
            break;
        }

        sock = danp_socket(DANP_TYPE_DGRAM);
        if (!sock)
        {
            ret = -1; // Socket creation failed
            LOG_ERR("Failed to create socket");
            break;
        }
        is_sock_created = true;

        sent_len = danp_send_packet_to(sock, rqst_pkt, dest_id, dest_port);
        if (sent_len < 0)
        {
            ret = -3; // Send failed
            LOG_ERR("Failed to send request packet");
            break;
        }

        *received_pkt = danp_recv_packet(sock, timeout);
        if (NULL == *received_pkt)
        {
            LOG_ERR("Failed to receive status packet");
            ret = -4; // Receive failed
            break;
        }

        ret = (size_t)1; // Actual packet received

        LOG_DBG("Transaction completed successfully");

        break;
    }

    if (is_sock_created && sock)
    {
        danp_close(sock);
    }

    return ret;
}

int32_t tmtc_transaction(
    uint16_t dest_id,
    uint16_t tmtc_id,
    const uint8_t *request,
    uint16_t request_len,
    uint8_t *reply,
    uint16_t reply_size,
    uint32_t timeout)
{
    int32_t ret = 0;
    cfl_status_t cfl_ret = CFL_OK;
    danp_packet_t *rqst_pkt = NULL;
    danp_packet_t *received_pkt = NULL;
    cfl_message_t *rqst_msg = NULL;
    cfl_message_t *status_msg = NULL;
    cfl_message_t *rply_msg = NULL;
    cfl_message_t *received_msg = NULL;
    uint16_t received_len = 0;
    
    for (;;)
    {
        rqst_pkt = danp_buffer_get();
        if (rqst_pkt == NULL)
        {
            ret = -ENOMEM;
            break;
        }

        rqst_msg = (cfl_message_t *)rqst_pkt->payload;

        cfl_message_init(rqst_msg, tmtc_id, CFL_F_RQST);
        memcpy(rqst_msg->data, request, request_len);
        cfl_message_set_length(rqst_msg, request_len);
        cfl_message_compute_crc(rqst_msg);
        rqst_pkt->length = CFL_HEADER_SIZE + request_len;

        ret = tmtc_transaction_packet(dest_id, CONFIG_CFL_SUPPORT_DANP_SERVICE_PORT, rqst_pkt, &received_pkt, timeout);
        if (ret < 0)
        {
            break;
        }

        received_msg = (cfl_message_t *)received_pkt->payload;
        received_len = received_pkt->length;

        cfl_ret = cfl_message_validate(received_msg, received_len);
        if (CFL_OK != cfl_ret)
        {
            LOG_ERR("Status message validation failed with error: %d", cfl_ret);
            ret = -2;
            break;
        }

        if (cfl_message_has_flag(received_msg, CFL_F_NACK))
        {
            LOG_ERR("Received NACK for request ID: %d", received_msg->id);
            status_msg = received_msg;
            ret = -5;
            break;
        }
        else if (cfl_message_has_flag(received_msg, CFL_F_ACK))
        {
            LOG_INF("Received ACK for request ID: %d", received_msg->id);
            status_msg = received_msg;
            ret = 0;
            break;
        }
        else if (cfl_message_has_flag(received_msg, CFL_F_RPLY))
        {
            LOG_INF("Received reply for request ID: %d", received_msg->id);
            rply_msg = received_msg;
        }
        else
        {
            LOG_ERR("Unknown message flag");
            ret = -3;
            break;
        }

        if (rply_msg == NULL)
        {
            LOG_ERR("Reply message is NULL");
            ret = -4;
            break;
        }

        if (reply == NULL || reply_size == 0)
        {
            ret = rply_msg->length;
            break;
        }

        if (rply_msg->length > reply_size)
        {
            LOG_ERR("Reply data exceeds buffer size");
            ret = -6;
            break;
        }

        memcpy(reply, &rply_msg->data[0], rply_msg->length);
        ret = rply_msg->length;

        break;
    }

    if (received_pkt != NULL)
    {
        danp_buffer_free(received_pkt);
    }

    return ret;
}
