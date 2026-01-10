/* cli.c - one line definition */

/* All Rights Reserved */

/* Includes */

#include <stdlib.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/shell/shell.h>

#include "cfl/cfl_utilities.h"
#include "danp/danp_defs.h"

/* Imports */


/* Definitions */

LOG_MODULE_DECLARE(cfl);

#define TMTC_SHELL_DEFAULT_TIMEOUT_MS 1000

/* Types */

typedef struct cfl_shell_data_s
{
    uint8_t rqst[DANP_MAX_PACKET_SIZE];
    size_t rqst_len;
    uint8_t rply[DANP_MAX_PACKET_SIZE];
    size_t rply_len;
} cfl_shell_data_t;

/* Forward Declarations */

static int cfl_shell_transaction(const struct shell *shell, size_t argc, char **argv);
static int cfl_shell_test(const struct shell *shell, size_t argc, char **argv);
static int cfl_shell_stats(const struct shell *shell, size_t argc, char **argv);

/* Variables */

SHELL_STATIC_SUBCMD_SET_CREATE(
    sub_cfl_cmds,
    SHELL_CMD(
        transaction,
        NULL,
        "Send/receive message\nUsage: cfl transaction <dest_id> <cmd_id> [<data_hex>] "
        "[<timeout>]",
        cfl_shell_transaction),
    SHELL_CMD(
        test,
        NULL,
        "Run CFL test (not implemented yet)\nUsage: cfl test <dest_id> <interval>",
        cfl_shell_test),
    SHELL_CMD(stats, NULL, "Print CFL statistics", cfl_shell_stats),
    SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(cfl, &sub_cfl_cmds, "Base command for CFL operations", NULL);

static cfl_shell_data_t shell_data;

/* Functions */

static int cfl_shell_transaction(const struct shell *shell, size_t argc, char **argv)
{
    if (argc < 3)
    {
        shell_print(shell, "Usage: tmtc transaction <dest_id> <cmd_id> [<dataHex>] [<timeout>]");
        return -EINVAL;
    }

    int32_t ret = 0;
    uint16_t dest_id = (uint16_t)atoi(argv[1]);
    uint16_t cmd_id = (uint16_t)atoi(argv[2]);

    shell_data.rqst_len = 0;
    shell_data.rply_len = 0;

    // Parse data if provided
    if (argc >= 4)
    {
        const char *dataHex = argv[3];
        if (strlen(dataHex) % 2 != 0)
        {
            shell_error(shell, "Data hex string length must be even");
            return -EINVAL;
        }
        shell_data.rqst_len = strlen(dataHex) / 2;
        for (size_t i = 0; i < shell_data.rqst_len; i++)
        {
            sscanf(&dataHex[i * 2], "%2hhx", &shell_data.rqst[i]);
        }
        shell_print(shell, "Request data (%d bytes):", (int)shell_data.rqst_len);
        shell_hexdump(shell, shell_data.rqst, shell_data.rqst_len);
    }
    else
    {
        shell_print(shell, "No request data provided");
    }

    ret = cfl_transaction(
        dest_id,
        cmd_id,
        shell_data.rqst,
        (uint16_t)shell_data.rqst_len,
        shell_data.rply,
        sizeof(shell_data.rply),
        TMTC_SHELL_DEFAULT_TIMEOUT_MS);
    if (ret < 0)
    {
        shell_error(shell, "TMTC Transaction failed with error %d", ret);
        return ret;
    }
    shell_data.rply_len = (size_t)ret;

    shell_print(shell, "Reply data (%d bytes):", shell_data.rply_len);
    shell_hexdump(shell, shell_data.rply, shell_data.rply_len);

    return 0;
}

static int cfl_shell_test(const struct shell *shell, size_t argc, char **argv)
{
    shell_print(shell, "CFL test not implemented yet");
    return 0;
}

static int cfl_shell_stats(const struct shell *shell, size_t argc, char **argv)
{
    shell_print(shell, "CFL statistics not implemented yet");
    return 0;
}