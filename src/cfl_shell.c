/* cli.c - one line definition */

/* All Rights Reserved */

/* Includes */

#include <stdlib.h>

#include <zephyr/shell/shell.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "cfl/cfl_utilities.h"
#include "danp/danp_defs.h"

/* Imports */


/* Definitions */

LOG_MODULE_DECLARE(cfl);

/* Types */


/* Forward Declarations */

static int tmtc_shell_transaction(const struct shell *shell, size_t argc, char **argv);
static int tmtc_shell_test(const struct shell *shell, size_t argc, char **argv);
static int tmtc_shell_stats(const struct shell *shell, size_t argc, char **argv);

/* Variables */

SHELL_STATIC_SUBCMD_SET_CREATE(sub_tmtc_cmds,
    SHELL_CMD(
        transaction,
        NULL,
        "Send/receive message\nUsage: tmtc transaction <dest_id> <tmtc_id> [<data_hex>] [<timeout>]",
        tmtc_shell_transaction
    ),
    SHELL_CMD(
        test,
        NULL,
        "Run TMTC test (not implemented yet)\nUsage: tmtc test <dest_id> <interval>",
        tmtc_shell_test
    ),
    SHELL_CMD(
        stats,
        NULL,
        "Print TMTC statistics",
        tmtc_shell_stats
    ),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(tmtc, &sub_tmtc_cmds, "Base command for TMTC operations", NULL);

/* Functions */

static int tmtc_shell_transaction(const struct shell *shell, size_t argc, char **argv)
{
    if (argc < 3) {
        shell_print(shell, "Usage: tmtc transaction <id> <tmtc_id> [<dataHex>] [<timeout>]");
        return -EINVAL;
    }

    int32_t ret = 0;
    uint16_t dest_id = (uint16_t)atoi(argv[1]);
    uint16_t tmtc_id = (uint16_t)atoi(argv[2]);
    uint8_t request[DANP_MAX_PACKET_SIZE];
    uint8_t reply[DANP_MAX_PACKET_SIZE];
    size_t data_len = 0;

    // Parse data if provided
    if (argc >= 4) {
        const char *dataHex = argv[3];
        data_len = strlen(dataHex) / 2;
        for (size_t i = 0; i < data_len; i++) {
            sscanf(&dataHex[i * 2], "%2hhx", &request[i]);
        }
    }

    ret = tmtc_transaction(
        dest_id,
        tmtc_id,
        request,
        (uint16_t)data_len,
        reply,
        sizeof(reply),
        2000);
    if (ret < 0) {
        shell_error(shell, "TMTC Transaction failed with error %d", ret);
        return ret;
    }

    shell_print(shell, "Received %d bytes:", ret);
    for (int i = 0; i < ret; i++) {
        shell_print(shell, "%02X ", reply[i]);
    }
    shell_print(shell, "\n");

    return 0;
}

static int tmtc_shell_test(const struct shell *shell, size_t argc, char **argv)
{
    shell_print(shell, "TMTC test not implemented yet");
    return 0;
}
static int tmtc_shell_stats(const struct shell *shell, size_t argc, char **argv)
{
    shell_print(shell, "TMTC statistics not implemented yet");
    return 0;
}