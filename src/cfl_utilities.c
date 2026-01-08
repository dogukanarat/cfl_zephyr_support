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
        rqst_msg->sync = CFL_SYNC_WORD;
        rqst_msg->version = CFL_VERSION;
        rqst_msg->flags = CFL_F_RQST;
        rqst_msg->id = tmtc_id;
        rqst_msg->seq = 0;
        rqst_msg->length = request_len;
        memcpy(rqst_msg->data, request, rqst_msg->length);
        rqst_pkt->length = CFL_HEADER_SIZE + rqst_msg->length;

        ret = tmtc_transaction_packet(dest_id, CONFIG_CFL_SUPPORT_DANP_SERVICE_PORT, rqst_pkt, &received_pkt, timeout);
        if (ret < 0)
        {
            break;
        }

        received_msg = (cfl_message_t *)received_pkt->payload;
        received_len = received_pkt->length;

        if (received_msg->length + CFL_HEADER_SIZE != received_len)
        {
            LOG_ERR("Received packet length mismatch");
            ret = -2; // Length mismatch
            break;
        }

        if (received_msg->flags & CFL_F_NACK)
        {
            LOG_ERR("Received NACK for request ID: %d", received_msg->id);
            status_msg = received_msg;
            ret = -5;
            break;
        }
        else if (received_msg->flags & CFL_F_ACK)
        {
            LOG_INF("Received ACK for request ID: %d", received_msg->id);
            status_msg = received_msg;
            ret = 0;
            break;
        }
        else if (received_msg->flags & CFL_F_RPLY)
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
